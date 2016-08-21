//
// Nuclear Heart Interactive
// XRhodes
//
// EventDispatcher.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    18/03/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_EVENTDISPATCHER_HPP
#define XR_EVENTDISPATCHER_HPP

#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include "types.hpp"

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

//==============================================================================
template <typename E = void>
class EventNotifier : protected EventDispatcher<bool, E>
{
public:
  // types
  typedef EventDispatcher<bool, E>  BaseType;

  // using
  using BaseType::AddListener;
  using BaseType::RemoveListener;
  using BaseType::Clear;

  // general
  bool  Notify(E eventData)
  {
    // start traversal
    m_isTraversing = true;

    bool  handled(false);
    for (ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      if (static_cast<ListenerBase*>(*i0)->Handle(eventData))
      {
        handled = true;
        break;
      }
    }

    // finish traversal
    m_isTraversing = false;

    ProcessPostponed();

    return handled;
  }
};

template <>
class EventNotifier<void> : protected EventDispatcher<bool>
{
public:
  // types
  typedef EventDispatcher<bool>  BaseType;

  // using
  using BaseType::AddListener;
  using BaseType::RemoveListener;
  using BaseType::Clear;

  // general
  bool  Notify()
  {
    // start traversal
    m_isTraversing = true;

    bool  handled(false);
    for (ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      if (static_cast<ListenerBase*>(*i0)->Handle())
      {
        handled = true;
        break;
      }
    }

    // finish traversal
    m_isTraversing = false;

    ProcessPostponed();

    return handled;
  }
};

//==============================================================================
template <typename E = void>
class EventBroadcaster : protected EventDispatcher<void, E>
{
public:
  // types
  typedef EventDispatcher<void, E>  BaseType;

  // using
  using BaseType::AddListener;
  using BaseType::RemoveListener;
  using BaseType::Clear;

  // general
  void  Broadcast(E eventData)
  {
    // start traversal
    m_isTraversing = true;

    for (ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      static_cast<ListenerBase*>(*i0)->Handle(eventData);
    }

    // finish traversal
    m_isTraversing = false;

    ProcessPostponed();
  }
};

template <>
class EventBroadcaster<void>: protected EventDispatcher<void>
{
public:
  // types
  typedef EventDispatcher<void>  BaseType;

  // using
  using BaseType::AddListener;
  using BaseType::RemoveListener;
  using BaseType::Clear;

  // general
  void  Broadcast()
  {
    // start traversal
    m_isTraversing = true;

    for (ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      static_cast<ListenerBase*>(*i0)->Handle();
    }

    // finish traversal
    m_isTraversing = false;

    ProcessPostponed();
  }
};

//==============================================================================
// implementation
//==============================================================================
inline
void  EventDispatcherCore::_AddListener(ListenerBaseBase* p)
{
  XR_ASSERT(EventDispatcherCore, !IsAdded(p));
  m_listeners.push_back(p);
}

//==============================================================================
inline
EventDispatcherCore::ListenerList::iterator
  EventDispatcherCore::GetAdded(ListenerBaseBase* p)
{
  return std::find_if(m_listeners.begin(), m_listeners.end(),
                      ListenerBaseBase::Compare(p));
}

//==============================================================================
inline
EventDispatcherCore::PostponedList::iterator
  EventDispatcherCore::GetPostponedAdded(ListenerBaseBase* p)
{
  Postponed  op(Postponed::ADD, p);
  return std::find(m_postponed.begin(), m_postponed.end(), op);
}

//==============================================================================
inline
bool  EventDispatcherCore::IsAdded(ListenerBaseBase* p) const
{
  ListenerList::const_reverse_iterator  iEnd(m_listeners.rend());
  return std::find_if(m_listeners.rbegin(), iEnd,
                      ListenerBaseBase::Compare(p)) != iEnd;
}

//==============================================================================
inline
bool  EventDispatcherCore::IsPostponedAdded(ListenerBaseBase* p) const
{
  Postponed  op(Postponed::ADD, p);
  PostponedList::const_reverse_iterator  iEnd(m_postponed.rend());
  return std::find(m_postponed.rbegin(), iEnd, op) != iEnd;
}

} // XR

#endif // XR_EVENTDISPATCHER_HPP
