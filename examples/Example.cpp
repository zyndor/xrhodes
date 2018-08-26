//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Example.hpp"
#include <algorithm>
#include <vector>

namespace
{

Example* s_examples = nullptr;

}

Example* Example::GetFirst()
{
  return s_examples;
}

Example::Example(char const* name)
: m_name(name),
  m_previous(nullptr),
  m_next(nullptr)
{
  Register(this);
}

Example::~Example()
{
  if (m_previous)
  {
    m_previous->m_next = m_next;
  }

  if (m_next)
  {
    m_next->m_previous = m_previous;
  }

  if (s_examples == this)
  {
    s_examples = s_examples->m_next;
  }
}

std::string const & Example::GetName() const
{
  return m_name;
}

Example* Example::GetNext() const
{
  return m_next;
}

Example* Example::GetPrevious() const
{
  return m_previous;
}

void Example::Register(Example* e)
{
  // Find the nodes to insert between.
  Example** i = &s_examples;
  Example* previous = nullptr;
  while (*i)
  {
    if ((*i)->GetName() < e->GetName())
    {
      previous = *i;
      i = &(*i)->m_next;
    }
    else
    {
      break;
    }
  }

  // Set neighbours of new node.
  e->m_previous = previous;
  e->m_next = *i;

  // Set new node on neighbours
  if (*i)
  {
    (*i)->m_previous = e;
  }

  if (e->m_previous)  // since *i is a reference to [the local] previous, we will have overwritten it.
  {
    e->m_previous->m_next = e;
  }

  // Set node on insertion pointer (might be s_example or a neighbour's m_next.
  *i = e;

  // trim s_examples back. not sure if necessary.
  while (s_examples->m_previous)
  {
    s_examples = s_examples->m_previous;
  }
}

