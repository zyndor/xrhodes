//
// Nuclear Heart Games
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
    void* pObject;

    // structors
    ListenerBaseBase(void* p)
    : pObject(p)
    {}
    
    virtual ~ListenerBaseBase()
    {}
  };

  typedef std::list<ListenerBaseBase*>  ListenerList;
  typedef std::list<void*>              PtrList;
  
  // data
  ListenerList  m_listeners;  // yes ownership
  bool          m_isTraversing;
  ListenerList  m_postponedAdd; // yes ownership
  PtrList       m_postponedRemove;
  bool          m_postponedClear;
  
  // structors
  EventDispatcherCore();
  ~EventDispatcherCore();
  
  // internal
  ListenerList::iterator  GetAdded(ListenerBaseBase* p)
  {
    return std::find_if(m_listeners.begin(), m_listeners.end(),
      ListenerBaseBase::Compare(p));
  }
  
  ListenerList::iterator  GetPostponedAdded(ListenerBaseBase* p)
  {
    return std::find_if(m_postponedAdd.begin(), m_postponedAdd.end(),
      ListenerBaseBase::Compare(p));
  }

  bool  IsAdded(ListenerBaseBase* p) const
  {
    ListenerList::const_reverse_iterator  iEnd(m_listeners.rend());
    return std::find_if(m_listeners.rbegin(), m_listeners.rend(),
      ListenerBaseBase::Compare(p)) != iEnd;
  }
  
  bool  IsPostPonedAdded(ListenerBaseBase* p) const
  {
    ListenerList::const_reverse_iterator  iEnd(m_postponedAdd.rend());
    return std::find_if(m_postponedAdd.rbegin(), iEnd,
      ListenerBaseBase::Compare(p)) != iEnd;
  }
  
  bool  RemoveListener(void* pListener); // no ownership transfer
  
  void  Clear();
};

//==============================================================================
template  <typename E = void>
class EventDispatcher:  protected EventDispatcherCore
{
public:
  // types
  typedef E Event;
  
  class ListenerBase: public ListenerBaseBase
  {
  public:
    // structors
    ListenerBase(void* p): ListenerBaseBase(p)  {}
    virtual ~ListenerBase() {}
    
    // virtual
    virtual bool  Handle(E ev) =0;
  };
  
  template  <class T>
  class Listener: public ListenerBase
  {
  public:
    // types
    typedef T Type;
    typedef bool(Type::*Callback)(Event);
    
    // data
    Callback  pCallback;
    
    // structors
    Listener(T* pListener, Callback pCb)
    : ListenerBase(pListener),
      pCallback(pCb)
    {}
    
    // general
    virtual bool  Handle(E ev)
    {
      return (static_cast<T*>(ListenerBase::pObject)->*pCallback)(ev);
    }
    
    Listener* Clone() const
    {
      return new Listener(static_cast<T*>(ListenerBase::pObject), pCallback);
    }
  };
  
  // using
  using EventDispatcherCore::RemoveListener;
  using EventDispatcherCore::Clear;
  
  // structors
  EventDispatcher()
  : EventDispatcherCore()
  {}
  
  ~EventDispatcher()
  {}
  
  // general
  template  <class T>
  bool  AddListener(T* pListener, typename Listener<T>::Callback pCallback)  // no ownership transfer
  {
    Listener<T>  l(pListener, pCallback);
    
    bool  result(!IsAdded(&l)); // || *iInsert != pListener;
    if(result)  // if not added
    {
      if(m_isTraversing)  // traversing
      {
        result = !IsPostPonedAdded(&l);
        if(result)  // if unique - postpone add
        {
          m_postponedAdd.push_back(l.Clone());
        }
        // else ignore
      }
      else  // not traversing - add instantly
      {
        m_listeners.push_back(l.Clone());
      }
    }
    return result;
  }
  
