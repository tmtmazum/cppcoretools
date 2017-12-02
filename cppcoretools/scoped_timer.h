#pragma once

#include "print.h"
#include <chrono>

namespace cct
{

template <typename T>
auto num_hours(T t) { return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::hours>(t).count()); }

template <typename T>
auto num_minutes(T t) { return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::minutes>(t).count()); }

template <typename T>
auto num_seconds(T t) { return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(t).count()); }

template <typename T>
auto num_milliseconds(T t) { return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(t).count()); }

template <typename T>
auto num_microseconds(T t) { return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(t).count()); }

template <typename T>
auto num_nanoseconds(T t) { return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(t).count()); }

template <typename Duration>
std::string to_string(Duration dur)
{
  using namespace std::string_literals;

  auto h = num_hours(dur);
  auto m = num_minutes(dur) - num_hours(dur)*60;
  auto s = num_seconds(dur) - num_minutes(dur) * 60;
  auto ms = num_milliseconds(dur) - num_seconds(dur) * 1000;
  auto us = num_microseconds(dur) - num_milliseconds(dur) * 1000;
  auto ns = num_nanoseconds(dur) - num_microseconds(dur) * 1000;

  return std::string{ "" }
    + (h ? std::to_string(h) + " hr(s) " : ""s)
    + (m ? std::to_string(h) + " min(s) " : ""s)
    + (s ? std::to_string(s) + " s " : ""s)
    + (ms ? std::to_string(ms) + " ms " : ""s)
    + (us ? std::to_string(us) + " us " : ""s)
    + (ns ? std::to_string(ns) + " ns" : ""s);
}

class scoped_timer
{
public:
  using clock_t = std::chrono::steady_clock;

  using time_handler = std::function<void(std::chrono::nanoseconds const&, std::string const&)>;

  static time_handler default_time_handler()
  {
    return [](auto duration, auto op_name)
    { 
      println("Operation '%s' took %s", op_name.c_str(), to_string(duration).c_str());
    };
  }

  scoped_timer(std::string operation_name, time_handler handler = default_time_handler())
    : m_id{std::move(operation_name)}
    , m_handler{std::move(handler)}
  {}

  ~scoped_timer()
  {
    m_handler(clock_t::now() - m_start, m_id);
  }

private:
  time_handler        m_handler;
  std::string         m_id;
  clock_t::time_point m_start = clock_t::now();
};

} // namespace cct
