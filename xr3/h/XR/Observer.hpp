//
// Nuclear Heart Games
// XRhodes
//
// Observer.hpp
// @author  Gyorgy Straub
// @date    16/07/2011
//
// copyright (c) 2011 - 2013. All rights reserved.
//
#if !defined XR_OBSERVER_HPP
#define XR_OBSERVER_HPP

#include <list>

namespace XR
{

template  <typename T>
class Observer
{
public:
  typedef T Type;
  typedef void(*Callback)(const Type& value, void* pData);

  Observer();
  ~Observer();

  void  Register(const Type& value, Callback pCallback, void* pData);
  void  Unregister(const Type& value, Callback pCallback);

  void  Update();

protected:
  // types
  struct  Record
  {
    const Type* pValue;
    Callback    pCallback;
    void*       pData;
    Type        lastValue;

    bool  IsChanged() const;
    void  Process();

    bool  operator==(const Record& rhs);
  };

  typedef std::list<Record> RecordList;

  // data
  RecordList  m_records;
};


/*
  implementation
  */
template  <typename T>
inline
bool Observer<T>::Record::IsChanged() const
{
  XR_ASSERT(Observer, pValue != 0);
  return *pValue != lastValue;
}

template  <typename T>
void Observer<T>::Record::Process()
{
  if(IsChanged())
  {
    (pCallback)(*pValue, pData);
    lastValue = *pValue;
  }
}

template <typename T>
inline
bool  Observer<T>::Record::operator==(const Record& rhs)
{
  return pValue == rhs.pValue && pCallback == rhs.pCallback;
}

template <typename T>
Observer<T>::Observer()
: m_records()
{}

template <typename T>
Observer<T>::~Observer()
{}

template <typename T>
void  Observer<T>::Register(const Type& value, Callback pCallback, void* pData)
{
  Record  r = { &value, pCallback, pData, value };
  m_records.push_back(r);
}

template <typename T>
void  Observer<T>::Unregister(const Type& value, Callback pCallback)
{
  Record  r = { &value, pCallback, 0, value };
  m_records.remove(r);
}

template  <typename T>
void Observer<T>::Update()
{
  for(typename RecordList::iterator i0(m_records.begin()), i1(m_records.end());
    i0 != i1; ++i0)
  {
    i0->Process();
  }
}

} // XR

#endif // XR_OBSERVER_HPP