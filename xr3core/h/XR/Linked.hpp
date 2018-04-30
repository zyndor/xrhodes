#ifndef XR_LINKED_HPP
#define XR_LINKED_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <functional>

namespace XR
{

//==============================================================================
///@brief Intrusive doubly linked list template, which allows processing every
/// one of its elements in one go. Each new instance adds itself to the end of
/// its Linked<> list upon construction, and unlinks itself upon destruction.
/// The primary intended use of Linked<> is to tie together objects that may be
/// defined in different translation units, typically as globals (file statics).
/// As such, no assumption should be made about the order of these objects in
/// the list.
/// The use of the Curiously Recurring Template Pattern is recommended, i.e.
/// derive YourClass from Linked<YourClass>.
template <typename T>
class Linked
{
public:
  // static
  static void ForEach(std::function<void(T&)> fn)
  {
    Linked<T>* p = s_head;
    while (p)
    {
      auto pp = p->m_prev;
      fn(*p->data);
      p = pp;
    }
  }

  // structors
  Linked(T& obj)
  : data(&obj),
    m_prev(s_head),
    m_next(nullptr)
  {
    if (s_head)
    {
      s_head->m_next = this;
    }

    s_head = this;
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
  static Linked<T>* s_head;

  // data
  Linked<T>* m_prev;
  Linked<T>* m_next;
};

template <typename T>
Linked<T>* Linked<T>::s_head = nullptr;

}

#endif //XR_LINKED_HPP
