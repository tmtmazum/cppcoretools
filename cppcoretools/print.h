#pragma once

#include <cstdio>
#include <vector>
#include <string>
#include <functional>

namespace cct
{

using path = std::string;

class unique_file
{
public:
  explicit unique_file(FILE* fi) : m_file{ fi } {}

  unique_file(path filename, char const* mode)
    : unique_file{fopen(filename.c_str(), mode)}
  {}

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

  auto handle() const noexcept { return m_file; }

private:
  FILE* m_file;
};

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
    : scope_stack{ unique_file{p, "a"} }
  {}

  static auto default_value() { return stdout; }
};
  
unique_file::~unique_file()
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

void check(bool value, char const* op_name)
{
  if (!!(value)) { return; }
  scoped_failure_handler::stack().back()(op_name);
}

} // namespace cct

#define CCT_CHECK(op) ::cct::check(op, #op)
