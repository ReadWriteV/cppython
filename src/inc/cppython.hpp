#pragma once

#include "object/float.hpp"
#include "object/integer.hpp"
#include "runtime/function.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace cppython {

using native_function_t = std::shared_ptr<object>(
    std::shared_ptr<std::vector<std::shared_ptr<object>>>);

struct ext_method {
  std::string_view method_name;
  native_function_t *method_func;
  int method_info;
  std::string_view method_doc;
};

using init_func = ext_method *();

} // namespace cppython
