//
// Nuclear Heart Games
// XRhodes
//
// Quadtree.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    28/07/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_QUADTREE_HPP
#define XR_QUADTREE_HPP

#include <list>
#include "RectObject.hpp"
#include "Queue.hpp"
#include "functors.hpp"

namespace XR
{

//==============================================================================
class QuadtreeCore: public RectObject
{
public:
  // types
  ///@brief A unary function that you pass to Quadtree::Process() and will be
  /// called with each collision candidate as arguments.
  typedef void(*UnaryCallback)(void*); 

  ///@brief A binary function that you pass to Quadtree::Process() and will be
  /// called with each collision candidate as arguments: the first one is the
  /// one in the Quadtree, the second one is the one you passed to Process().
  typedef void(*BinaryCallback)(void*, void*); 

  // static
  static int  GetMaxSubdivisions();
  static void SetMaxSubdivisions(int maxSubdivisions);

  static bool   CalculateCanSplit(float hw, float hh, float min);
  static float  CalculateMin(float hw, float hh);

  static const AABB&  GetCandidateBox();
  
  // structors
  QuadtreeCore();
  QuadtreeCore(const Vector2& pos, float hw, float hh);
  ~QuadtreeCore();
  
protected:
  // types
  enum  Leaf
  {
    SW,
    SE,
    NW,
    NE,
    kNumLeaves
  };
  
  // static
  static int  s_maxSubdivisions;
  
  static AABB           s_candidateBox;
  static void*          s_pCandidate;
  static UnaryCallback  s_pUnaryCallback;
  static BinaryCallback s_pBinaryCallback;

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
  void  Add(const AABB& box, void* p);
  void  Add(const Vector2& pos, void* p);
  
  void  Process(const AABB& box, UnaryCallback pUnary);
  void  Process(const Vector2& pos, UnaryCallback pUnary);
  void  ProcessAll(UnaryCallback pUnary);
  
  void  Process(const AABB& box, BinaryCallback pBinary, void *pObject);
  void  Process(const Vector2& pos, BinaryCallback pBinary, void *pObject);
  void  ProcessAll(BinaryCallback pUnary, void *pObject);
  
  void  Remove(void* p);
  
  void  Update(const AABB& box, void* p);
  
  void  Clear();
  
