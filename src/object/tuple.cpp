#include "object/tuple.hpp"
#include "object/integer.hpp"
#include "object/string.hpp"
#include "runtime/static_value.hpp"

#include <cassert>
#include <print>

using namespace cppython;

tuple_klass::tuple_klass() {
  set_name("tuple");
  std::make_shared<type>()->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::string tuple_klass::to_string(std::shared_ptr<object> obj) {
  assert(obj && (obj->get_klass() == this));
  auto tuple_obj = std::static_pointer_cast<tuple>(obj);

  std::string result;

  result += "(";

  auto fmt_str = [&tuple_obj](size_t idx) {
    if (tuple_obj->at(idx)->get_klass() == string_klass::get_instance()) {
      return "'" + tuple_obj->at(idx)->to_string() + "'";
    } else {
      return tuple_obj->at(idx)->to_string();
    }
  };

  if (!tuple_obj->empty()) {
    result += fmt_str(0);
  }

  for (size_t i{1}; i < tuple_obj->size(); ++i) {
    result += ", ";
    result += fmt_str(i);
  }

  result += ")";
  return result;
}

std::shared_ptr<object> tuple_klass::subscr(std::shared_ptr<object> x,
                                            std::shared_ptr<object> y) {
  assert(x->get_klass() == this);
  assert(y->get_klass() == integer_klass::get_instance());

  auto tuple_obj = std::static_pointer_cast<tuple>(x);
  auto index_obj = std::static_pointer_cast<integer>(y);

  return tuple_obj->at(index_obj->get_value());
}
