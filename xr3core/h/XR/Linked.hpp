#ifndef XR_LINKED_HPP
#define XR_LINKED_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

namespace XR
{

//==============================================================================
///@brief Intrusive doubly linked list, whose primary intended use is for global
/// (static) objects. The use of the Curiously Recurring Template Pattern is
/// recommended, i.e. derive YourClass from Linked<YourClass>.
template <typename T>
class Linked
{
public:
  // static
  static void ForEach(void(*fn)(T&))
  {
    Linked<T>* p = s_root;
    while (p)
    {
      (*fn)(*p->data);
      p = p->m_next;
    }
  }

  // structors
  Linked(T& obj)
  : data(&obj),
    m_prev(nullptr),
    m_next(s_root)
  {
    if (s_root)
    {
      s_root->m_prev = this;
    }

    s_root = this;
  }

  ~Linked()
  {
    if (m_prev)
    {
      m_prev->m_next = m_next;
    }
    if (m_next)
    {
      m_next->m_prev = m_prev;
    }
  }

protected:
  // data
  T* data;

private:
  // static
  static Linked<T>* s_root;

  // data
  Linked* m_prev;
  Linked* m_next;
};

template <typename T>
Linked<T>* Linked<T>::s_root = nullptr;

}

#endif //XR_LINKED_HPP