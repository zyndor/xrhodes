#ifndef XR_QUADTREE_HPP
#define XR_QUADTREE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "RectObject.hpp"
#include "AABB.hpp"
#include "xr/memory/Queue.hpp"

namespace xr
{

//==============================================================================
class QuadtreeCore: protected RectObject
{
public:
  // types
  enum  Leaf
  {
    SW,
    SE,
    NW,
    NE,
    kNumLeaves
  };

  enum { kDefaultSubdivisionLimit = 5 };

  // types
  ///@brief A unary function that you pass to Quadtree::Process() and will be
  /// called with each collision candidate as arguments.
  using UnaryCallback = void(*)(void* systemData);

  ///@brief A binary function that you pass to Quadtree::Process() and will be
  /// called with each collision candidate as arguments: the first one is the
  /// one in the Quadtree, the second one is the one you passed to Process().
  using BinaryCallback = void(*)(void* systemData, void* userData);

  using RectObject::GetPosition;
  using RectObject::GetHalfWidth;
  using RectObject::GetHalfHeight;

  // static
  ///@return Whether a node of half-width @a hw and half-height @a hh can
  /// split, i.e. half of its smaller dimension is greater than the @a min.
  /// False means no child nodes will be created.
  static bool   CalculateCanSplit(float hw, float hh, float min);

  ///@brief Calculates the minimum size of node (along either axis), based on
  /// @a hw, @a hh and @a s_divisions.
  static float  CalculateMin(float hw, float hh, uint32_t maxSubdivisions = kDefaultSubdivisionLimit);

  [[deprecated]]
  static float  CalculateMin(float hw, float hh);

  [[deprecated]]
  static int  GetMaxSubdivisions();

  [[deprecated("Set via Create().")]]
  static void SetMaxSubdivisions(int maxSubdivisions);

  [[deprecated]]
  static AABB const&  GetCandidateBox();

  // general
  [[deprecated]]
  bool  CanSplit() const;

protected:
  // static
  static int  sMaxSubdivisions;

  // data
  QuadtreeCore* mLeaves[4]{};

  // structors
  QuadtreeCore();
  QuadtreeCore(const Vector2& pos, float hw, float hh);

  ~QuadtreeCore();

  // general
  ///@return If the Quadtree is subdivided, i.e. if child nodes are created.
  bool  IsSubdivided() const;
};

//==============================================================================
///@brief Rectangle Quadtree class that pre-creates its structure and stores
/// and processes items as void*, based on their AABB's. You can specify the
/// minimum size of a node in the @a min parameter of the constructor. If omitted,
/// kDefaultSubdivisionLimit subdivisions are created.
template <template <typename> class Alloc = std::allocator>
class Quadtree: public QuadtreeCore
{
public:
  // types
  using AllocType = Alloc<void*>;
  using QueueType = Queue<void*, AllocType>;
  using SelfType = Quadtree<Alloc>;

  // structors
  explicit Quadtree(AllocType a = AllocType());
  Quadtree(const Vector2& pos, float hw, float hh, float min = 0,
    AllocType a = AllocType());
  ~Quadtree();

  // general use
  ///@brief Creates the Quadtree at the given starting position @a pos and half-
  /// size (@a hw, @a hh), subdividing while any of the nodes' dimensions is >
  /// @a min.
  ///@note Previously added objects are lost.
  void  Create(Vector2 const& pos, float hw, float hh, float min);

  ///@brief Creates the Quadtree based on the given bounding @a box, subdividing
  /// while any of the nodes' dimensions is > @a min.
  ///@note Previously added objects are lost.
  void  Create(AABB const& box, float min);

  ///@brief Offsets the position of the Quadtree (and all of its leaves) by @a delta,
  /// without re-creating its structure.
  void  Translate(Vector2 const& delta);

  ///@brief Multiplies the half-width and -height of the Quadtree (and all of its
  /// leaves) by @a sx and @a sy respectively, without re-creating its structure.
  /// In additon, the position of the leaves are adjusted to fulfil the Quadtree
  /// criterion.
  void  Scale(float sx, float sy);

  ///@brief Multiplies the half-width and -height of the Quadtree (and all of its
  /// leaves) by @a s, without re-creating its structure. In additon, the position
  /// of the leaves are adjusted to fulfil the Quadtree criterion.
  void  Scale(float s);

  ///@brief Adds the @a object bounded by the AABB @a box, to the Quadtree.
  ///@note Does not transfer ownership of @a object to the Quadtree.
  void  Add(const AABB& box, void* object);

  ///@brief Adds the point @a object at position @a pos, to the Quadtree.
  ///@note Does not transfer ownership of @a object to the Quadtree.
  void  Add(const Vector2& pos, void* object);

