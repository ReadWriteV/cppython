#include "object/object.hpp"
#include "memory/oop_closure.hpp"
#include "object/dict.hpp"
#include "runtime/function.hpp"
#include "runtime/static_value.hpp"
#include "runtime/string_table.hpp"

#include <algorithm>
#include <format>

using namespace cppython;

std::string object::to_string() { return get_klass()->to_string(this); }

object *object::add(object *x) { return get_klass()->add(this, x); }

object *object::sub(object *x) { return get_klass()->sub(this, x); }

object *object::mul(object *x) { return get_klass()->mul(this, x); }

object *object::div(object *x) { return get_klass()->div(this, x); }

object *object::mod(object *x) { return get_klass()->mod(this, x); }

object *object::greater(object *x) { return get_klass()->greater(this, x); }

object *object::less(object *x) { return get_klass()->less(this, x); }

object *object::equal(object *x) { return get_klass()->equal(this, x); }

object *object::not_equal(object *x) { return get_klass()->not_equal(this, x); }

object *object::ge(object *x) { return get_klass()->ge(this, x); }

object *object::le(object *x) { return get_klass()->le(this, x); }

object *object::getattr(object *x) { return get_klass()->getattr(this, x); }

object *cppython::object::setattr(object *x, object *y) {
  return get_klass()->setattr(this, x, y);
}

object *object::subscr(object *x) { return get_klass()->subscr(this, x); }

void object::store_subscr(object *x, object *y) {
  get_klass()->store_subscr(this, x, y);
}

void object::del_subscr(object *x) { get_klass()->del_subscr(this, x); }

object *object::contains(object *x) { return get_klass()->contains(this, x); }

object *object::iter() { return get_klass()->iter(this); }

object *object::len() { return get_klass()->len(this); }

void *object::operator new(size_t size) { return static_value::allocate(size); }

void object::oops_do(oop_closure *closure) {
  // object does not know who to visit, klass knows
  closure->do_oop(reinterpret_cast<object *&>(obj_dict));
  get_klass()->oops_do(closure, this);
}

size_t object::size() const { return get_klass()->size(); }

char *object::new_address() {
  if ((mark_word & 0x1) == 0x1) {
    return reinterpret_cast<char *>(mark_word & (static_cast<intmax_t>(-8)));
  }
  return nullptr;
}

void object::set_new_address(char *addr) {
  if (!addr) {
    return;
  }
  mark_word = reinterpret_cast<intmax_t>(addr) | 0x1;
}

std::string type_klass::to_string(object *obj) {
  auto type_obj = obj->as<type>();
  return std::format("<class '{}'>", type_obj->get_type_name());
}

object *type_klass::setattr(object *x, object *y, object *z) {
  auto type_obj = x->as<type>();
  type_obj->get_own_klass()->get_dict()->insert(y, z);
  return static_value::none_value;
}

void cppython::type_klass::oops_do(oop_closure *f, object *obj) {
  // do nothing since HiTypeObject refers no oop.
}

size_t cppython::type_klass::size() const { return sizeof(type); }

std::string type::get_type_name() const {
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

  result += own_klass->get_name()->to_string();
  return result;
}
