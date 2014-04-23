//
// Nuclear Heart Games
// XRhodes
//
// Dictionary.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    06/12/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_DICTIONARY_HPP
#define XR_DICTIONARY_HPP

#include  <algorithm>
#include  <iterator>
#include  "types.hpp"

namespace XR
{

namespace DictionaryImpl
{

//==============================================================================
template  <typename Key, typename Value>
struct  Node
{
  // types
  typedef Node<Key, Value>  SelfType;

  typedef Key   key_type;
  typedef Value value_type;

  // data
  key_type    key;
  value_type  value;

  SelfType*   pLow;   // yes ownership
  SelfType*   pHigh;  // yes ownership

  // structors
  Node(Node* pParent, key_type k, value_type d = value_type())
  : key(k),
    value(d),
    pLow(0),
    pHigh(0),
    m_pParent(pParent)
  {}

  ~Node()
  {}

  // general
  SelfType* GetParent() const
  {
    return m_pParent;
  }

  void  SetParent(SelfType* p)
  {
    m_pParent = p;
  }

  size_t  CountChildren() const
  {
    return (pLow != 0 ? pLow->CountChildren() + 1: 0) +
      (pHigh != 0 ? pHigh->CountChildren() + 1: 0);
  }

  template <class Alloc>
  void  Add(key_type k, value_type d, Alloc& a)
  {
    if(k == key)
    {
      value = d;
    }
    else
    {
      SelfType*&  p = k < key ? pLow : pHigh;
      if(p == 0)
      {
        p = a.allocate(1);
        a.construct(p, SelfType(this, k, d));
      }
      else
      {
        p->Add(k, d, a);
      }
    }
  }

  template <class Alloc>
  SelfType* CloneRecursive(SelfType* pParent, Alloc& a)
  {
    SelfType* pNew(a.allocate(1));
    a.construct(pNew, SelfType(pParent, key, value));

    if(pLow != 0)
    {
      pNew->pLow = pLow->CloneRecursive(pParent, a);
    }
    if(pHigh != 0)
    {
      pNew->pHigh = pHigh->CloneRecursive(pParent, a);
    }
    return pNew;
  }

  template <class Alloc>
  void  Destroy(Alloc& a)
  {
    if(pLow != 0)
    {
      pLow->Destroy(a);

      a.destroy(pLow);
      a.deallocate(pLow, 1);
    }

    if(pHigh != 0)
    {
      pHigh->Destroy(a);

      a.destroy(pHigh);
      a.deallocate(pHigh, 1);
    }
  }

protected:
  // data
  SelfType* m_pParent;  // no ownership
};

//==============================================================================
template  <typename Key, typename Value>
struct  Iterator 
{
  // typedef
  typedef Iterator<Key, Value> Type;
  typedef Node<Key, Value>     NodeType;
  
  typedef NodeType* pointer;
  typedef NodeType& reference;

  // data
  pointer pNode;  // no ownership

  // structors
  Iterator()
  : pNode(0)
  {}

  explicit Iterator(pointer p)
  : pNode(p)
  {}

  // general use
  bool  IsValid() const
  {
    return pNode != 0;
  }

  Iterator  Next() const
  {
    XR_ASSERT(Dictionary::Iterator, IsValid());
    Iterator next(pNode);
    return ++next;
  }

  Iterator  Prev() const
  {
    XR_ASSERT(Dictionary::Iterator, IsValid());
    Iterator next(pNode);
    return --next;
  }

  // overloads
  reference  operator *() const
  {
    return *pNode;
  }
  
  pointer    operator ->() const
  {
    return pNode;
  }

  Iterator&  operator++()
  {
    XR_ASSERT(Dictionary::Iterator, IsValid());
    pointer p(pNode->pHigh);
    if(p != 0)
    {
      while(p->pLow != 0)
      {
        p = p->pLow;
      }
    }
    else
    {
      p = static_cast<pointer>(pNode->GetParent());
      while(p != 0)
      {
        if(p->key > pNode->key)
        {
          break;
        }
        p = static_cast<pointer>(p->GetParent());
      }
    }

    pNode = p;
    return *this;
  }