  void  Destroy();
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
  SelfType  *m_arpLeaves[kNumLeaves];
};

//==============================================================================
/*
  implementation
*/
//==============================================================================
///@return The maximum number of subdivisions for quadtrees created with no
/// minimum node size specified.
inline
int QuadtreeCore::GetMaxSubdivisions()
{
  return s_maxSubdivisions;
}

//==============================================================================
///@brief Calculates the minimum size of node (along either axis), based on
///@a hw and @a hh and @a s_divisions.
inline
float QuadtreeCore::CalculateMin(float hw, float hh)
{
  return float(static_cast<int>(floorf(hw + hh)) >> (s_maxSubdivisions + 1));
}

//==============================================================================
///@return  Whether a node of half-width @a hw and half-height @a hh can split,
/// i.e. half of its smaller dimension is greater than the @a min. False means
/// no child nodes will be created.
inline
bool  QuadtreeCore::CalculateCanSplit(float hw, float hh, float min)
{
  XR_ASSERT(Quadtree, hw > 0);
  XR_ASSERT(Quadtree, hh > 0);
  XR_ASSERT(Quadtree, min > 0);
  return XR::Min(hw, hh) > min;
}

//==============================================================================
///@return  The AABB that has been most recently passed for an Add, Process or
/// Remove operation.
inline
const AABB& QuadtreeCore::GetCandidateBox()
{
  return s_candidateBox;
}

//==============================================================================
template  <template <typename> class Alloc>
Quadtree<Alloc>::Quadtree(AllocType a)
: QuadtreeCore(),
  m_objects(a),
  m_arpLeaves()
{
  memset(m_arpLeaves, 0x00, sizeof(m_arpLeaves));
}

//==============================================================================
template  <template <typename> class Alloc>
Quadtree<Alloc>::Quadtree(const Vector2 &pos, float hw, float hh, float min,
  AllocType a)
: QuadtreeCore(pos, hw, hh),
  m_objects(a),
  m_arpLeaves()
{
  memset(m_arpLeaves, 0x00, sizeof(m_arpLeaves));
  _Create(min);
}

//==============================================================================
template  <template <typename> class Alloc>
Quadtree<Alloc>::~Quadtree()
{
  Destroy();
}

//==============================================================================
///@brief Adds the object @a p with the AABB @a box, to the Quadtree.
///@note  Each object should be added to the Quadtree no more than once. For
/// the sake of efficiency this is not being checked.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Add(const AABB& box, void* p)
{
  XR_ASSERT(Quadtree, p != 0);
  XR_ASSERT(Quadtree, box.left < box.right);
  XR_ASSERT(Quadtree, box.bottom < box.top);
  s_candidateBox = box;
  s_pCandidate = p;
  
  _AddRecursion();
}

//==============================================================================
///@brief Adds the point object @a p at position @a pos, to the Quadtree.
///@note  Each object should be added to the Quadtree no more than once. For
/// the sake of efficiency this is not being checked.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Add(const Vector2& pos, void* p)
{
  XR_ASSERT(Quadtree, p != 0);
  s_candidateBox.Import(pos.x, pos.y, .0f, .0f);
  s_pCandidate = p;
  
  _AddRecursion();
}

//==============================================================================
///@brief Checks the objects in the Quadtree against the AABB @a box. The
/// collision candidates are passed to the UnaryCallback @a pUnary.
///@note  You will need to check for actual collision.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const AABB& box, UnaryCallback pUnary)
{
  XR_ASSERT(Quadtree, pUnary != 0);
  XR_ASSERT(Quadtree, box.left <= box.right);
  XR_ASSERT(Quadtree, box.bottom <= box.top);
  s_candidateBox = box;
  s_pUnaryCallback = pUnary;
  
  _ProcessRecursionUnary();
}

//==============================================================================
///@brief Checks the objects in the Quadtree against the point @a pos. The
/// collision candidates are passed to the UnaryCallback @a pUnary.
///@note  You will need to check for actual collision.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const Vector2& pos, UnaryCallback pUnary)
{
  XR_ASSERT(Quadtree, pUnary != 0);
  s_candidateBox.Import(pos.x, pos.y, .0f, .0f);
  s_pUnaryCallback = pUnary;

  _ProcessRecursionUnary();
}

//==============================================================================
///@brief Passes all objects in the Quadtree to the UnaryCallback @a pUnary.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::ProcessAll(UnaryCallback pUnary)
{
  XR_ASSERT(Quadtree, pUnary != 0);
  s_pUnaryCallback = pUnary;

  _ProcessAllRecursionUnary();
}

//==============================================================================
///@brief Checks the objects in the Quadtree against the AABB @a box. The
/// collision candidates are passed to the BinaryCallback @a pBinary.
///@note  You will need to check for actual collision.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const AABB& box, BinaryCallback pBinary,
  void *pObject)
{
  XR_ASSERT(Quadtree, pBinary != 0);
  XR_ASSERT(Quadtree, box.left <= box.right);
  XR_ASSERT(Quadtree, box.bottom <= box.top);
  s_candidateBox = box;
  s_pCandidate = pObject;
  s_pBinaryCallback = pBinary;
  
  _ProcessRecursionBinary();
}

//==============================================================================
///@brief Checks the objects in the Quadtree against the point @a pos. The
/// collision candidates are passed to the BinaryCallback @a pBinary.
///@note  You will need to check for actual collision.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Process(const Vector2& pos, BinaryCallback pBinary,
  void *pObject)
{
  XR_ASSERT(Quadtree, pBinary != 0);
  s_candidateBox.Import(pos.x, pos.y, .0f, .0f);
  s_pCandidate = pObject;
  s_pBinaryCallback = pBinary;

  _ProcessRecursionBinary();
}

