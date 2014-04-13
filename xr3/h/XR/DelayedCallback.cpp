#include  "DelayedCallback.hpp"

namespace XR
{

bool DelayedCallback::IsActivePredicate(const DelayedCallback& v)
{
  return v.IsActive();
}

void  DelayedCallback::Update(float tDelta)
{
  delay -= tDelta;
  if(delay <= .0f && pCallback != 0)
  {
    (*pCallback)(pCallbackData);
    pCallback = 0;
  }
}

} //XR