#include "runtime/static_value.hpp"
#include "code/code_object.hpp"
#include "memory/heap.hpp"
#include "memory/oop_closure.hpp"
#include "object/dict.hpp"
#include "object/float.hpp"
#include "object/integer.hpp"
#include "object/list.hpp"
#include "object/object.hpp"
#include "object/string.hpp"
#include "runtime/function.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/module.hpp"
#include "static_value.hpp"
#include "utils/vector.hpp"

using namespace cppython;

void static_value::create() {
  klasses = new vector<klass *>{};

  true_value = new string{"True"};
  false_value = new string{"False"};
  none_value = new string{"None"};

  auto obj_klass = object_klass::get_instance();
  auto ty_klass = type_klass::get_instance();

  auto type_obj = new type{};
  type_obj->set_own_klass(ty_klass);

  auto obj_obj = new type{};
  obj_obj->set_own_klass(obj_klass);

  ty_klass->add_super(obj_klass);
  // obj_klass->add_super(nullptr);

  integer_klass::get_instance()->initialize();
  float_klass::get_instance()->initialize();
  string_klass::get_instance()->initialize();
  list_klass::get_instance()->initialize();
  dict_klass::get_instance()->initialize();
  module_klass::get_instance()->initialize();

  ty_klass->set_dict(new dict{});
  obj_klass->set_dict(new dict{});

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

object *static_value::get_bool_value(bool v) {
  return v ? true_value : false_value;
}

void static_value::add_klass(klass *k) { klasses->push_back(k); }

void static_value::gc() { heap::get_instance()->gc(); }

void *static_value::allocate(size_t size) {
  return heap::get_instance()->allocate(size);
}

void *cppython::static_value::allocate_meta(size_t size) {
  return heap::get_instance()->allocate_meta(size);
}

void cppython::static_value::oops_do(oop_closure *closure) {
  closure->do_oop(true_value);
  closure->do_oop(false_value);
  closure->do_oop(none_value);

  closure->do_oop(reinterpret_cast<object *&>(main_code));
  closure->do_vector(klasses);

  closure->do_oop(stop_iteration);
}

bool value_equal::operator()(object *lhs, object *rhs) const {
  return lhs->equal(rhs) == static_value::true_value;
}

bool value_less::operator()(object *lhs, object *rhs) const {
  return lhs->less(rhs) == static_value::true_value;
}
