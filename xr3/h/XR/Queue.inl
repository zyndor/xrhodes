//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

// no guard

namespace XR
{
namespace QueueImpl
{
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

} // QueueImpl

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
  return m_pHead == 0;
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
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::iterator  Queue<Type, AllocType>::begin()
{
  return iterator(m_pHead);
}

//==============================================================================
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::const_iterator  Queue<Type, AllocType>::begin() const
{
  return const_iterator(m_pHead);
}

//==============================================================================
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::iterator  Queue<Type, AllocType>::end()
{
  return iterator(0);
}

//==============================================================================
template  <typename Type, class AllocType>
inline
typename Queue<Type, AllocType>::const_iterator  Queue<Type, AllocType>::end() const
{
  return const_iterator(0);
}

//==============================================================================
template  <typename Type, class AllocType>
inline
Type&  Queue<Type, AllocType>::front()
{
  XR_ASSERT(Queue, !empty());
  return m_pHead->data;
}

//==============================================================================
template  <typename Type, class AllocType>
inline
const Type&  Queue<Type, AllocType>::front() const
{
  XR_ASSERT(Queue, !empty());
  return m_pHead->data;
}

//==============================================================================
template  <typename Type, class AllocType>
inline
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
template  <typename Type, class AllocType>
inline
void  Queue<Type, AllocType>::pop_front()
{
  XR_ASSERT(Queue, !empty());
  NodeType  *pDelete(m_pHead);
  m_pHead = static_cast<NodeType*>(m_pHead->pNext);
  m_allocator.destroy(pDelete);
  m_allocator.deallocate(pDelete, 1);
}

//==============================================================================
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

} // XR
