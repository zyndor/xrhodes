#ifndef XR_QUADTREE_HPP
#define XR_QUADTREE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "RectObject.hpp"
#include "AABB.hpp"
#include "xr/functors.hpp"
#include "xr/Queue.hpp"
#include <list>

#if defined  _MSC_VER
#pragma warning(push)
#pragma warning(disable:4351)
#endif  //_MSC_VER

namespace xr
{

//==============================================================================
class QuadtreeCore: public RectObject
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

  // types
  ///@brief A unary function that you pass to Quadtree::Process() and will be
  /// called with each collision candidate as arguments.
  typedef void(*UnaryCallback)(void* systemData);

  ///@brief A binary function that you pass to Quadtree::Process() and will be
  /// called with each collision candidate as arguments: the first one is the
  /// one in the Quadtree, the second one is the one you passed to Process().
  typedef void(*BinaryCallback)(void* systemData, void* userData);

  // static
  ///@return The maximum number of subdivisions for quadtrees created with no
  /// minimum node size specified.
  static int  GetMaxSubdivisions();

  ///@brief Sets the maximum number of subdivisions for quadtrees created with
  /// no minimum node size specified.
  static void SetMaxSubdivisions(int maxSubdivisions);

  ///@return  Whether a node of half-width @a hw and half-height @a hh can
  /// split, i.e. half of its smaller dimension is greater than the @a min.
  /// False means no child nodes will be created.
  static bool   CalculateCanSplit(float hw, float hh, float min);

  ///@brief Calculates the minimum size of node (along either axis), based on
  /// @a hw, @a hh and @a s_divisions.
  static float  CalculateMin(float hw, float hh);

  ///@return  The AABB that has been most recently passed for an Add, Process or
  /// Remove operation.
  static const AABB&  GetCandidateBox();

  // structors
  QuadtreeCore();
  QuadtreeCore(const Vector2& pos, float hw, float hh);
  ~QuadtreeCore();

  // general
  ///@return  If the Quadtree can split, i.e. if child nodes are created.
  bool  CanSplit() const;

protected:
  // static
  static int  s_maxSubdivisions;

  static AABB           s_hitBox;
  static void*          s_callbackData;
  static UnaryCallback  s_unaryCallback;
  static BinaryCallback s_binaryCallback;

  // data
  bool  m_canSplit;
};

//==============================================================================
///@brief Rectangle Quadtree class that pre-creates its structure and stores
/// and processes items as void*, based on their AABB's. You can specify the
/// minimum size of a node in the @a min parameter of the constructor. If .0f,
/// this will be calculated based on @a s_divisions.
template  <template <typename> class Alloc = std::allocator>
class Quadtree: public QuadtreeCore
{
public:
  // types
  typedef Alloc<void*>            AllocType;
  typedef Queue<void*, AllocType> QueueType;
  typedef Quadtree<Alloc>         SelfType;

  // structors
  explicit Quadtree(AllocType a = AllocType());
  Quadtree(const Vector2& pos, float hw, float hh, float min = 0,
    AllocType a = AllocType());
  ~Quadtree();

  // general use
  SelfType* GetLeaf(Leaf l) const;

  ///@brief Adds the object @a p with the AABB @a box, to the Quadtree.
  void  Add(const AABB& box, void* object);

  ///@brief Adds the point object @a p at position @a pos, to the Quadtree.
  void  Add(const Vector2& pos, void* object);

  ///@brief Checks the objects in the Quadtree against the AABB @a box. The
  /// collision candidates are passed to the UnaryCallback @a cb.
  ///@note  You will need to check for actual collision.
  void  Process(const AABB& box, UnaryCallback cb);

  ///@brief Checks the objects in the Quadtree against the point @a pos. The
  /// collision candidates are passed to the UnaryCallback @a cb.
  ///@note  You will need to check for actual collision.
  void  Process(const Vector2& pos, UnaryCallback cb);

  ///@brief Passes all objects in the Quadtree to the UnaryCallback @a cb.
  void  ProcessAll(UnaryCallback cb);

