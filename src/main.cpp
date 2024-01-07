#include "code/pyc_parser.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/static_value.hpp"

#include <filesystem>
#include <print>

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::println("cppython need a parameter: filename");
    return 0;
  }

  std::filesystem::path file{argv[1]};

  if (!std::filesystem::exists(file)) {
    std::println("{} doesn't exist");
    return -1;
  }

  if (file.extension() != ".pyc") {
    std::println("cppython need a pyc file");
    return -1;
  }

  cppython::static_value::create();

  cppython::pyc_parser parser{argv[1]};
  auto code = parser.parse();

  cppython::interpreter::get_instance()->run(code);
  return 0;
}
