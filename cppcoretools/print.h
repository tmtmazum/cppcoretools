#pragma once

#include <cstdio>
#include <vector>
#include <string>
#include <functional>
#include "error_and.h"

namespace cct
{

using path = std::string;

class unique_file
{
public:
  explicit unique_file(FILE* fi) : m_file{ fi } {}

  auto& operator=(unique_file&& other)
  {
    m_file = other.m_file;
    other.m_file = nullptr;
    return *this;
  }

  unique_file(unique_file&& other) { operator=(std::move(other)); }

  unique_file(unique_file const&) = delete;

  unique_file& operator=(unique_file const&) = delete;

  ~unique_file();

// public member functions:
  auto puts(char const* str) { return ::fputs(str, m_file); }

  template <typename... Args>
  auto printf(Args&&... args) { return ::fprintf(m_file, std::forward<Args>(args)...); }

  auto gets(char* str, int num) { return ::fgets(str, num, m_file); }

  auto handle() const noexcept { return m_file; }

  auto getc() { return ::fgetc(m_file); }

  auto eof() { return ::feof(m_file); }

private:
  FILE* m_file;
};

inline auto make_unique_file(path filename, char const* mode)
{
    auto f = fopen(filename.c_str(), mode);
    if (!f)
    {
        return error_and<unique_file>{std::error_code{ errno, std::system_category() }, nullptr};
    }
    return error_and<unique_file>{std::error_code{ errno, std::system_category() }, f};
}

template <typename Elem, typename T, typename Container = std::vector<Elem>>
class scope_stack
{
public:
  static auto& stack()
  {
    static auto s = std::invoke([]
    {
      Container c;
      c.emplace_back(T::default_value());
      return c;
    });
    return s;
  }

  explicit scope_stack(Elem elem) { stack().emplace_back(std::move(elem)); }

  ~scope_stack() { stack().pop_back(); }
};

class scoped_redirect : public scope_stack<unique_file, scoped_redirect>
{
public:
  explicit scoped_redirect(unique_file f)
    : scope_stack{std::move(f)}
  {}

  explicit scoped_redirect(path const& p)
    : scope_stack{ make_unique_file(p, "a").value() }
  {}

  static auto default_value() { return stdout; }
};
  
inline unique_file::~unique_file()
{
  if (!m_file) { return; }

  for (auto const& item : scoped_redirect::stack())
  {
    if (item.handle() == m_file)
      { return; }
  }

  fclose(m_file); 
}

template <typename... Args>
auto print(Args&&... args)
{
  return fprintf(scoped_redirect::stack().back().handle(), std::forward<Args>(args)...);
}

template <typename... Args>
auto println(Args&&... args)
{
  auto const r = print(std::forward<Args>(args)...); printf("\n");
  return r;
}

using failure_handler_t = std::function<void(char const* op)>;

class scoped_failure_handler : public scope_stack<failure_handler_t, scoped_failure_handler>
{
public:
  explicit scoped_failure_handler(failure_handler_t f)
    : scope_stack{std::move(f)}
  {}

  static auto default_value()
  {
    return [](auto const s) { println("'%s' failed", s); };
  }
};

inline void check(bool value, char const* op_name)
{
  if (!!(value)) { return; }
  scoped_failure_handler::stack().back()(op_name);
}

} // namespace cct

#define CCT_CHECK(op) ::cct::check(op, #op)