  bool  Notify(E ev)
  {
    // start traversal
    m_isTraversing = true;
    
    bool  handled(false);
    for(ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      if(static_cast<ListenerBase*>(*i0)->Handle(ev))
      {
        handled = true;
        break;
      }
    }

    // finish traversal
    m_isTraversing = false;
    if(m_postponedClear)
    {
      Clear();
    }
    
    // do postponed add
    m_listeners.splice(m_listeners.end(), m_postponedAdd);

    // do postponed removal
    while(!m_postponedRemove.empty())
    {
      RemoveListener(m_postponedRemove.back());
      m_postponedRemove.pop_back();
    }
    
    return handled;
  }

  void  Broadcast(E ev)
  {
    // start traversal
    m_isTraversing = true;
    
    for(ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      static_cast<ListenerBase*>(*i0)->Handle(ev);
    }

    // finish traversal
    m_isTraversing = false;
    
    // do postponed add
    m_listeners.splice(m_listeners.end(), m_postponedAdd);

    // do postponed removal
    while(!m_postponedRemove.empty())
    {
      RemoveListener(m_postponedRemove.back());
      m_postponedRemove.pop_back();
    }
  }
};

template  <>
class EventDispatcher<void>:  protected EventDispatcherCore
{
public:
  // types
  class ListenerBase: public ListenerBaseBase
  {
  public:
    // structors
    ListenerBase(void* p): ListenerBaseBase(p)  {}
    virtual ~ListenerBase() {}
    
    // virtual
    virtual bool  Handle() =0;
  };
  
  template  <class T>
  class Listener: public ListenerBase
  {
  public:
    // types
    typedef T Type;
    typedef bool(Type::*Callback)();
    
    // data
    Callback  pCallback;
    
    // structors
    Listener(T* pListener, Callback pCb)
    : ListenerBase(pListener),
      pCallback(pCb)
    {}
    
    // general
    virtual bool  Handle()
    {
      return (static_cast<T*>(pObject)->*pCallback)();
    }
    
    Listener* Clone() const
    {
      return new Listener(static_cast<T*>(pObject), pCallback);
    }
  };
  
  // using
  using EventDispatcherCore::RemoveListener;
  using EventDispatcherCore::Clear;
  
  // structors
  EventDispatcher()
  : EventDispatcherCore()
  {}
  
  ~EventDispatcher()
  {}
  
  // general
  template  <class T>
  bool  AddListener(T* pListener, typename Listener<T>::Callback pCallback)  // no ownership transfer
  {
    Listener<T>  l(pListener, pCallback);
    
    bool  result(!IsAdded(&l)); // || *iInsert != pListener;
    if(result)  // if not added
    {
      if(m_isTraversing)  // traversing
      {
        result = !IsPostPonedAdded(&l);
        if(result)  // if unique - postpone add
        {
          m_postponedAdd.push_back(l.Clone());
        }
        // else ignore
      }
      else  // not traversing - add instantly
      {
        m_listeners.push_back(l.Clone());
      }
    }
    return result;
  }
  
  bool  Notify()
  {
    // start traversal
    m_isTraversing = true;
    
    bool  handled(false);
    for(ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      if(static_cast<ListenerBase*>(*i0)->Handle())
      {
        handled = true;
        break;
      }
    }

    // finish traversal
    m_isTraversing = false;
    
    // do postponed add
    m_listeners.splice(m_listeners.end(), m_postponedAdd);

    // do postponed removal
    while(!m_postponedRemove.empty())
    {
      RemoveListener(m_postponedRemove.back());
      m_postponedRemove.pop_back();
    }
    
    return handled;
  }

  void  Broadcast()
  {
    // start traversal
    m_isTraversing = true;
    
    for(ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      static_cast<ListenerBase*>(*i0)->Handle();
    }

    // finish traversal
    m_isTraversing = false;
    if(m_postponedClear)
    {
      Clear();
    }
    
    // do postponed add
    m_listeners.splice(m_listeners.end(), m_postponedAdd);

    // do postponed removal
    while(!m_postponedRemove.empty())
    {
      RemoveListener(m_postponedRemove.back());
      m_postponedRemove.pop_back();
    }
  }
};

//==============================================================================
// implementation
//==============================================================================

} // XR

#endif // XR_EVENTDISPATCHER_HPP
