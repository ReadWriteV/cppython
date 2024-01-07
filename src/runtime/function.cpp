#include "runtime/function.hpp"
#include "code/code_object.hpp"
#include "object/dict.hpp"
#include "object/integer.hpp"
#include "object/list.hpp"
#include "object/string.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/static_value.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <print>

using namespace cppython;

function_klass::function_klass() {
  add_super(object_klass::get_instance());
  set_name("function");
  auto type_obj = std::make_shared<type>();
  type_obj->set_own_klass(this);
}

native_function_klass::native_function_klass() {
  add_super(function_klass::get_instance());
  set_name("native function");
  auto type_obj = std::make_shared<type>();
  type_obj->set_own_klass(this);
}

method_klass::method_klass() {
  add_super(function_klass::get_instance());
  set_name("method");
  auto type_obj = std::make_shared<type>();
  type_obj->set_own_klass(this);
}

std::string function_klass::to_string(std::shared_ptr<object> obj) {
  auto p = std::static_pointer_cast<function>(obj);
  assert(p && (p->get_klass() == this));
  return std::format("<function: {}>", p->get_func_name()->to_string());
}

function::function(std::shared_ptr<object> obj) {
  auto code_obj = std::static_pointer_cast<code_object>(obj);
  func_code = code_obj;
  func_name = func_code->name;
  flags = func_code->flags;

  set_klass(function_klass::get_instance());
}

function::function(native_function_t *native_func) : native_func{native_func} {
  set_klass(native_function_klass::get_instance());
}

std::shared_ptr<object>
cppython::len(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  return arg_0->len();
}

std::shared_ptr<object>
cppython::iter(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  return arg_0->len();
}

std::shared_ptr<object>
cppython::print(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {

  for (auto &e : *args) {
    std::print("{} ", e->to_string());
  }
  std::print("\n");
  return static_value::none_value;
}

std::shared_ptr<object>
cppython::type_of(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  return arg_0->get_klass()->get_type_object();
}

std::shared_ptr<object> cppython::isinstance(
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto x = args->at(0);
  auto y = args->at(1);

  assert(y && y->get_klass() == type_klass::get_instance());
  auto type_obj = std::static_pointer_cast<type>(y);

  auto k = x->get_klass();

  for (const auto &e : k->get_mro()->get_value()) {
    if (e == y) {
      return static_value::true_value;
    }
  }

  return static_value::false_value;
}

std::shared_ptr<object> cppython::build_class(
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0); // function
  auto arg_1 = args->at(1); // name
  auto supers_list = std::make_shared<list>(
      args->begin() + 2, args->end() - 1); // 2...-2 parent class

  auto arg_last = args->back(); // locals dict

  assert(arg_0 && arg_0->get_klass() == function_klass::get_instance());
  assert(arg_1 && arg_1->get_klass() == string_klass::get_instance());
  assert(arg_last && arg_last->get_klass() == dict_klass::get_instance());

  auto new_klass = new klass;

  auto constructor = std::static_pointer_cast<function>(arg_0);
  auto class_name = std::static_pointer_cast<string>(arg_1);
  auto klass_dict = std::static_pointer_cast<dict>(arg_last);

  new_klass->set_name(class_name->to_string());
  new_klass->set_super_list(supers_list);
  new_klass->set_dict(klass_dict);
  new_klass->order_supers();

  auto new_type = std::make_shared<type>();
  new_type->set_own_klass(new_klass);

  return new_type;
}

std::shared_ptr<object>
function::call(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  return (*native_func)(args);
}

bool method::is_function(std::shared_ptr<object> x) {
  auto k = x->get_klass();

  if (k == function_klass::get_instance()) {
    return true;
  }

  if (k->get_mro() == nullptr) {
    return false;
  }

  return std::ranges::any_of(k->get_mro()->get_value(), [](auto &&e) {
    return e == function_klass::get_instance()->get_type_object();
  });
}