//==============================================================================
///@brief Passes all objects in the Quadtree to the BinaryCallback @a pBinary.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::ProcessAll(BinaryCallback pBinary, void *pObject)
{
  XR_ASSERT(Quadtree, pBinary != 0);
  s_pCandidate = pObject;
  s_pBinaryCallback = pBinary;

  _ProcessAllRecursionBinary();
}

//==============================================================================
///@brief Removes from the Quadtree the first object at the address pointed to
/// by @a p.
///@note  The assumption is made that the object has been inserted into the
/// Quadtree once at most.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Remove(void* p)
{
  XR_ASSERT(Quadtree, p != 0);
  s_pCandidate = p;
  
  _RemoveRecursion();
}

//==============================================================================
///@brief Clears the Quadtree of objects.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Clear()
{
  m_objects.clear();
  if (m_canSplit)
  {
    m_arpLeaves[NW]->Clear();
    m_arpLeaves[NE]->Clear();
    m_arpLeaves[SW]->Clear();
    m_arpLeaves[SE]->Clear();
  }
}

//==============================================================================
///@brief Convenience function to remove an object and reinsert it at using
/// @a box as its new boundaries.
template  <template <typename> class Alloc>
inline
void  Quadtree<Alloc>::Update(const AABB& box, void* p)
{
  Remove(p);
  Add(box, p);
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_AddRecursion()
{
  if (m_canSplit)
  {
    if (s_candidateBox.bottom >= m_position.y)
    {
      if (s_candidateBox.right < m_position.x)
      {
        m_arpLeaves[NW]->_AddRecursion();
        return;
      }
      else if (s_candidateBox.left >= m_position.x)
      {
        m_arpLeaves[NE]->_AddRecursion();
        return;
      }
    }
    else if (s_candidateBox.top < m_position.y)
    {
      if (s_candidateBox.right < m_position.x)
      {
        m_arpLeaves[SW]->_AddRecursion();
        return;
      }
      else if (s_candidateBox.left >= m_position.x)
      {
        m_arpLeaves[SE]->_AddRecursion();
        return;
      }
    }
  }

  m_objects.push_back(s_pCandidate);
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_ProcessRecursionUnary()
{
  // All objects touching more than one leaf are candidates to collision - 
  // these happen to reside in the node.
  std::for_each(m_objects.begin(), m_objects.end(), s_pUnaryCallback);

  // ...and process all objects in the child nodes that the box is touching
  if (m_canSplit)
  {
    bool  west(s_candidateBox.left < m_position.x);
    bool  east(s_candidateBox.right >= m_position.x);

    if (s_candidateBox.top >= m_position.y)
    {
      if (west)
      {
        m_arpLeaves[NW]->_ProcessRecursionUnary();
      }

      if (east)
      {
        m_arpLeaves[NE]->_ProcessRecursionUnary();
      }
    }

    if (s_candidateBox.bottom < m_position.y)
    {
      if (west)
      {
        m_arpLeaves[SW]->_ProcessRecursionUnary();
      }

      if (east)
      {
        m_arpLeaves[SE]->_ProcessRecursionUnary();
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
  std::for_each(m_objects.begin(), m_objects.end(), s_pUnaryCallback);

  // ...and collect all objects from the child nodes that the box is touching
  if (m_canSplit)
  {
    m_arpLeaves[NW]->_ProcessRecursionUnary();
    m_arpLeaves[NE]->_ProcessRecursionUnary();
    m_arpLeaves[SW]->_ProcessRecursionUnary();
    m_arpLeaves[SE]->_ProcessRecursionUnary();
  }
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_ProcessRecursionBinary()
{
  // All objects touching more than one leaf are candidates to collision - 
  // these happen to reside in the node.
  std::for_each(m_objects.begin(), m_objects.end(),
    XR::Bind2nd<BinaryCallback, void*>(s_pBinaryCallback, s_pCandidate));

  // ...and process all objects in the child nodes that the box is touching
  if (m_canSplit)
  {
    bool  west(s_candidateBox.left < m_position.x);
    bool  east(s_candidateBox.right >= m_position.x);

    if (s_candidateBox.top >= m_position.y)
    {
      if (west)
      {
        m_arpLeaves[NW]->_ProcessRecursionBinary();
      }

      if (east)
      {
        m_arpLeaves[NE]->_ProcessRecursionBinary();
      }
    }

    if (s_candidateBox.bottom < m_position.y)
    {
      if (west)
      {
        m_arpLeaves[SW]->_ProcessRecursionBinary();
      }

      if (east)
      {
        m_arpLeaves[SE]->_ProcessRecursionBinary();
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
    Bind2nd<BinaryCallback, void*>(s_pBinaryCallback, s_pCandidate));

  // ...and collect all objects from the child nodes that the box is touching
  if (m_canSplit)
  {
    m_arpLeaves[NW]->_ProcessRecursionBinary();
    m_arpLeaves[NE]->_ProcessRecursionBinary();
    m_arpLeaves[SW]->_ProcessRecursionBinary();
    m_arpLeaves[SE]->_ProcessRecursionBinary();
  }
}

//==============================================================================
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::_RemoveRecursion()
{
  typename QueueType::iterator iEnd(m_objects.end());
  typename QueueType::iterator iFind(std::find(m_objects.begin(), iEnd,
    s_pCandidate));
  if (iFind != iEnd)
  {
    m_objects.adopt(m_objects, iFind);  // smooth move
    m_objects.pop_front();
  }
  else if (m_canSplit)
  {
    m_arpLeaves[NW]->_RemoveRecursion();
    m_arpLeaves[NE]->_RemoveRecursion();
    m_arpLeaves[SW]->_RemoveRecursion();
    m_arpLeaves[SE]->_RemoveRecursion();
  }
}

//==============================================================================
///@brief Empties the list of objects and deletes all four leaves.
///@note  Does not reset position.
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::Destroy()
{
  m_halfWidth = .0f;
  m_halfHeight = .0f;
  m_objects.clear();
  m_canSplit = false;
  
  for (int i = 0; i < kNumLeaves; ++i)
  {
    delete m_arpLeaves[i];
  }

  memset(m_arpLeaves, 0x00, sizeof(m_arpLeaves));
}

//==============================================================================
///@brief Creates the Quadtree at the given starting position @a pos and half-
/// size (@a hw, @a hh), subdividing while any of the nodes' dimensions is >
/// @a min or splitting down to @a s_divisions levels if @a min is .0f.
template  <template <typename> class Alloc>
void  Quadtree<Alloc>::Create(const Vector2 &pos, float hw, float hh, float min)
{
  XR_ASSERT(Quadtree, hw > .0f);
  XR_ASSERT(Quadtree, hh > .0f);
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
  
  float hw(m_halfWidth * .5f);
  float hh(m_halfHeight * .5f);
  m_canSplit = CalculateCanSplit(hw, hh, min);

  if (m_canSplit)
  {
    m_arpLeaves[NW] = new Quadtree(Vector2(m_position.x - hw,
      m_position.y + hh), hw, hh, min, m_objects.get_allocator());
    m_arpLeaves[NE] = new Quadtree(Vector2(m_position.x + hw,
      m_position.y + hh), hw, hh, min, m_objects.get_allocator());
    m_arpLeaves[SW] = new Quadtree(Vector2(m_position.x - hw,
      m_position.y - hh), hw, hh, min, m_objects.get_allocator());
    m_arpLeaves[SE] = new Quadtree(Vector2(m_position.x + hw,
      m_position.y - hh), hw, hh, min, m_objects.get_allocator());
  }
}

} // XR

#endif  //XR_QUADTREE_HPP
