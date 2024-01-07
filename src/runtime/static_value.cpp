#include "runtime/static_value.hpp"
#include "object/dict.hpp"
#include "object/float.hpp"
#include "object/integer.hpp"
#include "object/list.hpp"
#include "object/object.hpp"
#include "object/string.hpp"
#include "runtime/function.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/module.hpp"

using namespace cppython;

void static_value::create() {
  true_value = std::make_shared<string>("True");
  false_value = std::make_shared<string>("False");
  none_value = std::make_shared<string>("None");

  auto obj_klass = object_klass::get_instance();
  auto ty_klass = type_klass::get_instance();

  auto type_obj = std::make_shared<type>();
  type_obj->set_own_klass(ty_klass);

  auto obj_obj = std::make_shared<type>();
  obj_obj->set_own_klass(obj_klass);

  ty_klass->add_super(obj_klass);
  // obj_klass->add_super(nullptr);

  integer_klass::get_instance()->initialize();
  float_klass::get_instance()->initialize();
  string_klass::get_instance()->initialize();
  list_klass::get_instance()->initialize();
  dict_klass::get_instance()->initialize();
  module_klass::get_instance()->initialize();

  ty_klass->set_dict(std::make_shared<dict>());
  obj_klass->set_dict(std::make_shared<dict>());

  ty_klass->set_name("type");
  obj_klass->set_name("object");

  integer_klass::get_instance()->order_supers();
  float_klass::get_instance()->order_supers();
  string_klass::get_instance()->order_supers();
  list_klass::get_instance()->order_supers();
  dict_klass::get_instance()->order_supers();
  ty_klass->order_supers();

  function_klass::get_instance()->order_supers();
  native_function_klass::get_instance()->order_supers();
  method_klass::get_instance()->order_supers();
  module_klass::get_instance()->order_supers();

  interpreter::get_instance()->initialize();
}

void static_value::destroy() {}

std::shared_ptr<object> static_value::get_bool_value(bool v) {
  return v ? true_value : false_value;
}

bool value_equal::operator()(const std::shared_ptr<object> &lhs,
                             const std::shared_ptr<object> &rhs) const {
  return lhs->equal(rhs) == static_value::true_value;
}

bool value_less::operator()(const std::shared_ptr<object> &lhs,
                            const std::shared_ptr<object> &rhs) const {
  return lhs->less(rhs) == static_value::true_value;
}
