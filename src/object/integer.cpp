#include "object/integer.hpp"
#include "memory/oop_closure.hpp"
#include "object/dict.hpp"
#include "runtime/static_value.hpp"
#include "utils/vector.hpp"

#include <cassert>
#include <compare>
#include <functional>

using namespace cppython;

integer_klass *integer_klass::instance = nullptr;

integer_klass *integer_klass::get_instance() {
  if (instance == nullptr) {
    instance = new integer_klass;
  }
  return instance;
}

void integer_klass::initialize() {

  set_name("int");
  set_dict(new dict{});
  (new type{})->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::string integer_klass::to_string(object *obj) {
  auto p = obj->as<integer>();
  return std::to_string(p->get_value());
}

template <typename PredicateOperation>
  requires std::predicate<PredicateOperation, int, int>
object *integer_klass::binary_predicate(object *x, object *y,
                                        PredicateOperation pred) {
  auto p = x->as<integer>();

  // check y is integer or not
  if (!y->is<integer>()) {
    return static_value::get_bool_value(
        std::is_lt(klass::compare(x->get_klass(), y->get_klass())));
  }

  auto q = y->as<integer>();

  return static_value::get_bool_value(pred(p->get_value(), q->get_value()));
}

object *integer_klass::greater(object *x, object *y) {
  return binary_predicate(x, y, std::greater<int>{});
}

object *integer_klass::less(object *x, object *y) {
  return binary_predicate(x, y, std::less<int>{});
}

object *integer_klass::equal(object *x, object *y) {

  if (x->get_klass() != y->get_klass()) {
    return static_value::false_value;
  }

  return binary_predicate(x, y, std::equal_to<int>{});
}

object *integer_klass::not_equal(object *x, object *y) {
  if (x->get_klass() != y->get_klass()) {
    return static_value::true_value;
  }

  return binary_predicate(x, y, std::not_equal_to<int>{});
}

object *integer_klass::ge(object *x, object *y) {
  return binary_predicate(x, y, std::greater_equal<int>{});
}

object *integer_klass::le(object *x, object *y) {
  return binary_predicate(x, y, std::less_equal<int>{});
}

template <typename BinaryOperation>
  requires std::invocable<BinaryOperation, int, int>
object *integer_klass::binary_op(object *x, object *y, BinaryOperation op) {
  auto p = x->as<integer>();
  auto q = y->as<integer>();

  return new integer{op(p->get_value(), q->get_value())};
}

object *integer_klass::add(object *x, object *y) {
  return binary_op(x, y, std::plus<int>{});
}

object *integer_klass::sub(object *x, object *y) {
  return binary_op(x, y, std::minus<int>{});
}

object *integer_klass::mul(object *x, object *y) {
  return binary_op(x, y, std::multiplies<int>{});
}

object *integer_klass::div(object *x, object *y) {
  return binary_op(x, y, std::divides<int>{});
}

object *integer_klass::mod(object *x, object *y) {
  return binary_op(x, y, std::modulus<int>{});
}

object *integer_klass::allocate_instance(object *obj_type,
                                         vector<object *> *args) {
  if (!args || args->size() == 0) {
    return new integer{0};
  } else {
    return nullptr;
  }
}

void integer_klass::oops_do(oop_closure *closure, object *obj) {
  // do nothing
  // only overwrite warning message in Klass
}

size_t integer_klass::size() const { return sizeof(integer); }
