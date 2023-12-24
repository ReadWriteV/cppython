#include "object/object.hpp"
#include "object/dict.hpp"
#include "object/klass.hpp"
#include "runtime/function.hpp"
#include "runtime/static_value.hpp"
#include "runtime/string_table.hpp"

#include <algorithm>
#include <cassert>

using namespace cppython;

void object::set_klass(klass *x) {
  assert(x != nullptr);
  klass_ = x;
}

std::string object::to_string() {
  return get_klass()->to_string(shared_from_this());
}

std::shared_ptr<object> object::add(std::shared_ptr<object> x) {
  return get_klass()->add(shared_from_this(), x);
}

std::shared_ptr<object> object::sub(std::shared_ptr<object> x) {
  return get_klass()->sub(shared_from_this(), x);
}

std::shared_ptr<object> object::mul(std::shared_ptr<object> x) {
  return get_klass()->mul(shared_from_this(), x);
}

std::shared_ptr<object> object::div(std::shared_ptr<object> x) {
  return get_klass()->div(shared_from_this(), x);
}

std::shared_ptr<object> object::mod(std::shared_ptr<object> x) {
  return get_klass()->mod(shared_from_this(), x);
}

std::shared_ptr<object> object::greater(std::shared_ptr<object> x) {
  return get_klass()->greater(shared_from_this(), x);
}

std::shared_ptr<object> object::less(std::shared_ptr<object> x) {
  return get_klass()->less(shared_from_this(), x);
}

std::shared_ptr<object> object::equal(std::shared_ptr<object> x) {
  return get_klass()->equal(shared_from_this(), x);
}

std::shared_ptr<object> object::not_equal(std::shared_ptr<object> x) {
  return get_klass()->not_equal(shared_from_this(), x);
}

std::shared_ptr<object> object::ge(std::shared_ptr<object> x) {
  return get_klass()->ge(shared_from_this(), x);
}

std::shared_ptr<object> object::le(std::shared_ptr<object> x) {
  return get_klass()->le(shared_from_this(), x);
}

std::shared_ptr<object> object::getattr(std::shared_ptr<object> x) {
  return get_klass()->getattr(shared_from_this(), x);
}

std::shared_ptr<object> cppython::object::setattr(std::shared_ptr<object> x,
                                                  std::shared_ptr<object> y) {
  return get_klass()->setattr(shared_from_this(), x, y);
}

std::shared_ptr<object> object::subscr(std::shared_ptr<object> x) {
  return get_klass()->subscr(shared_from_this(), x);
}

void object::store_subscr(std::shared_ptr<object> x,
                          std::shared_ptr<object> y) {
  get_klass()->store_subscr(shared_from_this(), x, y);
}

void object::del_subscr(std::shared_ptr<object> x) {
  get_klass()->del_subscr(shared_from_this(), x);
}

std::shared_ptr<object> object::contains(std::shared_ptr<object> x) {
  return get_klass()->contains(shared_from_this(), x);
}

std::shared_ptr<object> object::iter() {
  return get_klass()->iter(shared_from_this());
}

std::shared_ptr<object> object::len() {
  return get_klass()->len(shared_from_this());
}

std::string type_klass::to_string(std::shared_ptr<object> obj) {
  assert(obj->get_klass() == this);
  auto type_obj = std::static_pointer_cast<type>(obj);

  std::string result;
  result += "<class '" + type_obj->get_type_name() + "\'>";
  return result;
}

std::shared_ptr<object> type_klass::setattr(std::shared_ptr<object> x,
                                            std::shared_ptr<object> y,
                                            std::shared_ptr<object> z) {
  assert(x->get_klass() == this);
  auto type_obj = std::static_pointer_cast<type>(x);
  type_obj->get_own_klass()->get_dict()->insert(y, z);
  return static_value::none_value;
}

std::string type::get_type_name() {
  auto own_klass = get_own_klass();
  std::string result;

  auto attr_dict = own_klass->get_dict();
  if (attr_dict) {
    auto mod = attr_dict->at(string_table::get_instance()->mod_str);
    if (mod != static_value::none_value) {
      result += mod->to_string();
      result += ".";
    }
  }

  result += own_klass->get_name();
  return result;
}
