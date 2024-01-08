#include "inc/cppython.hpp"
#include "object/float.hpp"
#include "object/integer.hpp"

#include <cmath>
#include <vector>

using namespace cppython;

// issue: static value in cppython.exe and math.dll

double get_double(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto x = args->at(0);
  double y = 0;
  if (x->get_klass() == integer_klass::get_instance()) {
    y = std::static_pointer_cast<integer>(x)->get_value();
  } else if (x->get_klass() == float_klass::get_instance()) {
    y = std::static_pointer_cast<float_num>(x)->get_value();
  }
  return y;
}

std::shared_ptr<object>
math_sqrt(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  double x = get_double(args);
  return std::make_shared<float_num>(std::sqrt(x));
}

std::shared_ptr<object>
math_sin(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  double x = get_double(args);
  return std::make_shared<float_num>(std::sin(x));
}

ext_method math_methods[] = {{.method_name = "sin",
                              .method_func = math_sin,
                              .method_info = 0,
                              .method_doc = "sin(x)"},
                             {.method_name = "sqrt",
                              .method_func = math_sqrt,
                              .method_info = 0,
                              .method_doc = "square root of x"},
                             {.method_func = nullptr, .method_info = 0}};

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) ext_method *init_libmath() { return math_methods; }

#ifdef __cplusplus
}
#endif
