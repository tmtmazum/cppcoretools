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

  unique_file(unique_file&& other)
  {
    operator=(std::move(other));
  }

  unique_file(unique_file const&) = delete;

  unique_file& operator=(unique_file const&) = delete;

  ~unique_file();

  auto handle() const noexcept { return m_file; }

private:
  FILE* m_file;
};

class scoped_redirect
{
public:
  static auto& stack()
  {
    static auto paths = std::invoke([]
    {
      std::vector<unique_file> paths;
      paths.emplace_back(stdout);
      return paths;
    });
    return paths;
  }

  explicit scoped_redirect(path const& p)
  {
    stack().emplace_back(p, "a");
  }

  explicit scoped_redirect(unique_file f)
  {
    stack().emplace_back(std::move(f));
  }

  ~scoped_redirect()
  {
    stack().pop_back();
  }
};
  
unique_file::~unique_file()
{
  if (!m_file) { return; }

  for (auto const& item : scoped_redirect::stack())
  {
    if (item.handle() == m_file)
    {
      return;
    }
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
  auto const r = print(std::forward<Args>(args)...);
  printf("\n");
  return r;
}

class scoped_failure_handler
{
public:
  using failure_handler_t = std::function<void(char const* op)>;

  static auto& stack()
  {
    static auto handlers = std::invoke([]
    {
      std::vector<failure_handler_t> handlers;
      handlers.emplace_back([](auto const s)
      {
        println("'%s' failed", s);
      });
      return handlers;
    });
    return handlers;
  }

  explicit scoped_failure_handler(failure_handler_t f)
  {
    stack().emplace_back(std::move(f));
  }

  ~scoped_failure_handler()
  {
    stack().pop_back();
  }
};

void check(bool value, char const* op_name)
{
  if (!(value))
  {
    scoped_failure_handler::stack().back()(op_name);
  }
}

} // namespace cct

#define CCT_CHECK(op) ::cct::check(op, #op)
