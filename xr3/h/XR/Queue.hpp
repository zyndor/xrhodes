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
  
  NodeType  *m_pHead;
  NodeType  *m_pTail;
};

} // XR

#include "Queue.inl"

#endif  //XR_QUEUE_HPP
