//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_QUEUE_HPP
#define XR_QUEUE_HPP

#include <algorithm>
#include <iterator>
#include "debug.hpp"

namespace XR
{
namespace  QueueImpl
{

//==============================================================================
struct  NodeCore 
{
  // structors
  explicit NodeCore(NodeCore *p);
  
  // general use
  void  Hook(NodeCore *p);

  // data
  NodeCore  *pNext;
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
  NodeType  *pNode;
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
  NodeType  *pNode;
};

//==============================================================================
// implementation
//==============================================================================
inline
NodeCore::NodeCore(NodeCore *p)
: pNext(p)
{}

//==============================================================================
inline
void  NodeCore::Hook(NodeCore *p)
{
  pNext = p;
}

//==============================================================================
template<typename T>
inline
Node<T>::Node(T d)
: NodeCore(0),
  data(d)
{}

//==============================================================================
template <typename T>
Iterator<T>::Iterator()
: pNode(0)
{}

//==============================================================================
template <typename T>
Iterator<T>::Iterator(NodeType* p)
: pNode(p)
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
  return pNode->data;
}

//==============================================================================
template <typename T>
inline
typename Iterator<T>::pointer
  Iterator<T>::operator ->() const
{
  return &pNode->data;
}

//==============================================================================
template <typename T>
inline
Iterator<T>&  Iterator<T>::operator ++()
{
  pNode = static_cast<NodeType*> (pNode->pNext);
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
  return pNode == rhs.pNode;
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
: pNode(0)
{}

//==============================================================================
template <typename T>
ConstIterator<T>::ConstIterator(NodeType* p)
: pNode(p)
{}

//==============================================================================
template <typename T>
ConstIterator<T>::ConstIterator(const IteratorType& rhs)
: pNode(rhs.pNode)
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
  return pNode->data;
}

//==============================================================================
template <typename T>
inline
typename ConstIterator<T>::pointer
  ConstIterator<T>::operator ->() const
{
  return &pNode->data;
}

//==============================================================================
template <typename T>
inline
ConstIterator<T>&  ConstIterator<T>::operator ++()
{
  pNode = static_cast<NodeType*> (pNode->pNext);
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
  return pNode == rhs.pNode;
}

//==============================================================================
template <typename T>
inline
bool  ConstIterator<T>::operator !=(const SelfType& rhs) const
{
  return !(*this == rhs);
}

}  // end namespace QueueImpl

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
  bool    empty() const;
  size_t  size() const;
  
  allocator get_allocator() const;

  iterator        begin();
  const_iterator  begin() const;
  iterator        end();
  const_iterator  end() const;

  value_type&       front();
  const value_type& front() const;

  void  push_back(value_type d);
  void  pop_front();
  void  clear();

  void  adopt(SelfType& rhs);
  void  adopt(SelfType& rhs, iterator end);


  // operator overloads
  SelfType&  operator=(const SelfType& rhs);

protected:
  // internal use
  void  _adopt(SelfType& rhs);

  // data
  NodeAllocType  m_allocator;
  
  NodeType  *m_pHead;
  NodeType  *m_pTail;
};

//==============================================================================
// implementation
//==============================================================================
template  <typename Type, class AllocType>
Queue<Type, AllocType>::Queue(allocator a)
: m_allocator(a),
  m_pHead(0),
  m_pTail(0)
{}

//==============================================================================
template  <typename Type, class AllocType>
Queue<Type, AllocType>::Queue(const SelfType& rhs)
: m_allocator(rhs.m_allocator),
  m_pHead(0),
  m_pTail(0)
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
Queue<Type, AllocType>::~Queue()
{
  clear();
}

//==============================================================================
///@return  Whether the Queue is empty.
template  <typename Type, class AllocType>
inline
bool  Queue<Type, AllocType>::empty() const
{
  return m_pHead == 0;
}

//==============================================================================
///@return  A copy of the allocator object of the Queue.
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::allocator
  Queue<Type, AllocType>::get_allocator() const
{
  return m_allocator;
}

//==============================================================================
///@return  The number of elements in the Queue.
template  <typename Type, class AllocType>
size_t  Queue<Type, AllocType>::size() const
{
  size_t  count(0);
  const NodeType  *p(m_pHead);
  while (p != 0)
  {
    p = static_cast<const NodeType*> (p->pNext);
    ++count;
  }
  return count;
}

//==============================================================================
///@return  An iterator to the first element of the Queue.
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::iterator  Queue<Type, AllocType>::begin()
{
  return iterator(m_pHead);
}

//==============================================================================
///@return  A const_iterator to the first element of the Queue.
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::const_iterator  Queue<Type, AllocType>::begin() const
{
  return const_iterator(m_pHead);
}

