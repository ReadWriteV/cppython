#include "object/string.hpp"
#include "object/dict.hpp"
#include "object/integer.hpp"
#include "runtime/function.hpp"
#include "runtime/static_value.hpp"
#include "runtime/string_table.hpp"

#include <algorithm>
#include <cassert>
#include <print>

using namespace cppython;

void string_klass::initialize() {

  auto string_dict = std::make_shared<dict>();

  string_dict->insert(std::make_shared<string>("upper"),
                      std::make_shared<function>(string::string_upper));
  string_dict->insert(std::make_shared<string>("join"),
                      std::make_shared<function>(string::string_join));
  string_dict->insert(string_table::get_instance()->repr_str,
                      std::make_shared<function>(string::string_repr));
  set_dict(string_dict);

  set_name("str");
  std::make_shared<type>()->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::shared_ptr<string> string_klass::repr(std::shared_ptr<object> obj) {

  auto p = std::static_pointer_cast<string>(obj);

  assert(p && (p->get_klass() == this));

  return std::make_shared<string>(p->get_value());
}

std::shared_ptr<object> string_klass::add(std::shared_ptr<object> x,
                                          std::shared_ptr<object> y) {
  assert(x && (x->get_klass() == this));
  auto p = std::static_pointer_cast<string>(x);

  assert(y && (y->get_klass() == this));
  auto q = std::static_pointer_cast<string>(y);

  return std::make_shared<string>(p->get_value() + q->get_value());
}

std::shared_ptr<object> string_klass::equal(std::shared_ptr<object> x,
                                            std::shared_ptr<object> y) {
  if (x->get_klass() != y->get_klass()) {
    return static_value::false_value;
  }

  auto p = std::static_pointer_cast<string>(x);
  auto q = std::static_pointer_cast<string>(y);

  assert(p && (p->get_klass() == this));
  assert(q && (q->get_klass() == this));

  auto r = p->get_value() <=> q->get_value();
  return static_value::get_bool_value(std::is_eq(r));
}

std::shared_ptr<object> string_klass::less(std::shared_ptr<object> x,
                                           std::shared_ptr<object> y) {
  auto p = std::static_pointer_cast<string>(x);
  assert(p && (p->get_klass() == this));

  // check y is string or not
  if (y->get_klass() != this) {
    return static_value::get_bool_value(
        std::is_lt(klass::compare(x->get_klass(), y->get_klass())));
  }

  auto q = std::static_pointer_cast<string>(y);
  assert(q && (q->get_klass() == this));

  auto r = p->get_value() <=> q->get_value();
  return static_value::get_bool_value(std::is_lt(r));
}

std::shared_ptr<object> string_klass::subscr(std::shared_ptr<object> x,
                                             std::shared_ptr<object> y) {
  assert(x->get_klass() == this);
  assert(y->get_klass() == integer_klass::get_instance());

  auto string_obj = std::static_pointer_cast<string>(x);
  auto index_obj = std::static_pointer_cast<integer>(y);

  auto ch = string_obj->at(index_obj->get_value());

  return std::make_shared<string>(1, ch);
}

std::shared_ptr<object> string_klass::len(std::shared_ptr<object> x) {
  assert(x->get_klass() == this);
  auto string_obj = std::static_pointer_cast<string>(x);
  return std::make_shared<integer>(static_cast<int>(string_obj->size()));
}

std::shared_ptr<object> string_klass::allocate_instance(
    std::shared_ptr<object> obj_type,
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  if (!args || args->size() == 0) {
    return std::make_shared<string>("");
  } else {
    return nullptr;
  }
}

std::shared_ptr<string> string::join(std::shared_ptr<object> iterable) {
  auto iter = iterable->iter();
  auto obj = iter->next();

  if (obj == nullptr) {
    return std::make_shared<string>("");
  }
  auto str = std::static_pointer_cast<string>(obj);
  size_t total = str->size();
  while ((obj = iter->next()) != nullptr) {
    total += this->size();
    str = std::static_pointer_cast<string>(obj);
    total += str->size();
  }

  std::string result;
  result.resize(total);

  iter = iterable->iter();
  obj = iter->next();
  str = std::static_pointer_cast<string>(obj);

  auto i = std::copy(str->get_value().begin(), str->get_value().end(),
                     result.begin());

  while ((obj = iter->next()) != nullptr) {
    str = std::static_pointer_cast<string>(obj);
    i = std::copy(value.begin(), value.end(), i);
    i = std::copy(str->get_value().begin(), str->get_value().end(), i);
  }

  return std::make_shared<string>(std::move(result));
}

std::shared_ptr<object> string::string_upper(
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == string_klass::get_instance());

  auto str_obj = std::static_pointer_cast<string>(arg_0);

  auto length = str_obj->size();
  if (length <= 0) {
    return static_value::none_value;
  }

  std::string upper_str;
  upper_str.resize(length);

  std::transform(str_obj->get_value().begin(), str_obj->get_value().end(),
                 upper_str.begin(), ::toupper);

  return std::make_shared<string>(std::move(upper_str));
}

std::shared_ptr<object> string::string_join(
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == string_klass::get_instance());

  auto str_obj = std::static_pointer_cast<string>(arg_0);
  return str_obj->join(args->at(1));
}

std::shared_ptr<object> string::string_repr(
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == string_klass::get_instance());

  auto str_obj = std::static_pointer_cast<string>(arg_0);
  return str_obj->repr();
}