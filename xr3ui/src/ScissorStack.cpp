//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/ScissorStack.hpp"
#include "xr/Gfx.hpp"
#include <vector>

namespace xr
{
namespace
{

struct Impl
{
  struct Entry
  {
    Rect const* original;
    Rect actual;  // ignore if !original
  };

  Impl()
  {
    m_stack.reserve(4);
  }

  void Push(Rect const* rect, bool intersect)
  {
    Entry e = { rect };
    if (rect)
    {
      if (intersect && !m_stack.empty() && m_stack.back().original)
      {
        e.actual = m_stack.back().actual.Intersection(*rect);
      }
      else
      {
        e.actual = *rect;
      }
    }

    m_stack.push_back(e);
    Gfx::SetScissor(rect ? &e.actual : nullptr);
  }

  Rect const* GetCurrent() const
  {
    return (m_stack.empty() || !m_stack.back().original) ? nullptr :
      &m_stack.back().actual;
  }

  void Pop()
  {
    m_stack.pop_back();
    Gfx::SetScissor(GetCurrent());
  }

private:
  std::vector<Entry> m_stack;
};

Impl* s_impl = nullptr;

} // nonamespace

//==============================================================================
void ScissorStack::Init()
{
  XR_ASSERTMSG(ScissorStack, !s_impl, ("Already initialized."));
  s_impl = new Impl();
  Gfx::ShutdownSignal().Connect(FunctionPtrCallback<void>([](void*) {
    delete s_impl;
    s_impl = nullptr;
  }, nullptr));
}

//==============================================================================
void ScissorStack::RequireInit()
{
  if (!s_impl)
  {
    Init();
  }
}

//==============================================================================
void ScissorStack::Push(Rect const* rect, bool intersect)
{
  XR_ASSERTMSG(ScissorStack, s_impl, ("Not initialized."));
  s_impl->Push(rect, intersect);
}

//==============================================================================
void ScissorStack::Pop()
{
  XR_ASSERTMSG(ScissorStack, !s_impl, ("Not initialized."));
  s_impl->Pop();
}

//==============================================================================
ScissorStack::Scope::Scope(Rect const* rect, bool intersect)
{
  ScissorStack::Push(rect, intersect);
}

//==============================================================================
ScissorStack::Scope::~Scope()
{
  ScissorStack::Pop();
}

//==============================================================================
Rect const* ScissorStack::GetCurrent()
{
  return s_impl->GetCurrent();;
}

} // xr
