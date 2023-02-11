#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include <spdlog/spdlog.h>
// doctest comments
// 'function' : must be 'attribute' - see issue #182
DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4007)
int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%H:%M:%S.%e] [%^%L%$] [%P:%t] [%s:%#] [%!] %v");
  return doctest::Context(argc, argv).run();
}
DOCTEST_MSVC_SUPPRESS_WARNING_POP