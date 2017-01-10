//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_EVENTDISPATCHER_HPP
#define XR_EVENTDISPATCHER_HPP

#include <XR/debug.hpp>
#include <list>
#include <map>
#include <algorithm>
#include <functional>

namespace XR
{

//==============================================================================
class EventDispatcherCore
{
protected:
  // types
  class ListenerBaseBase
  {
  public:
    // types
    class Compare
    {
    public:
      // structors
      explicit Compare(const ListenerBaseBase* p)
      : p1(p)
      {}

      // operators
      bool  operator()(const ListenerBaseBase* p2) const
      {
        return p1->pObject == p2->pObject;
      }

    protected:
      // data
      const ListenerBaseBase* const p1;
    };

    // data
    void* const pObject;

    // structors
    ListenerBaseBase(void* p)
    : pObject(p)
    {}
    
    virtual ~ListenerBaseBase()
    {}
  };

  typedef std::list<ListenerBaseBase*>  ListenerList;
  typedef std::list<void*>              PtrList;
  
  struct  Postponed
  {
    // types
    enum  Type
    {
      ADD,
      REMOVE,
      CLEAR
    };
    
    // data
    const Type  type;
    void* const pData;  // this is a ListenerBaseBase* for ADD types and comes with ownership.
    
    // structors
    explicit Postponed(Type t, void* p = 0)
    : type(t),
      pData(p)
    {}
    
    // operators
    bool  operator==(const Postponed& rhs) const
    {
      return type == rhs.type && pData == rhs.pData;
    }
  };
  
  typedef std::list<Postponed> PostponedList;
  
  // data
  ListenerList  m_listeners;  // yes ownership
  bool          m_isTraversing;
  PostponedList m_postponed;
  
  // structors
  EventDispatcherCore();
  ~EventDispatcherCore();
  
  // internal
  void  _AddListener(ListenerBaseBase* p);
  
  ListenerList::iterator  GetAdded(ListenerBaseBase* p);
  
  PostponedList::iterator  GetPostponedAdded(ListenerBaseBase* p);

  bool  IsAdded(ListenerBaseBase* p) const;
  
  bool  IsPostponedAdded(ListenerBaseBase* p) const;
  
  bool  RemoveListener(void* pListener); // no ownership transfer

  void  ProcessPostponed();
  
  void  Clear();
};

//==============================================================================
template  <typename Return, typename E = void>
class EventDispatcher:  protected EventDispatcherCore
{
protected:
  // types
  typedef E Event;
  
  class ListenerBase: public ListenerBaseBase
  {
  public:
    // structors
    ListenerBase(void* p): ListenerBaseBase(p)  {}
    virtual ~ListenerBase() {}
    
    // virtual
    virtual Return Handle(E eventData) =0;
  };
  
  template  <class T>
  class Listener: public ListenerBase
  {
  public:
    // types
    typedef T Type;
    typedef Return(Type::*Callback)(Event);
    
    // data
    Callback  pCallback;
    
    // structors
    Listener(T* pListener, Callback pCb)
    : ListenerBase(pListener),
      pCallback(pCb)
    {}
    
    // general
    virtual Return Handle(E eventData)
    {
      return (static_cast<T*>(ListenerBase::pObject)->*pCallback)(eventData);
    }
    
    Listener* Clone() const
    {
      return new Listener(static_cast<T*>(ListenerBase::pObject), pCallback);
    }
  };
  
  // using
  using EventDispatcherCore::RemoveListener;
  using EventDispatcherCore::Clear;
  
  // general
  template  <class T>
  bool  AddListener(T* pListener, typename Listener<T>::Callback pCallback)  // no ownership transfer
  {
    Listener<T>  l(pListener, pCallback);
    
    bool  result(!IsAdded(&l)); // || *iInsert != pListener;
    if (result)  // if not added
    {
      if (m_isTraversing)  // traversing
      {
        result = !IsPostponedAdded(&l);
        if (result)  // if unique - postpone add
        {
          m_postponed.push_back(Postponed(Postponed::ADD, l.Clone()));
        }
        // else ignore
      }
      else  // not traversing - add instantly
      {
        _AddListener(l.Clone());
      }
    }
    return result;
  }
};

template  <typename Return>
class EventDispatcher<Return, void>:  protected EventDispatcherCore
{
protected:
  // types
  class ListenerBase: public ListenerBaseBase
  {
  public:
    // structors
    ListenerBase(void* p): ListenerBaseBase(p)  {}
    virtual ~ListenerBase() {}
    
    // virtual
    virtual Return Handle() =0;
  };
  
  template  <class T>
  class Listener: public ListenerBase
  {
  public:
    // types
    typedef T Type;
    typedef Return(Type::*Callback)();
    
    // data
    Callback  pCallback;
    
    // structors
    Listener(T* pListener, Callback pCb)
    : ListenerBase(pListener),
      pCallback(pCb)
    {}
    
    // general
    virtual Return Handle()
    {
      return (static_cast<T*>(pObject)->*pCallback)();
    }
    
    Listener* Clone() const
    {
      return new Listener(static_cast<T*>(pObject), pCallback);
    }
  };
  
  // general
  template  <class T>
  bool  AddListener(T* pListener, typename Listener<T>::Callback pCallback)  // no ownership transfer
  {
    Listener<T>  l(pListener, pCallback);
    
    bool  result(!IsAdded(&l)); // || *iInsert != pListener;
    if (result)  // if not added
    {
      if (m_isTraversing)  // traversing
      {
        result = !IsPostponedAdded(&l);
        if (result)  // if unique - postpone add
        {
          m_postponed.push_back(Postponed(Postponed::ADD, l.Clone()));
        }
        // else ignore
      }
      else  // not traversing - add instantly
      {
        _AddListener(l.Clone());
      }
    }
    return result;
  }
};

#include "EventDispatcher.inl"

} // XR

#endif // XR_EVENTDISPATCHER_HPP
