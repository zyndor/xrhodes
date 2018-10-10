#ifndef XR_LINKED_HPP
#define XR_LINKED_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <functional>

namespace xr
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
  ///@brief Calls @a fn with each item in the Linked<> list for this type.
  static void ForEach(std::function<void(T&)> fn);

  // structors
  Linked(T& obj);
  ~Linked();

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

//==============================================================================
// inline
//==============================================================================
template<typename T>
void Linked<T>::ForEach(std::function<void(T&)> fn)
{
  Linked<T>* p = s_head;
  while (p)
  {
    auto pp = p->m_prev;
    fn(*p->data);
    p = pp;
  }
}

//==============================================================================
template<typename T>
Linked<T>::Linked(T & obj)
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

//==============================================================================
template<typename T>
Linked<T>::~Linked()
{
  if (this == s_head)
  {
    s_head = m_prev;
  }

  if (m_prev)
  {
    m_prev->m_next = m_next;
  }

  if (m_next)
  {
    m_next->m_prev = m_prev;
  }
}

}

#endif //XR_LINKED_HPP
