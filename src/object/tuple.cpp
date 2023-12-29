#include "object/tuple.hpp"
#include "memory/oop_closure.hpp"
#include "object/integer.hpp"
#include "object/string.hpp"
#include "runtime/static_value.hpp"

using namespace cppython;

tuple_klass::tuple_klass() {
  set_name("tuple");
  (new type{})->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::string tuple_klass::to_string(object *obj) {
  auto tuple_obj = obj->as<tuple>();

  std::string result;

  result += "(";

  auto fmt_str = [](object *obj) {
    if (obj->is<string>()) {
      return "'" + obj->to_string() + "'";
    } else {
      return obj->to_string();
    }
  };

  if (!tuple_obj->empty()) {
    result += fmt_str(tuple_obj->at(0));
  }

  for (size_t i{1}; i < tuple_obj->size(); ++i) {
    result += ", ";
    result += fmt_str(tuple_obj->at(i));
  }

  result += ")";
  return result;
}

object *tuple_klass::subscr(object *x, object *y) {
  auto tuple_obj = x->as<tuple>();
  auto index_obj = y->as<integer>();
  return tuple_obj->at(index_obj->get_value());
}

void tuple_klass::oops_do(oop_closure *closure, object *obj) {
  auto tuple_obj = obj->as<tuple>();
  closure->do_vector(tuple_obj->data_address());
}

size_t tuple_klass::size() const { return sizeof(tuple); }