  Iterator  operator++(int)
  {
    Iterator  temp(*this);
    ++*this;
    return temp;
  }

  Iterator&  operator--()
  {
    XR_ASSERT(Dictionary::Iterator, IsValid());
    pointer p(pNode->pLow);
    if(p != 0)
    {
      while(p->pHigh != 0)
      {
        p = p->pHigh;
      }
    }
    else
    {
      p = pNode->GetParent();
      while(p != 0)
      {
        if(p->key < pNode->key)
        {
          break;
        }
        p = p->GetParent();
      }
    }

    pNode = p;
    return *this;
  }
  
  Iterator  operator--(int)
  {
    Iterator  temp(*this);
    --(*this);
    return temp;
  }

  bool  operator==(const Iterator& rhs) const
  {
    return pNode == rhs.pNode;
  }

  bool  operator!=(const Iterator& rhs) const
  {
    return pNode != rhs.pNode;
  }
};

//==============================================================================
template  <typename Key, typename Value>
struct  ConstIterator 
{
  // typedef
  typedef ConstIterator<Key, Value> Type;
  typedef const Node<Key, Value>    NodeType;

  typedef Iterator<Key, Value> iterator;
  
  typedef NodeType* pointer;
  typedef NodeType& reference;

  // data
  pointer pNode;  // no ownership

  // structors
  ConstIterator()
  : pNode(0)
  {}

  ConstIterator(iterator i)
  : pNode(i.pNode)
  {}

  explicit ConstIterator(pointer p)
  : pNode(p)
  {}

  // general use
  bool  IsValid() const
  {
    return pNode != 0;
  }

  ConstIterator  Next() const
  {
    XR_ASSERT(Dictionary::ConstIterator, IsValid());
    ConstIterator next(pNode);
    return ++next;
  }

  ConstIterator  Prev() const
  {
    XR_ASSERT(Dictionary::ConstIterator, IsValid());
    ConstIterator next(pNode);
    return --next;
  }

  // overloads
  reference  operator *() const
  {
    return *pNode;
  }
  
  pointer    operator ->() const
  {
    return pNode;
  }

  ConstIterator&  operator++()
  {
    XR_ASSERT(Dictionary::ConstIterator, IsValid());
    pointer p(pNode->pHigh);
    if(p != 0)
    {
      while(p->pLow != 0)
      {
        p = p->pLow;
      }
    }
    else
    {
      p = pNode->GetParent();
      while(p != 0)
      {
        if(p->key > pNode->key)
        {
          break;
        }
        p = p->GetParent();
      }
    }

    pNode = p;
    return *this;
  }

  ConstIterator  operator++(int)
  {
    ConstIterator  temp(*this);
    ++(*this);
    return temp;
  }

  ConstIterator&  operator--()
  {
    XR_ASSERT(Dictionary::ConstIterator, IsValid());
    pointer p(pNode->pHigh);
    if(p != 0)
    {
      while(p->pLow != 0)
      {
        p = p->pLow;
      }
    }
    else
    {
      p = pNode->GetParent();
      while(p != 0)
      {
        if(p->key > pNode->key)
        {
          break;
        }
        p = p->GetParent();
      }
    }

    pNode = p;
    return *this;
  }
  
  ConstIterator  operator--(int)
  {
    ConstIterator  temp(*this);
    --(*this);
    return temp;
  }

  bool  operator==(const ConstIterator& rhs) const
  {
    return pNode == rhs.pNode;
  }

  bool  operator!=(const ConstIterator& rhs) const
  {
    return pNode != rhs.pNode;
  }
};

} // DictionaryImpl

//==============================================================================
template  <typename Key, typename Value, class Alloc = std::allocator<Value> >
class Dictionary
{
public:
  // types
  typedef Key                                               key_type;
  typedef Value                                             value_type;

  typedef DictionaryImpl::Node<Key, Value>                  NodeType;

