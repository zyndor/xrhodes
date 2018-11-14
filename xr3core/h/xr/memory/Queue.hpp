#ifndef XR_QUEUE_HPP
#define XR_QUEUE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debug.hpp"
#include <algorithm>
#include <iterator>
#include <cstddef>

namespace xr
{
namespace QueueImpl
{

//==============================================================================
struct  NodeCore
{
  // structors
  explicit NodeCore(NodeCore *p);

  // general use
  void  Hook(NodeCore *p);

  // data
  NodeCore  *next;
};

//==============================================================================
template  <typename Type>
struct  Node: public NodeCore
{
  // structors
  explicit Node(Type d);

  // data
  Type  data;
};

//==============================================================================
template <typename Type>
struct  Iterator: public std::forward_iterator_tag
{
  // types
  typedef Node<Type>                NodeType;
  typedef Iterator<Type>            SelfType;

  typedef Type                      value_type;
  typedef Type*                     pointer;
  typedef Type&                     reference;

  typedef ptrdiff_t                 difference_type;
  typedef std::forward_iterator_tag iterator_category;

  // structors
  Iterator();
  explicit Iterator(NodeType* p);
  ~Iterator();

  // operators
  reference  operator *() const;
  pointer    operator ->() const;

  SelfType&  operator ++();
  SelfType&  operator ++(int);

  bool  operator ==(const SelfType& rhs) const;
  bool  operator !=(const SelfType& rhs) const;

  // data
  NodeType  *node;
};

//==============================================================================
template <typename Type>
struct  ConstIterator: public std::forward_iterator_tag
{
  // types
  typedef const Node<Type>          NodeType;
  typedef Iterator<Type>            IteratorType;
  typedef ConstIterator<Type>       SelfType;

  typedef Type                      value_type;
  typedef const Type*               pointer;
  typedef const Type&               reference;

  typedef ptrdiff_t                 difference_type;
  typedef std::forward_iterator_tag iterator_category;

  // structors
  ConstIterator();
  explicit ConstIterator(NodeType* p);
  ConstIterator(const IteratorType& rhs);
  ~ConstIterator();

  // operators
  reference  operator *() const;
  pointer    operator ->() const;

  SelfType&  operator ++();
  SelfType&  operator ++(int);

  bool  operator ==(const SelfType& rhs) const;
  bool  operator !=(const SelfType& rhs) const;

  // data
  NodeType  *node;
};

} // QueueImpl

//==============================================================================
///@brief Singly linked list based queue which allows traversal, however only
/// allows insertion at the end and removal from the front. Also allows optimal
/// adopt()ion of elements from the front of another Queue.
///@note  If you store pointers to the heap, you'll have to make sure
/// that they're properly copied and deallocated when your Queue<> is
/// copied / cleared (deconstructed).
template  <typename Type, class AllocType = std::allocator<Type> >
class  Queue
{
public:
  // types
  typedef Type  value_type;

  typedef QueueImpl::Iterator<value_type>       iterator;
  typedef QueueImpl::ConstIterator<value_type>  const_iterator;

  typedef AllocType                             allocator;

  typedef size_t                                size_type;
  typedef ptrdiff_t                             difference_type;

  typedef typename allocator::reference         reference;
  typedef typename allocator::const_reference   const_reference;
  typedef typename allocator::pointer           pointer;
  typedef typename allocator::const_pointer     const_pointer;

  typedef Queue<value_type, AllocType>          SelfType;
  typedef QueueImpl::Node<value_type>           NodeType;

  typedef typename AllocType::template rebind<NodeType>::other NodeAllocType;

  // structors
  Queue(allocator a = allocator());
  Queue(const SelfType& rhs);
  ~Queue();

  // general use
  ///@return  Whether the Queue is empty.
  bool    empty() const;

  ///@return  The number of elements in the Queue.
  ///@note O(n).
  size_t  size() const;

  ///@return  A copy of the allocator object of the Queue.
  allocator get_allocator() const;

  ///@return  An iterator to the first element of the Queue.
  iterator        begin();

  ///@return  A const_iterator to the first element of the Queue.
  const_iterator  begin() const;

  ///@return  An iterator past the last element of the Queue.
  iterator        end();

  ///@return  A const_iterator past the last element of the Queue.
  const_iterator  end() const;

  ///@return  A reference to the first element in the Queue.
  ///@note  You will have to make sure that the Queue is !empty().
  value_type&       front();

  ///@return  A const reference to the first element in the Queue.
  ///@note  You will have to make sure that the Queue is !empty().
  const value_type& front() const;

  ///@brief Adds an element to the end of the Queue.
  void  push_back(value_type d);

  ///@brief Removes an element from the front of the Queue.
  ///@note  You will have to make sure that there are elements in the Queue
  /// before removing.
  void  pop_front();

