#ifndef XR_STRUCTUREDREADER_HPP
#define XR_STRUCTUREDREADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <type_traits>
#include <algorithm>
#include <string>
#include <memory>
#include <set>
#include <stdexcept>

namespace xr
{
namespace Structured
{

//==============================================================================
///@brief Defines options that may be specified upon registering a Propery on a
/// Reader.
enum Options
{
  REQUIRED = 0x1 ///< The property registered with this option must be defined, otherwise Process() throws std::runtime_error.
};

namespace detail
{

//==============================================================================
template <class In, class Out>
struct PropertyBase
{
  virtual ~PropertyBase() = default;

  virtual void Set(In const& in, Out& out) const = 0;
};

//==============================================================================
class ReaderCore
{
protected:
  // types
  struct Record
  {
    std::string mKey;
    void* mProperty;
    uint32_t mOptions;
    mutable Record const* mNext;

    bool operator<(Record const& other) const
    {
      return mKey < other.mKey;
    }
  };

  // structors
  ReaderCore() = default;
  ReaderCore(ReaderCore&&) = default;
  ReaderCore(ReaderCore const&) = delete;

  ReaderCore& operator=(ReaderCore&&) = default;
  ReaderCore& operator=(ReaderCore const&) = delete;

  // general
  void AddRecord(Record&& record);

  // data
  std::set<Record> mProperties;
  Record const* mFirstRecord = nullptr; // no ownership
  Record const* mLastRecord = nullptr;  // no ownership
};

template <typename T, class In>
using Interpreter = T(*)(In const&);

template <typename T>
using Value = std::conditional_t<sizeof(T) <= sizeof(uintptr_t), T, T&&>;

template <typename T, class Out>
using Member = T Out::*;

template <typename T, class Out>
using Setter = void (Out::*)(Value<T>);

} // detail

///@brief A property of type T, of an object of type Out, obtainable from an object
/// of type In, using an interpreter, and pointer to a data member or setter.
template <typename T, class In, class Out>
class Property: public detail::PropertyBase<In, Out>
{
public:
  // types
  using Interpreter = detail::Interpreter<T, In>;
  using Member = detail::Member<T, Out>;
  using Setter = detail::Setter<T, Out>;

  // structors
  Property(Interpreter reader, Member member);
  Property(Interpreter reader, Setter setter);
  ~Property() = default;

  // general
  void Set(In const& in, Out& out) const override;

private:
  enum Type
  {
    MEMBER,
    SETTER
  };

  Interpreter mReader;
  Type mType;
  union {
    Member mMember;
    Setter mSetter;
  };
};

///@brief Facilitates the creation of a Property given an interpreter and a pointer
/// to a data member.
template <class Out, typename T, class In>
Property<T, In, Out>* MakeProperty(detail::Interpreter<T, In> interpreter,
  detail::Member<T, Out> member)
{
  return new Property<T, In, Out>(interpreter, member);
}

///@brief Facilitates the creation of a Property given an interpreter and a setter.
template <class Out, typename T, class In>
Property<T, In, Out>* MakeProperty(detail::Interpreter<T, In> interpreter,
  detail::Setter<T, Out> setter)
{
  return new Property<T, In, Out>(interpreter, setter);
}

//==============================================================================
///@brief Reads data from a hierarchical structure of keys and values, into output
/// objects of type Out. The InTraits type is required to define two things: a Type
/// definition for the type of the input object, and a GetChild function, which, given
/// a parent object, retrieves a child object mapped to a string key. 
template <class InTraits, class Out_>
class Reader: protected detail::ReaderCore
{
public:
  // types
  using In = typename InTraits::Type;
  static_assert(std::is_same<decltype(InTraits::GetChild),
    In const*(In const&, std::string const&)>::value,
    "InTraits must define a GetChild() function");

  using Out = Out_;
  using SelfType = Reader<InTraits, Out>;

  // static
  ///@brief Interpreter function for reading Out objects using the singleton instance.
  static Out Read(In const& in);

