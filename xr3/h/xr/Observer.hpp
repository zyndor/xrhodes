#ifndef XR_OBSERVER_HPP
#define XR_OBSERVER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <list>

namespace xr
{

template  <typename T>
class Observer
{
public:
  typedef T Type;
  typedef void(*Callback)(const Type& value, void* data);

  Observer();
  ~Observer();

  void  Register(const Type& value, Callback callback, void* data);
  void  Unregister(const Type& value, Callback callback);

  void  Update();

protected:
  // types
  struct  Record
  {
    const Type* value;
    Callback    callback;
    void*       data;
    Type        lastValue;

    bool  IsChanged() const;
    void  Process();

    bool  operator==(const Record& rhs);
  };

  typedef std::list<Record> RecordList;

  // data
  RecordList  m_records;
};

//==============================================================================
// implementation
//==============================================================================
template  <typename T>
inline
bool Observer<T>::Record::IsChanged() const
{
  XR_ASSERT(Observer, value != nullptr);
  return *value != lastValue;
}

template  <typename T>
void Observer<T>::Record::Process()
{
  if (IsChanged())
  {
    (callback)(*value, data);
    lastValue = *value;
  }
}

template <typename T>
inline
bool  Observer<T>::Record::operator==(const Record& rhs)
{
  return value == rhs.value && callback == rhs.callback;
}

template <typename T>
Observer<T>::Observer()
: m_records()
{}

template <typename T>
Observer<T>::~Observer()
{}

template <typename T>
void  Observer<T>::Register(const Type& value, Callback callback, void* data)
{
  Record  r = { &value, callback, data, value };
  m_records.push_back(r);
}

template <typename T>
void  Observer<T>::Unregister(const Type& value, Callback callback)
{
  Record  r = { &value, callback, nullptr, value };
  m_records.remove(r);
}

template  <typename T>
void Observer<T>::Update()
{
  for (typename RecordList::iterator i0(m_records.begin()), i1(m_records.end());
    i0 != i1; ++i0)
  {
    i0->Process();
  }
}

} // xr

#endif // XR_OBSERVER_HPP