  ///@brief Checks the objects in the Quadtree against the AABB @a box. The
  /// collision candidates are passed to the BinaryCallback @a cb.
  ///@note  You will need to check for actual collision.
  void  Process(const AABB& box, BinaryCallback cb, void* userData);

  ///@brief Checks the objects in the Quadtree against the point @a pos. The
  /// collision candidates are passed to the BinaryCallback @a cb.
  ///@note  You will need to check for actual collision.
  void  Process(const Vector2& pos, BinaryCallback cb, void* userData);

  ///@brief Passes all objects in the Quadtree to the BinaryCallback @a cb.
  void  ProcessAll(BinaryCallback cb, void* userData);

  ///@brief Removes from the Quadtree the first object at the address pointed
  /// to by @a p.
  ///@note  The assumption is made that the object has been inserted into the
  /// Quadtree once at most.
  void  Remove(void* object);

  ///@brief Convenience function to remove an object and reinsert it at using
  /// @a box as its new boundaries.
  void  Update(const AABB& box, void* object);

  ///@brief Clears the Quadtree of objects.
  void  Clear();

  ///@brief Empties the list of objects and deletes all four leaves.
  ///@note  Does not reset position.
  void  Destroy();

  ///@brief Creates the Quadtree at the given starting position @a pos and half-
  /// size (@a hw, @a hh), subdividing while any of the nodes' dimensions is >
  /// @a min or splitting down to @a s_divisions levels if @a min is .0f.
  void  Create(const Vector2 &pos, float hw, float hh, float min);

private:
  // internal use
  void  _Create(float min);
  void  _AddRecursion();
  void  _ProcessRecursionUnary();
  void  _ProcessAllRecursionUnary();
  void  _ProcessRecursionBinary();
  void  _ProcessAllRecursionBinary();
  void  _RemoveRecursion();

  // data
  QueueType m_objects;
  SelfType  *m_leaves[kNumLeaves];
};

//==============================================================================
// implementation
//==============================================================================
inline
int QuadtreeCore::GetMaxSubdivisions()
{
  return s_maxSubdivisions;
}

//==============================================================================
inline
float QuadtreeCore::CalculateMin(float hw, float hh)
{
  return float(static_cast<int>(floorf(hw + hh)) >> (s_maxSubdivisions + 1));
}

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
const AABB& QuadtreeCore::GetCandidateBox()
{
  return s_hitBox;
}

//==============================================================================
inline
bool  QuadtreeCore::CanSplit() const
{
  return m_canSplit;
}

//==============================================================================
template  <template <typename> class Alloc>
Quadtree<Alloc>::Quadtree(AllocType a)
: QuadtreeCore(),
  m_objects(a),
  m_leaves()
{
  memset(m_leaves, 0x00, sizeof(m_leaves));
}

//==============================================================================
template  <template <typename> class Alloc>
Quadtree<Alloc>::Quadtree(const Vector2 &pos, float hw, float hh, float min,
  AllocType a)
: QuadtreeCore(pos, hw, hh),
  m_objects(a),
  m_leaves()
{
  memset(m_leaves, 0x00, sizeof(m_leaves));
  _Create(min);
}

//==============================================================================
template  <template <typename> class Alloc>
Quadtree<Alloc>::~Quadtree()
{
  Destroy();
}

