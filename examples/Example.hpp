#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <string>

class Example
{
public:
  static Example* GetFirst();

  explicit Example(char const* name);
  virtual ~Example();

  virtual void Init() =0;
  virtual void Update() =0;
  virtual void Render() =0;
  virtual void Shutdown() =0;

  std::string const& GetName() const;
  Example* GetNext() const;
  Example* GetPrevious() const;

private:
  static void Register(Example* e);

  std::string m_name;
  Example* m_previous;
  Example* m_next;
};

#endif //EXAMPLE_HPP
