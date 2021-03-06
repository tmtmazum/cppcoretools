#include <cppcoretools/print.h>
#include <cppcoretools/scoped_timer.h>

void print_to_file()
{
  cct::scoped_redirect r{ "my_file.txt" };
  cct::println("Hurray!!");
}

void check_scoped_timer()
{
  using namespace std::chrono_literals;

  cct::scoped_timer timer("check_scoped_timer");

  cct::scoped_failure_handler on_failure{[](auto const op_str)
  {
    cct::println("Test '%s' failed", op_str);
    __debugbreak();
    std::abort();
  }};

  CCT_CHECK(cct::to_string(1s) == "1s ");
  timer = cct::scoped_timer{ "another scoped" };
  CCT_CHECK(cct::to_string(1s + 1ms) == "1s 1ms ");
  CCT_CHECK(cct::to_string(14s + 3ms + 5ns) == "14s 3ms 5ns");
  CCT_CHECK(cct::to_string(2h + 3ms + 6ns) == "2hr(s) 3ms 6ns");
}

int main()
{
  print_to_file();
  check_scoped_timer();
  cct::print("All tests %s passed.\n", "hurra");

  __debugbreak();
  return 0;
}