  ///@brief Checks the objects in the Quadtree against the AABB @a box. @a cb
  /// is invoked with each collision candidate (i.e. the ones in a quadrant that
  /// @a box intesects).
  ///@note You will need to check for actual collision.
  void  Process(const AABB& box, UnaryCallback cb);

  ///@brief Checks the objects in the Quadtree against the point @a pos. @a cb
  /// is invoked with each collision candidate (i.e. the ones in a quadrant that
  /// the point @a pos is inside).
  ///@note You will need to check for actual collision.
  void  Process(const Vector2& pos, UnaryCallback cb);

  ///@brief Checks the objects in the Quadtree against the AABB @a box. @a cb
  /// is invoked with each collision candidate (i.e. the ones in a quadrant that
  /// @a box intesects) and @a userData.
  ///@note You will need to check for actual collision.
  void  Process(const AABB& box, BinaryCallback cb, void* userData);

  ///@brief Checks the objects in the Quadtree against the point @a pos. @a cb
  /// is invoked with each collision candidate (i.e. the ones in a quadrant that
  /// the point @a pos is inside) and @a userData.
  ///@note You will need to check for actual collision.
  void  Process(const Vector2& pos, BinaryCallback cb, void* userData);

  ///@brief Invokes @a cb with each and every object in the Quadtree.
  void  ProcessAll(UnaryCallback cb);

  ///@brief Invokes @a cb with each and every object in the Quadtree and @a userData.
  void  ProcessAll(BinaryCallback cb, void* userData);

  ///@brief Removes from the Quadtree the first object whose address is @a object.
  ///@note The assumption is made that the object has been inserted into the
  /// Quadtree once at most.
  void  Remove(void* object);

  ///@brief Convenience function to remove an @a object and reinsert it at using
  /// @a box as its new boundaries.
  void  Update(const AABB& box, void* object);

  ///@brief Clears the Quadtree of objects.
  void  Clear();

  ///@brief Clears the Quadtree, deletes all four leaves and resets the size of
  /// the Quadtree to { 0.; 0. }.
  ///@note  Does not reset the position of the Quadtree.
  void  Destroy();

  [[deprecated]]
  SelfType* GetLeaf(Leaf l) const;

private:
  // internal use
  void  CreateRecursion(float min);
  void  AddRecursion(AABB const& box, void* object);
  void  ProcessRecursion(AABB const& box, UnaryCallback cb);
  void  ProcessRecursion(AABB const& box, BinaryCallback cb, void* userData);
  void  ProcessAllRecursion(UnaryCallback cb);
  void  ProcessAllRecursion(BinaryCallback cb, void* userData);
  void  RemoveRecursion(void* object);

