#include <sstream>
#include <IwHTTP.h>
#include "HttpClient.hpp"
#include "utils.hpp"

namespace XR
{

struct CIwHttpFix: public CIwHTTP 
{
  // static
  static const char*  kPhpSessId;

  static int32 ReceivedHeaders(void* pSystem, void* pUser);
  
  // structors
  CIwHttpFix();
  ~CIwHttpFix();
};


const char* CIwHttpFix::kPhpSessId = "PHPSESSID=";


int32 CIwHttpFix::ReceivedHeaders( void* pSystem, void* pUser )
{
  HttpJob*    pJob(static_cast<HttpJob*>(pUser));
  CIwHttpFix* pHttp(static_cast<CIwHttpFix*>(pSystem));

  bool  success(pHttp->GetStatus() == S3E_RESULT_SUCCESS);
  if (success)
  {
    switch(pHttp->GetResponseCode())
    {
    case  200:
      {
        uint32 len = pHttp->ContentExpected();
        if (len == 0)
        {
          len = HttpClient::kDefaultChunkSize;
        }

        const char* pPhpSessid(strstr(pHttp->m_response.c_str(), kPhpSessId));
        if (pPhpSessid != 0)
        {
          const char* pEnd(strchr(pPhpSessid, ';'));
          if (pEnd != 0)
          {
            pJob->phpSessionId.assign(pPhpSessid, pEnd - pPhpSessid);
          }
          else
          {
            pJob->phpSessionId.assign(pPhpSessid);
          }
        }

        pJob->bufferLen = len;
        pJob->pBuffer = (char*)(s3eMalloc(len + 1));
        pJob->pBuffer[len] = 0;

        pHttp->ReadDataAsync(pJob->pBuffer, len, 0, HttpClient::ReceivedData, pJob);
      }
      break;

    case  302:
      {
        std::string location;
        pHttp->GetHeader("location", location);

        pHttp->Cancel();
        pHttp->Get(location.c_str(), ReceivedHeaders, pUser);
      }
      break;

    default:
      success = false;
      break;
    }
  }

  if(!success)
  {
    pJob->pClient->ResetStatus();
    XR_TRACE(HttpClient, ("Request failed receiving headers."));
    pJob->Complete();
    delete pJob;
  }

  return 0;
}

CIwHttpFix::CIwHttpFix()
: CIwHTTP()
{}

CIwHttpFix::~CIwHttpFix()
{}



int32 HttpClient::ReceivedData( void* pSystem, void* pUser )
{
  HttpJob*  pJob(static_cast<HttpJob*>(pUser));
  CIwHTTP&  http(*static_cast<CIwHTTP*>(pJob->pHttpImpl));

  bool  success(http.GetStatus() == S3E_RESULT_SUCCESS);
  if (success)
  {
    if (http.ContentReceived() != http.ContentLength())
    {
      uint32  len(pJob->bufferLen);
      if (len < http.ContentExpected())
      {
        len = http.ContentExpected();
      }
      else
      {
        len += kDefaultChunkSize;
      }

      if (len != pJob->bufferLen)
      {
        //pJob->pClient->m_pool.Alloc(len);
        pJob->pBuffer = (char*)(s3eRealloc(pJob->pBuffer, len));
      }
      
      http.ReadDataAsync(pJob->pBuffer + pJob->bufferLen,
        len - pJob->bufferLen, 0, ReceivedData, pJob);

      // update buffer size
      pJob->bufferLen = len;
    }
    else
    {
      // a-ok
      pJob->pClient->ResetStatus();
      XR_TRACE(HttpClient, ("Request successfully completed, %d bytes received", pJob->bufferLen));
      pJob->Complete();
      delete pJob;
    }
  }
  else
  {
    pJob->pClient->ResetStatus();
    XR_TRACE(HttpClient, ("Request failed after receiving headers."));
    pJob->Complete();
    delete pJob;
  }

  return 0;
}


std::string HttpClient::Urlify( const char** parKeyValues, bool encodeValues )
{
  XR_ASSERT(HttpClient, parKeyValues != 0);

  int numKeyValues(CountArrayItems(parKeyValues));
  XR_ASSERTMSG(HttpClient, (numKeyValues & 1) == 0,
    ("Undefined value: an even number of items is required, found: %d", numKeyValues));

  std::ostringstream  buf;
  for (int i = 0; i < numKeyValues;) 
  {
    buf << parKeyValues[i] << '=' << 
      (encodeValues ? UrlEncode(parKeyValues[i + 1]) : parKeyValues[i + 1]);

    i += 2;
    if (i < numKeyValues)
    {
      buf << "&";
    }
  }

  return buf.str();
}

HttpClient::HttpClient()
: m_status(READY),
  m_pHttpImpl(new CIwHttpFix),
  m_jobs()
{}

HttpClient::~HttpClient()
{
  CIwHttpFix* pHttp(static_cast<CIwHttpFix*>(m_pHttpImpl));
  pHttp->Cancel();

  delete pHttp;
  pHttp = 0;
}


bool  HttpClient::RequestPost(const char* pUri, const char* pBody,
  const char** parKeyValues, ResponseCallback pResponseCb, void* pCbData,
  bool force)
{
  bool  success(m_status == READY || force);
  if (success)
  {
    XR_TRACE(HttpClient, ("POST request to: %s", pUri));
    HttpJob*  pJob = new HttpJob(this, m_pHttpImpl, pResponseCb, pCbData);

    CIwHttpFix* pHttp(static_cast<CIwHttpFix*>(m_pHttpImpl));
    if (parKeyValues != 0)
    {
      int numKeyValues(CountArrayItems(parKeyValues));
      XR_ASSERTMSG(HttpClient, (numKeyValues & 1) == 0,
        ("Undefined value: an even number of items is required, found: %d", numKeyValues));

      for (int i = 0; i < numKeyValues; i += 2) 
      {
        pHttp->SetFormData(parKeyValues[i], parKeyValues[i + 1]);
      }
    }

    if (m_phpSessid.size() > 0)
    {
      pHttp->SetRequestHeader("Cookie", m_phpSessid);
    }
    
    success = S3E_RESULT_ERROR != pHttp->Post(pUri, pBody, 0,
      CIwHttpFix::ReceivedHeaders, pJob);
    if (success)
    {
      m_status = BUSY;
      m_jobs.insert(pJob);
    }
    else
    {
      XR_TRACE(HttpClient, ("POST request failed."));
      m_status = READY;
      delete pJob;
    }
  }

  return success;
}

bool HttpClient::RequestGet(const char* pUri, ResponseCallback pResponseCb,
  void* pCbData, bool force)
{
  bool  success(m_status == READY || force);
  if (success)
  {
    XR_TRACE(HttpClient, ("GET request to: %s", pUri));

    CIwHttpFix* pHttp(static_cast<CIwHttpFix*>(m_pHttpImpl));
    if (m_phpSessid.size() > 0)
    {
      pHttp->SetRequestHeader("Cookie", m_phpSessid);
    }

    HttpJob*  pJob = new HttpJob(this, m_pHttpImpl, pResponseCb, pCbData);
    success = S3E_RESULT_ERROR !=
      pHttp->Get(pUri, CIwHttpFix::ReceivedHeaders, pJob);

    if (success)
    {
      m_status = BUSY;
      m_jobs.insert(pJob);
    }
    else
    {
      XR_TRACE(HttpClient, ("GET request failed."));
      m_status = READY;
      delete pJob;
    }
  }
  return success;
}

void  HttpClient::RemoveJob( HttpJob* pJob )
{
  m_jobs.erase(pJob);
}

void HttpClient::ResetStatus()
{
  CIwHttpFix* pHttp(static_cast<CIwHttpFix*>(m_pHttpImpl));
  pHttp->Cancel();

  m_status = READY;

  for (StringSet::iterator i0(m_clearHeaders.begin()), i1(m_clearHeaders.end()); i0 != i1; ++i0)
  {
    pHttp->SetRequestHeader(i0->c_str(), "");
  }
}

bool HttpClient::SetHeader( const char* pKey, const char* pValue, bool oneOff )
{
  XR_ASSERT(HttpClient, pKey != 0);
  XR_ASSERT(HttpClient, strlen(pKey) > 0);

  bool  success(m_status == READY);
  if (success)
  {
    if (pValue == 0)
    {
      pValue = "";
    }
    if (strlen(pValue) == 0)
    {
      oneOff = true;
    }
    
    CIwHttpFix* pHttp(static_cast<CIwHttpFix*>(m_pHttpImpl));
    pHttp->SetRequestHeader(pKey, pValue);
    if (oneOff)
    {
      m_clearHeaders.insert(pKey);
    }
  } 
  return success;
}


bool HttpClient::SetHeader( const char* pKey, int value, bool oneOff )
{
  const int kBufferLen = 16;
  char  arBuffer[kBufferLen];
  snprintf(arBuffer, kBufferLen, "%d", value);

  return SetHeader(pKey, arBuffer, oneOff);
}

HttpJob::HttpJob(HttpClient* pClient_, void* pHttpImpl_,
  HttpClient::ResponseCallback pResponseCb_, void* pCbData_)
: pHttpImpl(pHttpImpl_),
  pClient(pClient_),
  bufferLen(0),
  pBuffer(0),
  pResponseCb(pResponseCb_),
  pCbData(pCbData_)
{}

HttpJob::~HttpJob()
{
  s3eFree(pBuffer);
}

void HttpJob::Complete()
{
  CIwHttpFix* pHttp(static_cast<CIwHttpFix*>(pHttpImpl));
  (*pResponseCb)(pHttp->GetResponseCode(), bufferLen, pBuffer, pCbData);

  pClient->RemoveJob(this);
}

} // XR
