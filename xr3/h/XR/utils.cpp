#include <cstring>
#include <algorithm>
#include "utils.hpp"
#include "Hash.hpp"

namespace XR
{

int FindHashedListStringValue( const uint32* parValues, int numValues, const char* pValue )
{
  XR_ASSERT(GetIdFromList, parValues != 0);
  XR_ASSERT(GetIdFromList, numValues >= 0);
  int id(-1);
  if (pValue != 0)
  {
    const uint32* pFind(std::find(parValues, parValues + numValues,
      Hash::String(pValue)));

    id = pFind - parValues;
  }
  return id;
}

//void  MaterialiseTextures(const Material& mat, CIwResGroup* pGroup)
//{
//  XR_ASSERT(RingBuffer, pGroup != 0);
//  CIwResList* pList(pGroup->GetListNamed(IW_GX_RESTYPE_TEXTURE));
//
//  if (pList != 0)
//  {
//    CIwManagedList& lTextures(pList->m_Resources);
//
//    CIwManaged**  i0 = lTextures.GetBegin();
//    CIwManaged**  i1 = lTextures.GetEnd();
//
//    int32 hash(::Hash::String(IW_GX_RESTYPE_MATERIAL));
//    while (i0 != i1)
//    {
//      CIwTexture*   pTexture(static_cast<CIwTexture*>(*i0));
//      Material*  pMaterial(new Material);
//
//      pMaterial->Copy(mat);
//      pMaterial->m_Hash = pTexture->m_Hash;
//      pMaterial->SetTexture(pTexture);
//
//      pGroup->AddRes(hash, pMaterial);
//      ++i0;
//    }
//  }
//}

std::string Char2Hex( char dec )
{
  char dig1 = (dec&0xF0) >> 4;
  char dig2 = (dec&0x0F);
  if (0 <= dig1 && dig1 <= 9) dig1 += '0'; //0,48 in ascii
  if (10 <= dig1 && dig1 <=15) dig1 += 'A'-10; //a,97 in ascii
  if (0 <= dig2 && dig2 <= 9) dig2 += '0';
  if (10 <= dig2 && dig2 <=15) dig2 += 'A'-10;
  std::string r;
  r.append(&dig1, 1);
  r.append(&dig2, 1);
  return r;
}

std::string UrlEncode( const char* pString )
{
  std::ostringstream escaped;
  int max = strlen(pString);
  for (int i=0; i < max; ++i)
  {
    if ( (48 <= pString[i] && pString[i] <= 57) ||  //0-9
      (65 <= pString[i] && pString[i] <= 90) || //abc...xyz
      (97 <= pString[i] && pString[i] <= 122) || //ABC...XYZ
      (pString[i]=='-' || pString[i]=='_' || pString[i]=='.' || pString[i]=='~')
      /*(	c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'' ||
      c[i]=='$' || c[i]=='-' || c[i]=='_' || c[i]=='.' || c[i]=='+' )*/
      )
    {
      escaped << pString[i];
    }
    else
    {
      escaped << '%';
      escaped << Char2Hex(pString[i]);	//converts char 255 to string "ff"
    }
  }
  return escaped.str();
}


} // XR