  // data
  QueueType mObjects;
};

//==============================================================================
// implementation
//==============================================================================
inline
bool  QuadtreeCore::CalculateCanSplit(float hw, float hh, float min)
{
  XR_ASSERT(Quadtree, hw >= .0f);
  XR_ASSERT(Quadtree, hh >= .0f);
  XR_ASSERT(Quadtree, min > .0f);
  return std::min(hw, hh) > min;
}

//==============================================================================
inline
bool  QuadtreeCore::CanSplit() const
{
  return IsSubdivided();
}

//==============================================================================
inline
bool  QuadtreeCore::IsSubdivided() const
{
  return mLeaves[0] != nullptr;
}

//==============================================================================
template <template <typename> class Alloc>
Quadtree<Alloc>::Quadtree(AllocType a)
: QuadtreeCore(),
  mObjects(a)
{}

//==============================================================================
template <template <typename> class Alloc>
Quadtree<Alloc>::Quadtree(const Vector2 &pos, float hw, float hh, float min,
  AllocType a)
: QuadtreeCore(pos, hw, hh),
  mObjects(a)
{
  CreateRecursion(min);
}

//==============================================================================
template <template <typename> class Alloc>
Quadtree<Alloc>::~Quadtree()
{
  Destroy();
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::Create(const Vector2 &pos, float hw, float hh, float min)
{
  XR_ASSERT(Quadtree, hw > .0f);
  XR_ASSERT(Quadtree, hh > .0f);
  XR_ASSERT(Quadtree, min > std::numeric_limits<float>::epsilon());
  Destroy();

  mPosition = pos;
  mHalfWidth = hw;
  mHalfHeight = hh;

  CreateRecursion(min);
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::Create(AABB const& box, float min)
{
  XR_ASSERT(Quadtree, box.right > box.left);
  XR_ASSERT(Quadtree, box.top > box.bottom);
  XR_ASSERT(Quadtree, min > std::numeric_limits<float>::epsilon());
  Destroy();

  box.Export(mPosition.x, mPosition.y, mHalfWidth, mHalfHeight);

  CreateRecursion(min);
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::Translate(Vector2 const& delta)
{
  mPosition += delta;

  if (IsSubdivided())
  {
    for (auto l : mLeaves)
    {
      static_cast<SelfType*>(l)->Translate(delta);
    }
  }
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::Scale(float sx, float sy)
{
  XR_ASSERT(Quadtree, sx * sx > std::numeric_limits<float>::epsilon());
  XR_ASSERT(Quadtree, sy * sy > std::numeric_limits<float>::epsilon());
  float hw = mHalfWidth * sx;
  float hh = mHalfHeight * sy;
  mHalfWidth = hw;
  mHalfHeight = hh;

  if (IsSubdivided())
  {
    hw *= .5f;
    hh *= .5f;
    auto l = mLeaves;

    static_cast<SelfType*>(*l)->mPosition = mPosition + Vector2{ -hw, -hh };
    static_cast<SelfType*>(*l)->Scale(sx, sy);
    ++l;

    static_cast<SelfType*>(*l)->mPosition = mPosition + Vector2{ hw, -hh };
    static_cast<SelfType*>(*l)->Scale(sx, sy);
    ++l;

    static_cast<SelfType*>(*l)->mPosition = mPosition + Vector2{ -hw, hh };
    static_cast<SelfType*>(*l)->Scale(sx, sy);
    ++l;

    static_cast<SelfType*>(*l)->mPosition = mPosition + Vector2{ hw, hh };
    static_cast<SelfType*>(*l)->Scale(sx, sy);
  }
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Scale(float s)
{
  Scale(s, s);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Add(AABB const& box, void* object)
{
  XR_ASSERT(Quadtree, object != nullptr);
  XR_ASSERT(Quadtree, box.left <= box.right);
  XR_ASSERT(Quadtree, box.bottom <= box.top);
  AddRecursion(box, object);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Add(Vector2 const& pos, void* object)
{
  XR_ASSERT(Quadtree, object != nullptr);
  AddRecursion(AABB{ pos.x, pos.y, pos.x, pos.y }, object);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const AABB& box, UnaryCallback cb)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  XR_ASSERT(Quadtree, box.left <= box.right);
  XR_ASSERT(Quadtree, box.bottom <= box.top);
  ProcessRecursion(box, cb);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const Vector2& pos, UnaryCallback cb)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  AABB box;
  box.Import(pos.x, pos.y, 0.f, 0.f);
  ProcessRecursion(box, cb);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(AABB const& box, BinaryCallback cb, void* userData)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  XR_ASSERT(Quadtree, box.left <= box.right);
  XR_ASSERT(Quadtree, box.bottom <= box.top);
  ProcessRecursion(box, cb, userData);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const Vector2& pos, BinaryCallback cb, void* userData)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  AABB box;
  box.Import(pos.x, pos.y, .0f, .0f);
  ProcessRecursion(box, cb, userData);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::ProcessAll(UnaryCallback cb)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  ProcessAllRecursion(cb);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::ProcessAll(BinaryCallback cb, void* userData)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  ProcessAllRecursion(cb, userData);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Remove(void* object)
{
  XR_ASSERT(Quadtree, object != nullptr);
  RemoveRecursion(object);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Update(const AABB& box, void* object)
{
  Remove(object);
  Add(box, object);
}

//==============================================================================
template <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Clear()
{
  mObjects.clear();
  if (IsSubdivided())
  {
    for (auto l : mLeaves)
    {
      static_cast<SelfType*>(l)->Clear();
    }
  }
}

//==============================================================================
template <template <typename> class Alloc>
Quadtree<Alloc>*  Quadtree<Alloc>::GetLeaf(Leaf l) const
{
  XR_ASSERT(Quadtree, l < kNumLeaves);
  return static_cast<SelfType*>(mLeaves[l]);
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::AddRecursion(AABB const& box, void* object)
{
  if (IsSubdivided())
  {
    if (box.top < mPosition.y)
    {
      if (box.right < mPosition.x)
      {
        static_cast<SelfType*>(mLeaves[SW])->AddRecursion(box, object);
        return;
      }
      else if (box.left >= mPosition.x)
      {
        static_cast<SelfType*>(mLeaves[SE])->AddRecursion(box, object);
        return;
      }
    }
    else if (box.bottom >= mPosition.y)
    {
      if (box.right < mPosition.x)
      {
        static_cast<SelfType*>(mLeaves[NW])->AddRecursion(box, object);
        return;
      }
      else if (box.left >= mPosition.x)
      {
        static_cast<SelfType*>(mLeaves[NE])->AddRecursion(box, object);
        return;
      }
    }
  }

  mObjects.push_back(object);
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::ProcessRecursion(AABB const& box, UnaryCallback cb)
{
  // All objects touching more than one leaf are candidates to collision -
  // these happen to reside in the node.
  for (auto o : mObjects)
  {
    cb(o);
  }

  // ...and process all objects in the child nodes that the box is touching
  if (IsSubdivided())
  {
    const bool  west(box.left < mPosition.x);
    const bool  east(box.right >= mPosition.x);

    if (box.bottom < mPosition.y)
    {
      if (west)
      {
        static_cast<SelfType*>(mLeaves[SW])->ProcessRecursion(box, cb);
      }

      if (east)
      {
        static_cast<SelfType*>(mLeaves[SE])->ProcessRecursion(box, cb);
      }
    }

    if (box.top >= mPosition.y)
    {
      if (west)
      {
        static_cast<SelfType*>(mLeaves[NW])->ProcessRecursion(box, cb);
      }

      if (east)
      {
        static_cast<SelfType*>(mLeaves[NE])->ProcessRecursion(box, cb);
      }
    }
  }
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::ProcessAllRecursion(UnaryCallback cb)
{
  // All objects touching all leaves are candidates to collision - these
  // happen to reside in the node.
  for (auto o : mObjects)
  {
    cb(o);
  }

  // ...and process all objects in the child nodes that the box is touching
  if (IsSubdivided())
  {
    for (auto l : mLeaves)
    {
      static_cast<SelfType*>(l)->ProcessAllRecursion(cb);
    }
  }
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::ProcessRecursion(AABB const& box, BinaryCallback cb, void* userData)
{
  // All objects touching more than one leaf are candidates to collision -
  // these happen to reside in the node.
  for (auto o : mObjects)
  {
    cb(o, userData);
  }

  // ...and process all objects in the child nodes that the box is touching
  if (IsSubdivided())
  {
    const bool  west(box.left < mPosition.x);
    const bool  east(box.right >= mPosition.x);

    if (box.bottom < mPosition.y)
    {
      if (west)
      {
        static_cast<SelfType*>(mLeaves[SW])->ProcessRecursion(box, cb, userData);
      }

      if (east)
      {
        static_cast<SelfType*>(mLeaves[SE])->ProcessRecursion(box, cb, userData);
      }
    }

    if (box.top >= mPosition.y)
    {
      if (west)
      {
        static_cast<SelfType*>(mLeaves[NW])->ProcessRecursion(box, cb, userData);
      }

      if (east)
      {
        static_cast<SelfType*>(mLeaves[NE])->ProcessRecursion(box, cb, userData);
      }
    }
  }
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::ProcessAllRecursion(BinaryCallback cb, void* userData)
{
  // All objects touching all leaves are candidates to collision - these
  // happen to reside in the node.
  for (auto o : mObjects)
  {
    cb(o, userData);
  }

  // ...and process all objects from the child nodes that the box is touching
  if (IsSubdivided())
  {
    for (auto l : mLeaves)
    {
      static_cast<SelfType*>(l)->ProcessAllRecursion(cb, userData);
    }
  }
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::RemoveRecursion(void* object)
{
  typename QueueType::iterator iEnd(mObjects.end());
  typename QueueType::iterator iFind(std::find(mObjects.begin(), iEnd, object));
  if (iFind != iEnd)
  {
    mObjects.adopt(mObjects, iFind);  // smooth move
    mObjects.pop_front();
  }
  else if (IsSubdivided())
  {
    for (auto l : mLeaves)
    {
      static_cast<SelfType*>(l)->RemoveRecursion(object);
    }
  }
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::Destroy()
{
  mHalfWidth = .0f;
  mHalfHeight = .0f;
  mObjects.clear();

  for (int i = 0; i < kNumLeaves; ++i)
  {
    delete static_cast<SelfType*>(mLeaves[i]);
  }

  std::fill(std::begin(mLeaves), std::end(mLeaves), nullptr);
}

//==============================================================================
template <template <typename> class Alloc>
void  Quadtree<Alloc>::CreateRecursion(float min)
{
  XR_ASSERT(Quadtree, min > std::numeric_limits<float>::epsilon());
  if (CalculateCanSplit(mHalfWidth, mHalfHeight, min))
  {
    float hw = mHalfWidth * .5f;
    float hh = mHalfHeight * .5f;
    mLeaves[SW] = new SelfType(Vector2(mPosition.x - hw,
      mPosition.y - hh), hw, hh, min, mObjects.get_allocator());
    mLeaves[SE] = new SelfType(Vector2(mPosition.x + hw,
      mPosition.y - hh), hw, hh, min, mObjects.get_allocator());
    mLeaves[NW] = new SelfType(Vector2(mPosition.x - hw,
      mPosition.y + hh), hw, hh, min, mObjects.get_allocator());
    mLeaves[NE] = new SelfType(Vector2(mPosition.x + hw,
      mPosition.y + hh), hw, hh, min, mObjects.get_allocator());
  }
}

} // xr

#endif  //XR_QUADTREE_HPP
