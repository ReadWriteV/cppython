#pragma once

#include "object/float.hpp"
#include "object/integer.hpp"
#include "runtime/function.hpp"
#include "utils/vector.hpp"

#include <string_view>

namespace cppython {

struct ext_method {
  std::string_view method_name;
  native_function::native_function_t *method_func;
  int method_info;
  std::string_view method_doc;
};

using init_func = ext_method *();

} // namespace cppython