  typedef DictionaryImpl::Iterator<Key, Value>              iterator;
  typedef DictionaryImpl::ConstIterator<Key, Value>         const_iterator;

  typedef Alloc                                             allocator;
  
  typedef size_t                                            size_type;
  typedef ptrdiff_t                                         difference_type;

  typedef typename allocator::reference                     reference;
  typedef typename allocator::const_reference               const_reference;
  typedef typename allocator::pointer                       pointer;
  typedef typename allocator::const_pointer                 const_pointer;

  typedef typename Alloc::template rebind<NodeType>::other  NodeAllocType;

  // structors
  Dictionary(allocator a = allocator());
  Dictionary(const Dictionary& rhs);
  ~Dictionary();

  // general
  ///@return  Whether the Dictionary has any elements.
  bool              empty() const;

  ///@return  The number of elements in the Dictionary.
  size_t            size() const;

  ///@brief Inserts the value @a d at the key @a k.
  void              insert(key_type k, value_type d);

  ///@return  An iterator to the element keyed to @a k, or end() if it was not
  /// found.
  iterator          find(key_type k);

  ///@return  An const_iterator to the element keyed to @a k, or end() if it
  /// was not found.
  const_iterator    find(key_type k) const;

  ///@return  A pointer to the element keyed to @a k, or 0 if it was not found.
  value_type*       find_value(key_type k);

  ///@return  A const pointer to the element keyed to @a k, or 0 if it was not
  /// found.
  const value_type* find_value(key_type k) const;

  ///@brief Removes all elements from the Dictionary.
  ///@note If you are storing pointers to objects on the heap in the
  /// Dictionary, you will have to deallocate them yourself.
  void              clear();

  ///@return  An iterator to the first element in the Dictionary. If the
  /// Dictionary is empty, this will equal end().
  iterator          begin();

  ///@return  A const_iterator to the first element in the Dictionary. If the
  /// Dictionary is empty, this will equal end().
  const_iterator    begin() const;

  ///@return  An iterator past the last element in the Dictionary.
  ///@note  It is illegal to dereference the result.
  iterator          end();

  ///@return  A const_iterator past the last element in the Dictionary.
  ///@note  It is illegal to dereference the result.
  const_iterator    end() const;

  // overloads
  ///@return  A reference to the element keyed to @a k.
  ///@note  If the key-value pair doesn't exist, this will create it, i.e.
  /// memory is allocated and the constructor (if any) is called.
  value_type& operator[](key_type k);

  Dictionary& operator==(const Dictionary& rhs);

protected:
  // internal
  NodeType**        _Get(key_type k);
  NodeType*const *  _Get(key_type k) const;

