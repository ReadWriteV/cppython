#include "runtime/function.hpp"
#include "code/code_object.hpp"
#include "memory/oop_closure.hpp"
#include "object/dict.hpp"
#include "object/integer.hpp"
#include "object/list.hpp"
#include "object/string.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/static_value.hpp"

#include <algorithm>
#include <format>
#include <print>

using namespace cppython;

function_klass::function_klass() {
  add_super(object_klass::get_instance());
  set_name("function");
  auto type_obj = new type{};
  type_obj->set_own_klass(this);
}

std::string function_klass::to_string(object *obj) {
  auto p = obj->as<function>();
  return std::format("<function: {}>", p->get_func_name()->to_string());
}

void function_klass::oops_do(oop_closure *f, object *obj) {
  auto func_obj = obj->as<function>();

  f->do_oop(reinterpret_cast<object *&>(func_obj->get_func_code()));
  f->do_oop(func_obj->get_func_name());
  f->do_oop(reinterpret_cast<object *&>(func_obj->get_globals()));
  f->do_vector(func_obj->get_default_args());
  f->do_oop(reinterpret_cast<object *&>(func_obj->get_closure()));
}

size_t function_klass::size() const { return sizeof(function); }

function::function(object *obj) {
  auto code_obj = obj->as<code_object>();
  func_code = code_obj;
  func_name = func_code->name;
  flags = func_code->flags;

  set_klass(function_klass::get_instance());
}

object *cppython::len(vector<object *> *args) {
  auto arg_0 = args->at(0);
  return arg_0->len();
}

object *cppython::print(vector<object *> *args) {

  for (size_t i{0}; i < args->size(); i++) {
    std::print("{} ", args->at(i)->to_string());
  }
  std::print("\n");
  return static_value::none_value;
}

object *cppython::type_of(vector<object *> *args) {
  auto arg_0 = args->at(0);
  return arg_0->get_klass()->get_type_object();
}

object *cppython::isinstance(vector<object *> *args) {
  auto x = args->at(0);
  auto y = args->at(1);

  auto type_obj = y->as<type>();

  auto k = x->get_klass();

  for (auto e : *(k->get_mro()->get_value())) {
    if (e == y) {
      return static_value::true_value;
    }
  }

  return static_value::false_value;
}

object *cppython::builtin_super(vector<object *> *args) { return nullptr; }

object *cppython::build_class(vector<object *> *args) {
  auto arg_0 = args->at(0); // function
  auto arg_1 = args->at(1); // name
  auto supers_list = new list{};

  // 2...-2 parent class
  for (size_t i{2}; i < args->size() - 1; i++) {
    supers_list->append(args->at(i));
  }

  auto arg_last = args->back(); // locals dict

  auto new_klass = new klass;

  auto constructor = arg_0->as<function>();
  auto class_name = arg_1->as<string>();
  auto klass_dict = arg_last->as<dict>();

  new_klass->set_name(class_name->to_string());
  new_klass->set_super_list(supers_list);
  new_klass->set_dict(klass_dict);
  new_klass->order_supers();

  auto new_type = new type{};
  new_type->set_own_klass(new_klass);

  return new_type;
}

object *cppython::sysgc(vector<object *> *args) {
  static_value::gc();
  return static_value::none_value;
}

native_function_klass::native_function_klass() {
  add_super(function_klass::get_instance());
  set_name("native function");
  auto type_obj = new type{};
  type_obj->set_own_klass(this);
}

void native_function_klass::oops_do(oop_closure *f, object *obj) {
  // do nothing
  // only overwrite warning message in Klass
}

size_t native_function_klass::size() const { return sizeof(native_function); }

native_function::native_function(native_function_t *native_func)
    : native_func{native_func} {
  set_klass(native_function_klass::get_instance());
}

object *native_function::call(vector<object *> *args) {
  return (*native_func)(args);
}

method_klass::method_klass() {
  add_super(function_klass::get_instance());
  set_name("method");
  auto type_obj = new type{};
  type_obj->set_own_klass(this);
}

void method_klass::oops_do(oop_closure *f, object *obj) {
  auto method_obj = obj->as<method>();

  f->do_oop(reinterpret_cast<object *&>(method_obj->get_owner()));
  f->do_oop(reinterpret_cast<object *&>(method_obj->get_func()));
}

size_t method_klass::size() const { return sizeof(method); }

bool method::is_function(object *x) {
  auto k = x->get_klass();

  if (k == function_klass::get_instance()) {
    return true;
  }

  if (k->get_mro() == nullptr) {
    return false;
  }

  for (size_t i{0}; i < k->get_mro()->size(); i++) {
    if (k->get_mro()->at(i) ==
        function_klass::get_instance()->get_type_object()) {
      return true;
    }
  }

  return false;
}