//==============================================================================
///@return  An iterator past the last element of the Queue.
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::iterator  Queue<Type, AllocType>::end()
{
  return iterator(0);
}

//==============================================================================
///@return  A const_iterator past the last element of the Queue.
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::const_iterator  Queue<Type, AllocType>::end() const
{
  return const_iterator(0);
}

//==============================================================================
///@return  A reference to the first element in the Queue.
///@note  You will have to make sure that the Queue is !empty().
template  <typename Type, class AllocType>
Type&  Queue<Type, AllocType>::front()
{
  XR_ASSERT(Queue, !empty());
  return m_pHead->data;
}

//==============================================================================
///@return  A const reference to the first element in the Queue.
///@note  You will have to make sure that the Queue is !empty().
template  <typename Type, class AllocType>
const Type&  Queue<Type, AllocType>::front() const
{
  XR_ASSERT(Queue, !empty());
  return m_pHead->data;
}

//==============================================================================
///@brief Adds an element to the end of the Queue.
template  <typename Type, class AllocType>
void  Queue<Type, AllocType>::push_back(value_type d)
{
  NodeType  *pNode(m_allocator.allocate(1));
  m_allocator.construct(pNode, NodeType(d));
  
  if (empty())
  {
    m_pHead = pNode;
    m_pTail = pNode;	// = m_pHead, but Load-Hit-Store
  }
  else
  {
    m_pTail->Hook(pNode);
    m_pTail = static_cast<NodeType*>(m_pTail->pNext);
  }
}

//==============================================================================
///@brief Removes an element from the front of the Queue.
///@note  You will have to make sure that there are elements in the Queue
/// before removing.
template  <typename Type, class AllocType>
void  Queue<Type, AllocType>::pop_front()
{
  XR_ASSERT(Queue, !empty());
  NodeType  *pDelete(m_pHead);
  m_pHead = static_cast<NodeType*>(m_pHead->pNext);
  m_allocator.destroy(pDelete);
  m_allocator.deallocate(pDelete, 1);
}

//==============================================================================
///@brief Removes all elements from the Queue.
///@note  If you store pointers to objects on the heap, you will have to
/// make sure that these are deallocated.
template  <typename Type, class AllocType>
void  Queue<Type, AllocType>::clear()
{
  NodeType  *pHead(m_pHead);
  while (pHead != 0)
  {
    NodeType  *pTemp(static_cast<NodeType*>(pHead->pNext));
    m_allocator.destroy(pHead);
    m_allocator.deallocate(pHead, 1);
    pHead = pTemp;
  }
  m_pHead = pHead;
  //m_pTail = 0;	// not necessary; we're only accessing m_pTail when m_pHead != 0
}

//==============================================================================
///@brief Adds all the elements from @a rhs to the end of the Queue.
template  <typename Type, class AllocType>
void  Queue<Type, AllocType>::adopt(SelfType& rhs)
{
  SelfType  temp(m_allocator);
  std::swap(rhs, temp);

  _adopt(temp);
}

//==============================================================================
///@brief Adds all the elements from @a rhs to the end of the Queue, up to,
/// not including @a end.
///@note  @a end has to be a part of @a rhs.
template  <typename Type, class AllocType>
void  Queue<Type, AllocType>::adopt(SelfType& rhs, iterator end)
{
  const NodeType * const   pEnd(end.pNode);

  if (!(rhs.empty() || rhs.m_pHead == end.pNode))
  {
    SelfType  temp(m_allocator);
    NodeType  *pNode(rhs.m_pHead);
    temp.m_pHead = pNode;

    do
    {
      temp.m_pTail = pNode;
      pNode = static_cast<NodeType*>(pNode->pNext);
    }
    while (pNode != pEnd);

    rhs.m_pHead = pNode;
    _adopt(temp);

    m_pTail->pNext = 0;
  }
}

//==============================================================================
template  <typename Type, class AllocType>
typename Queue<Type, AllocType>::SelfType&  Queue<Type, AllocType>::operator=(const Queue& rhs)
{
  SelfType  temp(rhs);

  // swap(temp);  // we've got something faster.
  std::swap(m_pHead, temp.m_pHead);
  m_pTail = temp.m_pTail;

  return *this;
}

//==============================================================================
template  <typename Type, class AllocType>
void  Queue<Type, AllocType>::_adopt(SelfType& rhs)
{
  XR_ASSERT(Queue, &rhs != this);

  if (empty())
  {
    m_pHead = rhs.m_pHead;
  }
  else
  {
    m_pTail->pNext = rhs.m_pHead;
  }

  if (!rhs.empty())
  {
    rhs.m_pHead = 0;

    m_pTail = rhs.m_pTail;
    //rhs.m_pTail = 0;	// not necessary; we're only accessing m_pTail when m_pHead != 0
  }
}

}  // XR

#endif  //XR_QUEUE_HPP