  // data
  NodeAllocType m_allocator;
  NodeType*     m_pRoot;
};

//==============================================================================
// implementation
//==============================================================================
template  <typename Key, typename Value, class Alloc>
Dictionary<Key, Value, Alloc>::Dictionary(allocator a)
: m_allocator(a),
  m_pRoot(0)  
{}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
Dictionary<Key, Value, Alloc>::Dictionary(const Dictionary& rhs)
: m_allocator(rhs.m_allocator),
  m_pRoot(0)
{
  if(rhs.m_pRoot != 0)
  {
    m_pRoot = rhs.m_pRoot->CloneRecursive(0, m_allocator);
  }
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
Dictionary<Key, Value, Alloc>::~Dictionary()
{
  clear();
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
bool  Dictionary<Key, Value, Alloc>::empty() const
{
  return m_pRoot == 0;
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
size_t  Dictionary<Key, Value, Alloc>::size() const
{
  return (m_pRoot != 0 ? m_pRoot->CountChildren() + 1 : 0);
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
void  Dictionary<Key, Value, Alloc>::insert(key_type k, value_type d)
{
  if(m_pRoot == 0)
  {
    //m_pRoot = new NodeType(0, k, d);
    m_pRoot = m_allocator.allocate(1);
    m_allocator.construct(m_pRoot, NodeType(0, k, d));
  }
  else
  {
    m_pRoot->Add(k, d, m_allocator); 
  }
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
Value*  Dictionary<Key, Value, Alloc>::find_value(key_type k)
{
  NodeType* p(*_Get(k));
  return (p != 0) ? &(p->value) : 0;
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
const Value*  Dictionary<Key, Value, Alloc>::find_value(key_type k) const
{
  const NodeType* p(*_Get(k));
  return (p != 0) ? &(p->value) : 0;
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
typename Dictionary<Key, Value, Alloc>::iterator Dictionary<Key, Value, Alloc>::find(key_type k)
{
  return iterator(*_Get(k));
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
typename Dictionary<Key, Value, Alloc>::const_iterator Dictionary<Key, Value, Alloc>::find(key_type k) const
{
  return const_iterator(*_Get(k));
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
void  Dictionary<Key, Value, Alloc>::clear()
{
  if(m_pRoot != 0)
  {
    m_pRoot->Destroy(m_allocator);

    m_allocator.destroy(m_pRoot);
    m_allocator.deallocate(m_pRoot, 1);
    m_pRoot = 0;
  }
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
typename Dictionary<Key, Value, Alloc>::NodeType**
  Dictionary<Key, Value, Alloc>::_Get(key_type k)
{
  NodeType**  pp = &m_pRoot;
  while(*pp != 0)
  {
    NodeType* p(*pp);
    if(k == p->key)
    {
      break;
    }
    else
    {
      NodeType*&  pTemp = (k < p->key) ? p->pLow : p->pHigh;
      pp = &pTemp;
    }
  }
  return pp;
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
typename Dictionary<Key, Value, Alloc>::NodeType*const *
  Dictionary<Key, Value, Alloc>::_Get(key_type k) const
{
  NodeType*const *  pp = &m_pRoot;
  while(*pp != 0)
  {
    NodeType* p(*pp);
    if(k == p->key)
    {
      break;
    }
    else
    {
      NodeType*&  pTemp = (k < p->key) ? p->pLow : p->pHigh;
      pp = &pTemp;
    }
  }
  return pp;
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
typename Dictionary<Key, Value, Alloc>::iterator
  Dictionary<Key, Value, Alloc>::begin()
{
  NodeType* pNode(m_pRoot);
  if(pNode != 0)
  {
    while(pNode->pLow != 0)
    {
      pNode = pNode->pLow;
    }
  }
  return iterator(pNode);
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
typename Dictionary<Key, Value, Alloc>::const_iterator
  Dictionary<Key, Value, Alloc>::begin() const
{
  NodeType* pNode(m_pRoot);
  if(pNode != 0)
  {
    while(pNode->pLow != 0)
    {
      pNode = pNode->pLow;
    }
  }
  return const_iterator(pNode);
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
typename Dictionary<Key, Value, Alloc>::iterator
  Dictionary<Key, Value, Alloc>::end()
{
  return iterator(0);
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
typename Dictionary<Key, Value, Alloc>::const_iterator
  Dictionary<Key, Value, Alloc>::end() const
{
  return const_iterator(0);
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
Value&  Dictionary<Key, Value, Alloc>::operator[](key_type k)
{
  NodeType**  pp = _Get(k);
  NodeType*   p(*pp);
  bool  found(p != 0);
  if(found)
  {
    return p->value;
  }
  else
  {
    //(*pp) = new NodeType(*pp, k);
    (*pp) = m_allocator.allocate(1);
    m_allocator.construct((*pp), NodeType(p, k));
    return (*pp)->value;
  }
}

//==============================================================================
template  <typename Key, typename Value, class Alloc>
Dictionary<Key, Value, Alloc>& Dictionary<Key, Value, Alloc>::operator==(const Dictionary& rhs)
{
  Dictionary  temp(rhs);
  m_allocator = temp.m_allocator;
  m_pRoot = temp.m_pRoot;
  temp.m_pRoot = 0;
  return *this;
}

} // XR

#endif // XR_DICTIONARY_HPP
