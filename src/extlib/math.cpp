#include "inc/cppython.hpp"

#include <cmath>

using namespace cppython;

// issue: static value in cppython.exe and math.dll

double get_double(vector<object *> *args) {
  auto x = args->at(0);
  double y = 0;
  if (x->is<integer>()) {
    y = x->as<integer>()->get_value();
  } else if (x->is<float_num>()) {
    y = x->as<float_num>()->get_value();
  }
  return y;
}

object *math_sqrt(vector<object *> *args) {
  double x = get_double(args);
  return new float_num{std::sqrt(x)};
}

object *math_sin(vector<object *> *args) {
  double x = get_double(args);
  return new float_num{std::sin(x)};
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
