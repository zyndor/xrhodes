//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_FLOATBUFFER_HPP
#define XR_FLOATBUFFER_HPP
#include <XR/Vector3.hpp>
#include <XR/Vector2.hpp>
#include <XR/Color.hpp>
#include <functional>
#include <vector>

namespace XR
{

//=============================================================================
class FloatBuffer
{
public:
  // static
  static const size_t kSizeRest = -1;

  ///@brief Adaptation allows you to have a FloatBuffer object pointing to
  /// the data of another FloatBuffer, e.g. to simplify indexing of a relevant
  /// part.
  /// Adapting instances must not outlive the owner of their data. This means
  /// that adaptation locks down the original buffer for size changes,
  /// including destruction and assignment.
  /// An adapting instance can be changed into an owning instance, but not
  /// vice versa - this guarantees to prevent self-adapting.
  static FloatBuffer Adapt(FloatBuffer& other, size_t offset, size_t size = kSizeRest);

  // structors
  FloatBuffer();
  FloatBuffer(size_t elemSize, size_t numElems);
  FloatBuffer(FloatBuffer const& other);
  ~FloatBuffer();

  // general
  ///@brief Sets the buffer size via setting the size of elements and the number
  /// of elements. This instance will be the owner of this newly sized buffer.
  ///@note This is not synonymous with std::vector<>::resize() in that the
  /// data is released, the pre-existing elements are lost.
  void SetSize(size_t elemSize, size_t numElems);

  template <class T>
  void SetSize(size_t numElems)
  {
    SetSize(sizeof(T), numElems);
  }

  ///@brief Gets the size of the individual elements in this buffer.
  size_t GetElementSize() const { return m_elemSize; }

  ///@brief Gets the number or elements in this buffer.
  size_t GetNumElements() const { return m_numElems; }

  ///@brief Moves @a other's data to this, releasing previous data. @a other must
  /// not have any dependants.
  void Move(FloatBuffer&& other);

  ///@brief It creates a copy of the contents of this and owns it.
  ///@return Whether anything was done - false if this already owns its buffer.
  bool Own();

  ///@brief Sets @a numElems elements of @a pData in this, starting at
  /// @a offset (as T).
  ///@note This is indexed as an array of T.
  template <typename T>
  void Set(size_t numElems, T const* pData, size_t offset = 0)
  {
    XR_ASSERT(FloatBuffer, sizeof(T) == m_elemSize);
    XR_ASSERT(FloatBuffer, m_parData != nullptr);
    XR_ASSERT(FloatBuffer, offset + numElems <= m_numElems);
    T const* pEnd = pData + numElems;
    while(pData != pEnd)
    {
      SetInternal(offset * m_elemSize, *pData);
      ++offset;
      ++pData;
    }
  }

  ///@brief Sets the @a i-th element in this as T, to a@ value. The
  /// buffer must contain @a m_elemSize items of type T, and sizeof(T).
  ///@note This is indexed as an array of T.
  template <typename T>
  void Set(size_t i, T const& value)
  {
    XR_ASSERT(FloatBuffer, sizeof(T) == m_elemSize * sizeof(float));
    XR_ASSERT(FloatBuffer, m_parData != nullptr);
    XR_ASSERT(FloatBuffer, i < m_numElems);
    SetInternal(i * m_elemSize, value);
  }

  ///@brief Gets the underlying raw buffer of floats.
  ///@note Does not transfer ownership.
  float* GetRaw()
  {
    return m_parData;
  }

  ///@brief Gets the underlying raw buffer of floats.
  ///@note Does not transfer ownership.
  float const* GetRaw() const
  {
    return m_parData;
  }

  ///@brief Gets the underlying buffer as an array of const T.
  template <typename T>
  T const*  Get() const
  {
    XR_ASSERT(FloatBuffer, sizeof(T) == m_elemSize * sizeof(float));
    const T* p;
    GetInternal(p);
    return p;
  }

  ///@brief Gets the underlying buffer as an array of T.
  template <typename T>
  T*  Get()
  {
    XR_ASSERT(FloatBuffer, sizeof(T) == m_elemSize * sizeof(float));
    FloatBuffer const* ct = this;
    return const_cast<T*>(ct->Get<T>());
  }

  ///@brief Indexes into the buffer as an array of const T. 
  template <typename T>
  T const& Get(size_t i) const
  {
    return Get<T>()[i];
  }

  ///@brief Indexes into the buffer as an array of T. 
  template <typename T>
  T& Get(size_t i)
  {
    return Get<T>()[i];
  }

  ///@brief Passes each element in the buffer, cast to T, to the function @a fn.
  template <typename T>
  void ForEach(std::function<void(T&)> fn)
  {
    XR_ASSERT(FloatBuffer, m_elemSize == sizeof(T));
    auto parElems = Get<T>();
    std::for_each(parElems, parElems + m_numElems, fn);
  }

  // operator overloads
  FloatBuffer& operator=(FloatBuffer const& rhs)
  {
    FloatBuffer temp(rhs);
    ReleaseData();
    Move(std::move(temp));

    return *this;
  }

private:
  // static
  static float* AllocateBuffer(size_t elemSize, size_t numElems);

  // data members
  size_t              m_elemSize;
  size_t              m_numElems;
  float*              m_parData;  // ownership if m_pAdapted == nullptr

  FloatBuffer const*  m_pAdapted; // no ownership
  mutable size_t      m_numDependents;

  // internal
  FloatBuffer(FloatBuffer& other, size_t offset, size_t size = kSizeRest);

  void CopyData(FloatBuffer const& other);
  void ReleaseData();
  void DetachFromOwner();
  size_t ResolveSize(size_t offset, size_t size) const;

  void SetInternal(size_t i, Vector2 const& v)
  {
    std::copy(v.arData, v.arData + Vector2::kNumComponents, m_parData + i);
  }

  void SetInternal(size_t i, Vector3 const& v)
  {
    std::copy(v.arData, v.arData + Vector3::kNumComponents, m_parData + i);
  }

  void SetInternal(size_t i, Color const& c)
  {
    std::copy(c.arData, c.arData + Color::kNumComponents, m_parData + i);
  }

  void GetInternal(Vector2 const*& v) const
  {
    v = reinterpret_cast<Vector2 const*>(m_parData);
  }

  void GetInternal(Vector3 const*& v) const
  {
    v = reinterpret_cast<Vector3 const*>(m_parData);
  }

  void GetInternal(Color const*& c) const
  {
    c = reinterpret_cast<Color const*>(m_parData);
  }
};

}

#endif //XR_FLOATBUFFER_HPP