  ///@brief Removes all elements from the Queue.
  ///@note  If you store pointers to objects on the heap, you will have to
  /// make sure that these are deallocated.
  void  clear();

  ///@brief Moves all the elements from @a rhs to the end of the Queue.
  void  adopt(SelfType& rhs);

  ///@brief Moves all the elements from @a rhs to the end of the Queue, up to,
  /// not including @a end.
  ///@note  @a end has to be a part of @a rhs.
  void  adopt(SelfType& rhs, iterator end);

  ///@brief Attempts to find and remove @a d from this Queue.
  void  remove(value_type d);

  // operator overloads
  SelfType&  operator=(const SelfType& rhs);

protected:
  // internal use
  void  _adopt(SelfType& rhs);

  // data
  NodeAllocType  m_allocator;

  NodeType  *m_head;
  NodeType  *m_tail;
};

//==============================================================================
// inline
//==============================================================================
namespace QueueImpl
{
//==============================================================================
inline
NodeCore::NodeCore(NodeCore *p)
: next(p)
{}

//==============================================================================
inline
void  NodeCore::Hook(NodeCore *p)
{
  next = p;
}

//==============================================================================
template<typename T>
inline
Node<T>::Node(T d)
: NodeCore(nullptr),
  data(d)
{}

//==============================================================================
template <typename T>
Iterator<T>::Iterator()
: node(nullptr)
{}

//==============================================================================
template <typename T>
Iterator<T>::Iterator(NodeType* p)
: node(p)
{}

//==============================================================================
template <typename Type>
Iterator<Type>::~Iterator()
{}

//==============================================================================
template <typename T>
inline
typename Iterator<T>::reference
  Iterator<T>::operator *() const
{
  return node->data;
}

//==============================================================================
template <typename T>
inline
typename Iterator<T>::pointer
  Iterator<T>::operator ->() const
{
  return &node->data;
}

//==============================================================================
template <typename T>
inline
Iterator<T>&  Iterator<T>::operator ++()
{
  node = static_cast<NodeType*> (node->next);
  return *this;
}

//==============================================================================
template <typename T>
inline
Iterator<T>& Iterator<T>::operator ++(int)
{
  SelfType  tmp(*this);
  ++(*this);
  return tmp;
}

//==============================================================================
template <typename T>
inline
bool  Iterator<T>::operator ==(const SelfType& rhs) const
{
  return node == rhs.node;
}

//==============================================================================
template <typename T>
inline
bool  Iterator<T>::operator !=(const SelfType& rhs) const
{
  return !(*this == rhs);
}

//==============================================================================
template <typename T>
ConstIterator<T>::ConstIterator()
: node(nullptr)
{}

//==============================================================================
template <typename T>
ConstIterator<T>::ConstIterator(NodeType* p)
: node(p)
{}

//==============================================================================
template <typename T>
ConstIterator<T>::ConstIterator(const IteratorType& rhs)
: node(rhs.node)
{}

//==============================================================================
template <typename Type>
ConstIterator<Type>::~ConstIterator()
{}

//==============================================================================
template <typename T>
inline
typename ConstIterator<T>::reference
  ConstIterator<T>::operator *() const
{
  return node->data;
}

//==============================================================================
template <typename T>
inline
typename ConstIterator<T>::pointer
  ConstIterator<T>::operator ->() const
{
  return &node->data;
}

//==============================================================================
template <typename T>
inline
ConstIterator<T>&  ConstIterator<T>::operator ++()
{
  node = static_cast<NodeType*> (node->next);
  return *this;
}

//==============================================================================
template <typename T>
inline
ConstIterator<T>&  ConstIterator<T>::operator ++(int)
{
  SelfType  tmp(*this);
  ++(*this);
  return tmp;
}

//==============================================================================
template <typename T>
inline
bool  ConstIterator<T>::operator ==(const SelfType& rhs) const
{
  return node == rhs.node;
}

//==============================================================================
template <typename T>
inline
bool  ConstIterator<T>::operator !=(const SelfType& rhs) const
{
  return !(*this == rhs);
}

} // QueueImpl

//==============================================================================
template  <typename Type, class AllocType>
Queue<Type, AllocType>::Queue(allocator a)
: m_allocator(a),
  m_head(nullptr),
  m_tail(nullptr)
{}

//==============================================================================
template  <typename Type, class AllocType>
Queue<Type, AllocType>::Queue(const SelfType& rhs)
: m_allocator(rhs.m_allocator),
  m_head(nullptr),
  m_tail(nullptr)
{
  const_iterator  i0(rhs.begin()), i1(rhs.end());
  while (i0 != i1)
  {
    push_back(*i0);
    ++i0;
  }
}

//==============================================================================
template  <typename Type, class AllocType>
inline
Queue<Type, AllocType>::~Queue()
{
  clear();
}

//==============================================================================
template  <typename Type, class AllocType>
inline
bool  Queue<Type, AllocType>::empty() const
{
  return m_head == nullptr;
}

//==============================================================================
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::allocator
  Queue<Type, AllocType>::get_allocator() const
{
  return m_allocator;
}

//==============================================================================
template  <typename Type, class AllocType>
inline
size_t  Queue<Type, AllocType>::size() const
{
  size_t  count = 0;
  const NodeType  *p = m_head;
  while (p != nullptr)
  {
    p = static_cast<const NodeType*> (p->next);
    ++count;
  }
  return count;
}

//==============================================================================
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::iterator  Queue<Type, AllocType>::begin()
{
  return iterator(m_head);
}

//==============================================================================
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::const_iterator  Queue<Type, AllocType>::begin() const
{
  return const_iterator(m_head);
}

//==============================================================================
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::iterator  Queue<Type, AllocType>::end()
{
  return iterator(nullptr);
}

//==============================================================================
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::const_iterator  Queue<Type, AllocType>::end() const
{
  return const_iterator(nullptr);
}

//==============================================================================
template  <typename Type, class AllocType>
inline
Type&  Queue<Type, AllocType>::front()
{
  XR_ASSERT(Queue, !empty());
  return m_head->data;
}

//==============================================================================
template  <typename Type, class AllocType>
inline
const Type&  Queue<Type, AllocType>::front() const
{
  XR_ASSERT(Queue, !empty());
  return m_head->data;
}

//==============================================================================
template  <typename Type, class AllocType>
inline
void  Queue<Type, AllocType>::push_back(value_type d)
{
  NodeType  *node = m_allocator.allocate(1);
  m_allocator.construct(node, NodeType(d));

  if (empty())
  {
    m_head = node;
    m_tail = node;
  }
  else
  {
    m_tail->Hook(node);
    m_tail = static_cast<NodeType*>(m_tail->next);
  }
}

//==============================================================================
template  <typename Type, class AllocType>
inline
void  Queue<Type, AllocType>::pop_front()
{
  XR_ASSERT(Queue, !empty());
  NodeType  *deletee = m_head;
  m_head = static_cast<NodeType*>(m_head->next);
  m_allocator.destroy(deletee);
  m_allocator.deallocate(deletee, 1);
}

//==============================================================================
template  <typename Type, class AllocType>
void  Queue<Type, AllocType>::clear()
{
  NodeType  *head = m_head;
  while (head != nullptr)
  {
    NodeType  *temp(static_cast<NodeType*>(head->next));
    m_allocator.destroy(head);
    m_allocator.deallocate(head, 1);
    head = temp;
  }
  m_head = head;
  //m_tail = nullptr;	// not necessary; we're only accessing m_tail when m_head != nullptr
}

//==============================================================================
template  <typename Type, class AllocType>
void  Queue<Type, AllocType>::adopt(SelfType& rhs)
{
  SelfType  temp(m_allocator);
  std::swap(rhs, temp);

  _adopt(temp);
}

//==============================================================================
template  <typename Type, class AllocType>
void  Queue<Type, AllocType>::adopt(SelfType& rhs, iterator end)
{
  const NodeType * const   last = end.node;

  if (!(rhs.empty() || rhs.m_head == end.node))
  {
    SelfType  temp(m_allocator);
    NodeType  *node = rhs.m_head;
    temp.m_head = node;

    do
    {
      temp.m_tail = node;
      node = static_cast<NodeType*>(node->next);
    }
    while (node != last);

    rhs.m_head = node;
    _adopt(temp);

    m_tail->next = nullptr;
  }
}

//==============================================================================
template  <typename Type, class AllocType>
inline
void  Queue<Type, AllocType>::remove(value_type d)
{
  auto iEnd = end();
  auto iFind = std::find(begin(), iEnd, d);
  if (iFind != iEnd)
  {
    auto iBegin = begin();
    adopt(*this, iFind);
    pop_front();
    adopt(*this, iBegin);
  }
}

//==============================================================================
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::SelfType&  Queue<Type, AllocType>::operator=(const Queue& rhs)
{
  SelfType  temp(rhs);

  std::swap(m_head, temp.m_head); // faster than swap(temp)
  m_tail = temp.m_tail;

  return *this;
}

//==============================================================================
template  <typename Type, class AllocType>
void  Queue<Type, AllocType>::_adopt(SelfType& rhs)
{
  XR_ASSERT(Queue, &rhs != this);

  if (empty())
  {
    m_head = rhs.m_head;
  }
  else
  {
    m_tail->next = rhs.m_head;
  }

  if (!rhs.empty())
  {
    rhs.m_head = nullptr;

    m_tail = rhs.m_tail;
    //rhs.m_tail = nullptr;	// not necessary; we're only accessing m_tail when m_head != nullptr
  }
}

} // xr

#endif  //XR_QUEUE_HPP
