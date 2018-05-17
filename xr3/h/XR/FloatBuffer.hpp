#ifndef XR_FLOATBUFFER_HPP
#define XR_FLOATBUFFER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "XR/Vector3.hpp"
#include "XR/Vector2.hpp"
#include "XR/Color.hpp"
#include <functional>
#include <vector>

namespace XR
{

//=============================================================================
///@brief A buffer of floats, usually for renderer data. Provides convenience
/// functionality for buffer ownership management, indexing and casting to the
/// relevant vector type.
class FloatBuffer
{
public:
  // static
  static const uint32_t kSizeRest = -1;

  ///@brief Adaptation allows you to have a FloatBuffer object pointing to
  /// the data of another FloatBuffer, e.g. to simplify indexing of a relevant
  /// part.
  /// Adapting instances must not outlive the owner of their data. This means
  /// that adaptation locks down the original buffer for size changes,
  /// including destruction and assignment.
  /// An adapting instance can be changed into an owning instance, but not
  /// vice versa - this guarantees to prevent self-adapting.
  ///@note The @a offset and @a size parameters are numbers of objects of
  /// the vectorial type that @a other represents.
  static FloatBuffer Adapt(FloatBuffer& other, uint32_t offset, uint32_t size = kSizeRest);

  // structors
  FloatBuffer();
  FloatBuffer(uint32_t elemSize, uint32_t numElems, float* parBuffer = nullptr);
  FloatBuffer(FloatBuffer const& other);
  ~FloatBuffer();

  // general
  ///@brief Sets the buffer to the given size. The caller may optionally
  /// provide a buffer of their allocation, otherwise this instance will
  /// allocate a buffer and will own it.
  void SetBuffer(uint32_t elemSize, uint32_t numElems, float* parBuffer = nullptr);

  template <class T>
  void SetBuffer(uint32_t numElems, T* parBuffer = nullptr);

  ///@brief Gets the size of the individual elements in this buffer as the
  /// number of floats.
  uint32_t GetElementSizeFloats() const { return m_elemSizeFloats; }

  ///@brief Gets the size of the individual elements in this buffer, in bytes.
  uint32_t GetElementSizeBytes() const { return GetElementSizeFloats() * sizeof(float); }

  ///@brief Gets the number or elements in this buffer.
  uint32_t GetNumElements() const { return m_numElems; }

  ///@brief Moves @a other's data to this, releasing previous data. @a other must
  /// not have any dependants.
  void Move(FloatBuffer&& other);

  ///@brief It creates a copy of the contents of this and owns it.
  ///@return Whether anything was done - false if this already owns its buffer.
  bool Own();

  ///@brief Releases - and if this had ownership, deallocates - the the buffer
  /// of floats this was holding. Releases reference to adapted buffers.
  void ReleaseData();

  ///@brief Sets the first @a numElems elements of @a data in this, starting
  /// at @a offset (as T).
  ///@note This is indexed as an array of T.
  template <typename T>
  void Set(uint32_t numElems, T const* data, uint32_t offset = 0);

  ///@brief Sets the @a i-th element in this as T, to a@ value. The
  /// buffer must contain GetNumElements() items of type T, and sizeof(T)
  /// must be equal to GetElementSizeBytes().
  ///@note This is indexed as an array of T.
  template <typename T>
  void Set(uint32_t i, T const& value);

  ///@brief Gets the underlying raw buffer of floats.
  ///@note Does not transfer ownership.
  float* GetRaw();

  ///@brief Gets the underlying raw buffer of floats.
  ///@note Does not transfer ownership.
  float const* GetRaw() const;

  ///@brief Gets the underlying buffer as an array of const T.
  template <typename T>
  T const*  Get() const;

  ///@brief Gets the underlying buffer as an array of T.
  template <typename T>
  T*  Get();

  ///@brief Indexes into the buffer as an array of const T.
  template <typename T>
  T const& Get(uint32_t i) const;

  ///@brief Indexes into the buffer as an array of T.
  template <typename T>
  T& Get(uint32_t i);

  ///@brief Passes each element in the buffer, cast to T, to the function @a fn.
  template <typename T>
  void ForEach(std::function<void(T&)> fn);

  // operator overloads
  FloatBuffer& operator=(FloatBuffer const& rhs);

private:
  // static
  static float* AllocateBuffer(uint32_t elemSize, uint32_t numElems);

  // data members
  uint32_t            m_elemSizeFloats;
  uint32_t            m_numElems;
  float*              m_data;  // ownership if m_adapted == nullptr && m_ownData

  FloatBuffer const*  m_adapted; // no ownership
  bool                m_ownData : 1;
  mutable uint32_t    m_numDependents : 31;

  // internal
  FloatBuffer(FloatBuffer& other, uint32_t offset, uint32_t size = kSizeRest);

  void CopyData(FloatBuffer const& other);
  void DetachFromOwner();
  uint32_t ResolveSize(uint32_t offset, uint32_t size) const;
};

//=============================================================================
// inline implementation
//=============================================================================
template<class T>
inline
void FloatBuffer::SetBuffer(uint32_t numElems, T * parBuffer)
{
  static_assert((sizeof(T) % sizeof(float)) == 0, "Type T must have a size which is an integer multiple of sizeof(float).");
  SetBuffer(sizeof(T), numElems, reinterpret_cast<float*>(parBuffer));
}

//=============================================================================
template<typename T>
inline
void FloatBuffer::Set(uint32_t numElems, T const * data, uint32_t offset)
{
  XR_ASSERT(FloatBuffer, sizeof(T) == GetElementSizeBytes());
  XR_ASSERT(FloatBuffer, m_data != nullptr);
  XR_ASSERT(FloatBuffer, offset + numElems <= m_numElems);
  T const* dataEnd = data + numElems;
  auto write = Get<T>() + offset;
  while (data != dataEnd)
  {
    *write = *data;
    ++write;
    ++data;
  }
}

//=============================================================================
template<typename T>
inline
void FloatBuffer::Set(uint32_t i, T const & value)
{
  XR_ASSERT(FloatBuffer, sizeof(T) == GetElementSizeBytes());
  XR_ASSERT(FloatBuffer, m_data != nullptr);
  XR_ASSERT(FloatBuffer, i < m_numElems);
  Get<T>(i) = value;
}

//=============================================================================
inline
float * FloatBuffer::GetRaw()
{
  return m_data;
}

//=============================================================================
inline
float const * FloatBuffer::GetRaw() const
{
  return m_data;
}

//=============================================================================
template<typename T>
inline
T * FloatBuffer::Get()
{
  XR_ASSERT(FloatBuffer, sizeof(T) == GetElementSizeBytes());
  T* p = reinterpret_cast<T*>(m_data);
  return p;
}

//=============================================================================
template<typename T>
inline
T const * FloatBuffer::Get() const
{
  XR_ASSERT(FloatBuffer, sizeof(T) == GetElementSizeBytes());
  const T* p = reinterpret_cast<T const*>(m_data);
  return p;
}

//=============================================================================
template<typename T>
inline
T const & FloatBuffer::Get(uint32_t i) const
{
  return Get<T>()[i];
}

//=============================================================================
template<typename T>
inline
T & FloatBuffer::Get(uint32_t i)
{
  return Get<T>()[i];
}

//=============================================================================
template<typename T>
inline
void FloatBuffer::ForEach(std::function<void(T&)> fn)
{
  XR_ASSERT(FloatBuffer, sizeof(T) == GetElementSizeBytes());
  auto parElems = Get<T>();
  std::for_each(parElems, parElems + m_numElems, fn);
}

} // XR

#endif //XR_FLOATBUFFER_HPP
