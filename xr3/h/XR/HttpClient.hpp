//
// Nuclear Heart Games
// XRhodes
//
// HttpClient.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    17/10/12
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_HTTPCLIENT_HPP
#define XR_HTTPCLIENT_HPP

#include <set>
#include "types.hpp"

namespace XR
{

//==============================================================================
class HttpJob;

//==============================================================================
class HttpClient
{
  XR_NONCOPY_DECL(HttpClient)

public:
  // typedef
  enum  Status
  {
    READY,
    BUSY
  };

  enum  Response
  {
    RESP_OK = 200,
    RESP_BAD_REQUEST = 400,
    RESP_UNAUTHORIZED,
    RESP_PAYMENT_REQUIRED,
    RESP_FORBIDDEN,
    RESP_NOT_FOUND,
    RESP_METHOD_NOT_ALLOWED
  };

  typedef void(*ResponseCallback)(uint32 response, uint32 len, char* pData,
    void* pCbData);

  // static
  static const uint32 kDefaultChunkSize = 1024;
  static const uint32 kUriLen = 256;

  static std::string Urlify(const char** parKeyValues, bool encodeValues);

  // data
  HttpClient();
  ~HttpClient();

  // general
  bool SetHeader(const char* pKey, const char* pValue, bool oneOff);
  bool SetHeader(const char* pKey, int value, bool oneOff);

  bool  RequestPost(const char* pUri, const char* pBody, const char** parKeyValues,
    ResponseCallback pResponseCb, void* pCbData, bool force = false);
  bool  RequestGet(const char* pUri, ResponseCallback pResponseCb, void* pCbData,
    bool force = false);

  // internal
  void  RemoveJob(HttpJob* pJob);
  void  ResetStatus();
  void  ResetHeaders();

  // static
  static int32 ReceivedData(void* pSystem, void* pUser);

protected:
  // types
  typedef std::set<HttpJob*>        JobSet;
  typedef std::set<std::string> StringSet;

  // data
  Status      m_status;
  void*       m_pHttpImpl;  // yes ownership
  JobSet      m_jobs;

  std::string m_phpSessid;

  StringSet   m_clearHeaders;
};

//==============================================================================
class HttpJob 
{
public:
  // data
  void*                         pHttpImpl;  // no ownership
  HttpClient*                   pClient;  // no ownership
  uint32                        bufferLen;
  char*                         pBuffer;
  HttpClient::ResponseCallback  pResponseCb;
  void*                         pCbData;  // no ownership
  std::string                   phpSessionId;

  // structors
  HttpJob(HttpClient* pClient, void* pHttpImpl,
    HttpClient::ResponseCallback pResponseCb, void* pCbData);
  ~HttpJob();

  // general
  void  Complete();
};

} // XR

#endif // XR_HTTPCLIENT_HPP