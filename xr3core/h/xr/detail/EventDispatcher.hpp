#ifndef XR_EVENTDISPATCHER_HPP
#define XR_EVENTDISPATCHER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/debug.hpp"
#include "xr/fundamentals.hpp"
#include <list>
#include <map>
#include <algorithm>
#include <functional>

namespace xr
{

//==============================================================================
class EventDispatcherCore
{
  XR_NONCOPY_DECL(EventDispatcherCore)

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
        return p1->object == p2->object;
      }

    protected:
      // data
      const ListenerBaseBase* const p1;
    };

    // data
    void* const object;

    // structors
    ListenerBaseBase(void* o)
    : object(o)
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
    void* const data;  // this is a ListenerBaseBase* for ADD types and the owner of the Postponed object has ownership of it.

    // structors
    explicit Postponed(Type t, void* p = 0)
    : type(t),
      data(p)
    {}

    // operators
    bool  operator==(const Postponed& rhs) const
    {
      return type == rhs.type && data == rhs.data;
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
  void  AddListenerImpl(ListenerBaseBase* p);

  ListenerList::iterator  GetAdded(ListenerBaseBase* p);

  PostponedList::iterator  GetPostponedAdded(ListenerBaseBase* p);

  bool  IsAdded(ListenerBaseBase* p) const;

  bool  IsPostponedAdded(ListenerBaseBase* p) const;

  bool  RemoveListener(void* object); // no ownership transfer

  void  ProcessPostponed();

  void  FinishTraversal();

  void  Clear();

  template <class Listener>
  bool  AddListenerInternal(Listener& l);
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
    ListenerBase(void* object): ListenerBaseBase(object)  {}
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
    Callback  callback;

    // structors
    Listener(T* object, Callback cb)
    : ListenerBase(object),
      callback(cb)
    {}

    // general
    virtual Return Handle(E eventData)
    {
      return (static_cast<T*>(ListenerBaseBase::object)->*callback)(eventData);
    }

    Listener* Clone() const
    {
      return new Listener(static_cast<T*>(ListenerBaseBase::object), callback);
    }
  };

  // using
  using EventDispatcherCore::RemoveListener;
  using EventDispatcherCore::Clear;

  // general
  template  <class T>
  bool  AddListener(T& object, typename Listener<T>::Callback callback)  // no ownership transfer
  {
    Listener<T>  l(&object, callback);
    return AddListenerInternal(l);
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
    Callback  callback;

    // structors
    Listener(T* object, Callback cb)
    : ListenerBase(object),
      callback(cb)
    {}

    // general
    virtual Return Handle()
    {
      return (static_cast<T*>(ListenerBaseBase::object)->*callback)();
    }

    Listener* Clone() const
    {
      return new Listener(static_cast<T*>(ListenerBaseBase::object), callback);
    }
  };

  // general
  template  <class T>
  bool  AddListener(T& object, typename Listener<T>::Callback callback)  // no ownership transfer
  {
    Listener<T>  l(&object, callback);
    return AddListenerInternal(l);
  }
};

//==============================================================================
inline
void  EventDispatcherCore::AddListenerImpl(ListenerBaseBase* p)
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

//==============================================================================
template <class Listener>
bool  EventDispatcherCore::AddListenerInternal(Listener& l)
{
  bool  result(!IsAdded(&l)); // || *iInsert != %l;
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
      AddListenerImpl(l.Clone());
    }
  }
  return result;
}

} // xr

#endif // XR_EVENTDISPATCHER_HPP