//==============================================================================
template  <template <typename> class Alloc>
Quadtree<Alloc>*  Quadtree<Alloc>::GetLeaf(Leaf l) const
{
  XR_ASSERT(Quadtree, l < kNumLeaves);
  return m_leaves[l];
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Add(const AABB& box, void* object)
{
  XR_ASSERT(Quadtree, object != nullptr);
  XR_ASSERT(Quadtree, box.left <= box.right);
  XR_ASSERT(Quadtree, box.bottom <= box.top);
  s_hitBox = box;
  s_callbackData = object;

  _AddRecursion();
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Add(const Vector2& pos, void* object)
{
  XR_ASSERT(Quadtree, object != nullptr);
  s_hitBox.Import(pos.x, pos.y, .0f, .0f);
  s_callbackData = object;

  _AddRecursion();
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const AABB& box, UnaryCallback cb)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  XR_ASSERT(Quadtree, box.left <= box.right);
  XR_ASSERT(Quadtree, box.bottom <= box.top);
  s_hitBox = box;
  s_unaryCallback = cb;

  _ProcessRecursionUnary();
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const Vector2& pos, UnaryCallback cb)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  s_hitBox.Import(pos.x, pos.y, .0f, .0f);
  s_unaryCallback = cb;

  _ProcessRecursionUnary();
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::ProcessAll(UnaryCallback cb)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  s_unaryCallback = cb;

  _ProcessAllRecursionUnary();
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const AABB& box, BinaryCallback cb, void* userData)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  XR_ASSERT(Quadtree, box.left <= box.right);
  XR_ASSERT(Quadtree, box.bottom <= box.top);
  s_hitBox = box;
  s_callbackData = userData;
  s_binaryCallback = cb;

  _ProcessRecursionBinary();
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const Vector2& pos, BinaryCallback cb,
  void* userData)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  s_hitBox.Import(pos.x, pos.y, .0f, .0f);
  s_callbackData = userData;
  s_binaryCallback = cb;

  _ProcessRecursionBinary();
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::ProcessAll(BinaryCallback cb, void* userData)
{
  XR_ASSERT(Quadtree, cb != nullptr);
  s_callbackData = userData;
  s_binaryCallback = cb;

  _ProcessAllRecursionBinary();
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Remove(void* object)
{
  XR_ASSERT(Quadtree, object != nullptr);
  s_callbackData = object;

  _RemoveRecursion();
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Update(const AABB& box, void* object)
{
  Remove(object);
  Add(box, object);
}

//==============================================================================
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Clear()
{
  m_objects.clear();
  if (m_canSplit)
  {
    m_leaves[NW]->Clear();
    m_leaves[NE]->Clear();
    m_leaves[SW]->Clear();
    m_leaves[SE]->Clear();
  }
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_AddRecursion()
{
  if (m_canSplit)
  {
    if (s_hitBox.bottom >= m_position.y)
    {
      if (s_hitBox.right < m_position.x)
      {
        m_leaves[NW]->_AddRecursion();
        return;
      }
      else if (s_hitBox.left >= m_position.x)
      {
        m_leaves[NE]->_AddRecursion();
        return;
      }
    }
    else if (s_hitBox.top < m_position.y)
    {
      if (s_hitBox.right < m_position.x)
      {
        m_leaves[SW]->_AddRecursion();
        return;
      }
      else if (s_hitBox.left >= m_position.x)
      {
        m_leaves[SE]->_AddRecursion();
        return;
      }
    }
  }

  m_objects.push_back(s_callbackData);
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_ProcessRecursionUnary()
{
  // All objects touching more than one leaf are candidates to collision -
  // these happen to reside in the node.
  std::for_each(m_objects.begin(), m_objects.end(), s_unaryCallback);

  // ...and process all objects in the child nodes that the box is touching
  if (m_canSplit)
  {
    bool  west(s_hitBox.left < m_position.x);
    bool  east(s_hitBox.right >= m_position.x);

    if (s_hitBox.top >= m_position.y)
    {
      if (west)
      {
        m_leaves[NW]->_ProcessRecursionUnary();
      }

      if (east)
      {
        m_leaves[NE]->_ProcessRecursionUnary();
      }
    }

    if (s_hitBox.bottom < m_position.y)
    {
      if (west)
      {
        m_leaves[SW]->_ProcessRecursionUnary();
      }

      if (east)
      {
        m_leaves[SE]->_ProcessRecursionUnary();
      }
    }
  }
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_ProcessAllRecursionUnary()
{
  // All objects touching all leaves are candidates to collision - these
  // happen to reside in the node.
  std::for_each(m_objects.begin(), m_objects.end(), s_unaryCallback);

  // ...and process all objects in the child nodes that the box is touching
  if (m_canSplit)
  {
    m_leaves[NW]->_ProcessRecursionUnary();
    m_leaves[NE]->_ProcessRecursionUnary();
    m_leaves[SW]->_ProcessRecursionUnary();
    m_leaves[SE]->_ProcessRecursionUnary();
  }
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_ProcessRecursionBinary()
{
  // All objects touching more than one leaf are candidates to collision -
  // these happen to reside in the node.
  std::for_each(m_objects.begin(), m_objects.end(),
    Bind2nd<BinaryCallback, void*>(s_binaryCallback, s_callbackData));

  // ...and process all objects in the child nodes that the box is touching
  if (m_canSplit)
  {
    bool  west(s_hitBox.left < m_position.x);
    bool  east(s_hitBox.right >= m_position.x);

    if (s_hitBox.top >= m_position.y)
    {
      if (west)
      {
        m_leaves[NW]->_ProcessRecursionBinary();
      }

      if (east)
      {
        m_leaves[NE]->_ProcessRecursionBinary();
      }
    }

    if (s_hitBox.bottom < m_position.y)
    {
      if (west)
      {
        m_leaves[SW]->_ProcessRecursionBinary();
      }

      if (east)
      {
        m_leaves[SE]->_ProcessRecursionBinary();
      }
    }
  }
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_ProcessAllRecursionBinary()
{
  // All objects touching all leaves are candidates to collision - these
  // happen to reside in the node.
  std::for_each(m_objects.begin(), m_objects.end(),
    Bind2nd<BinaryCallback, void*>(s_binaryCallback, s_callbackData));

  // ...and process all objects from the child nodes that the box is touching
  if (m_canSplit)
  {
    m_leaves[NW]->_ProcessRecursionBinary();
    m_leaves[NE]->_ProcessRecursionBinary();
    m_leaves[SW]->_ProcessRecursionBinary();
    m_leaves[SE]->_ProcessRecursionBinary();
  }
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_RemoveRecursion()
{
  typename QueueType::iterator iEnd(m_objects.end());
  typename QueueType::iterator iFind(std::find(m_objects.begin(), iEnd, s_callbackData));
  if (iFind != iEnd)
  {
    m_objects.adopt(m_objects, iFind);  // smooth move
    m_objects.pop_front();
  }
  else if (m_canSplit)
  {
    m_leaves[NW]->_RemoveRecursion();
    m_leaves[NE]->_RemoveRecursion();
    m_leaves[SW]->_RemoveRecursion();
    m_leaves[SE]->_RemoveRecursion();
  }
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::Destroy()
{
  m_halfWidth = .0f;
  m_halfHeight = .0f;
  m_objects.clear();
  m_canSplit = false;

  for (int i = 0; i < kNumLeaves; ++i)
  {
    delete m_leaves[i];
  }

  memset(m_leaves, 0x00, sizeof(m_leaves));
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::Create(const Vector2 &pos, float hw, float hh, float min)
{
  XR_ASSERT(Quadtree, hw >= .0f);
  XR_ASSERT(Quadtree, hh >= .0f);
  XR_ASSERT(Quadtree, min >= .0f);
  Destroy();

  m_position = pos;
  m_halfWidth = hw;
  m_halfHeight = hh;

  _Create(min);
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_Create(float min)
{
  if (min == .0f)
  {
    min = CalculateMin(m_halfWidth, m_halfHeight);
  }

  if(min > .0f)
  {
    float hw = m_halfWidth * .5f;
    float hh = m_halfHeight * .5f;
    m_canSplit = CalculateCanSplit(hw, hh, min);

    if (m_canSplit)
    {
      m_leaves[NW] = new Quadtree(Vector2(m_position.x - hw,
        m_position.y + hh), hw, hh, min, m_objects.get_allocator());
      m_leaves[NE] = new Quadtree(Vector2(m_position.x + hw,
        m_position.y + hh), hw, hh, min, m_objects.get_allocator());
      m_leaves[SW] = new Quadtree(Vector2(m_position.x - hw,
        m_position.y - hh), hw, hh, min, m_objects.get_allocator());
      m_leaves[SE] = new Quadtree(Vector2(m_position.x + hw,
        m_position.y - hh), hw, hh, min, m_objects.get_allocator());
    }
  }
}

} // XR

#if defined  _MSC_VER
#pragma warning(pop)
#endif  //_MSC_VER

#endif  //XR_QUADTREE_HPP