  // structors
  Reader();
  Reader(SelfType&& other);
  Reader(SelfType const&) = delete;
  ~Reader();

  SelfType& operator=(SelfType&& other) = default;
  SelfType& operator=(SelfType const& other) = delete;

  // general
  ///@brief Registers a mapping of the given @a key to a pair of a @a reader function
  /// and a pointer to data @a member to assign the result of calling the reader.
  ///@note Attempting to register a key that already exists triggers a std::runtime_error.
  template <typename T>
  SelfType& Register(char const* key, Property<T, In, Out>* prop, uint32_t options = 0x0);

  ///@brief Processes @a in, trying to get each child mapped to one of the registered
  /// keys and passing it to the associated property, to populate @a out.
  ///@note Properties are queried in the order of their registration.
  ///@note If the property was registered as PropertyOptions::REQUIRED, @a in must have
  /// a child with the key of the property, otherwise an exception is thrown.
  void Process(In const& in, Out& out) const;

  ///@brief Sets this as the instance for Read() to use.
  void SetInstance()
  {
    sInstance = this;
  }

private:
  // static
  static SelfType const* sInstance;

  static detail::PropertyBase<In, Out>* Cast(void* p);
};

template <class InTraits, class Out>
Reader<InTraits, Out> const* Reader<InTraits, Out>::sInstance = nullptr;

//==============================================================================
// implementation
//==============================================================================
template<typename T, class In, class Out>
Property<T, In, Out>::Property(Interpreter reader, Member member)
: mReader(reader),
  mType(MEMBER),
  mMember(member)
{}

//==============================================================================
template<typename T, class In, class Out>
Property<T, In, Out>::Property(Interpreter reader, Setter setter)
: mReader(reader),
  mType(SETTER),
  mSetter(setter)
{}

//==============================================================================
template<typename T, class In, class Out>
void Property<T, In, Out>::Set(In const& in, Out& out) const
{
  switch (mType)
  {
  case MEMBER:
    out.*mMember = mReader(in);
    break;

  case SETTER:
    (out.*mSetter)(mReader(in));
    break;
  }
}

//==============================================================================
template<class InTraits, class Out_>
Out_ Reader<InTraits, Out_>::Read(In const & in)
{
  Out result;
  sInstance->Process(in, result);
  return result;
}

//==============================================================================
template<class InTraits, class Out>
Reader<InTraits, Out>::Reader()
{
  SetInstance();
}

//==============================================================================
template<class InTraits, class Out>
Reader<InTraits, Out>::Reader(SelfType && other)
: ReaderCore(std::move(other))
{
  SetInstance();
}

//==============================================================================
template <class InTraits, class Out>
Reader<InTraits, Out>::~Reader()
{
  for (auto& e : mProperties)
  {
    delete Cast(e.mProperty);
  }
}

//==============================================================================
template <class InTraits, class Out>
template <typename T>
inline
Reader<InTraits, Out>& Reader<InTraits, Out>::Register(char const* key,
  Property<T, In, Out>* property, uint32_t options)
{
  AddRecord(Record{ key, property, options });
  return *this;
}

//==============================================================================
template <class InTraits, class Out>
void Reader<InTraits, Out>::Process(In const& in, Out& out) const
{
  auto i = mFirstRecord;
  while (i != nullptr)
  {
    if (auto child = InTraits::GetChild(in, i->mKey))
    {
      Cast(i->mProperty)->Set(*child, out);
    }
    else if ((i->mOptions & REQUIRED) != 0)
    {
      throw std::runtime_error("Required key '" + i->mKey + "' missing.");
    }
    i = i->mNext;
  }
}

//==============================================================================
template <class InTraits, class Out>
inline
detail::PropertyBase<typename InTraits::Type, Out>* Reader<InTraits, Out>::Cast(void* p)
{
  return static_cast<detail::PropertyBase<In, Out>*>(p);
}

} // Structured
} // xr

#endif //XR_STRUCTUREDREADER_HPP
