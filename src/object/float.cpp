#include "object/float.hpp"
#include "object/dict.hpp"
#include "object/integer.hpp"
#include "object/klass.hpp"
#include "object/string.hpp"
#include "runtime/static_value.hpp"
#include "utils/vector.hpp"

#include <functional>

using namespace cppython;

float_klass *float_klass::get_instance() {
  static float_klass instance;
  return &instance;
}

void float_klass::initialize() {
  set_dict(new dict());
  set_name("int");
  (new type{})->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::string float_klass::to_string(object *obj) {

  return std::to_string(obj->as<float_num>()->get_value());
}

object *float_klass::allocate_instance(object *callable,
                                       vector<object *> *args) {
  if (!args || args->size() == 0) {
    return new float_num{0};
  } else {
    return nullptr;
  }
}

size_t float_klass::size() const { return sizeof(float_num); }

void float_klass::oops_do(oop_closure *f, object *obj) {
  // do nothing
  // only overwrite warning message in Klass
}

template <typename PredicateOperation>
  requires std::predicate<PredicateOperation, double, double>
object *float_klass::binary_predicate(object *x, object *y,
                                      PredicateOperation pred) {
  auto p = x->as<float_num>();

  // check y is integer or not
  if (!y->is<float_num>()) {
    return static_value::get_bool_value(
        std::is_lt(klass::compare(x->get_klass(), y->get_klass())));
  }

  auto q = y->as<float_num>();

  return static_value::get_bool_value(pred(p->get_value(), q->get_value()));
}

object *float_klass::greater(object *x, object *y) {
  return binary_predicate(x, y, std::greater<double>{});
}

object *float_klass::less(object *x, object *y) {
  return binary_predicate(x, y, std::less<double>{});
}

object *float_klass::equal(object *x, object *y) {
  if (x->get_klass() != y->get_klass()) {
    return static_value::false_value;
  }
  return binary_predicate(x, y, std::equal_to<double>{});
}

object *float_klass::not_equal(object *x, object *y) {
  if (x->get_klass() != y->get_klass()) {
    return static_value::true_value;
  }
  return binary_predicate(x, y, std::not_equal_to<double>{});
}

object *float_klass::ge(object *x, object *y) {
  return binary_predicate(x, y, std::greater_equal<double>{});
}

object *float_klass::le(object *x, object *y) {
  return binary_predicate(x, y, std::less_equal<double>{});
}

template <typename BinaryOperation>
  requires std::invocable<BinaryOperation, double, double>
object *float_klass::binary_op(object *x, object *y, BinaryOperation op) {
  auto p = x->as<float_num>();
  auto q = y->as<float_num>();

  return new float_num{op(p->get_value(), q->get_value())};
}

object *float_klass::add(object *x, object *y) {
  return binary_op(x, y, std::plus<double>{});
}

object *float_klass::sub(object *x, object *y) {
  return binary_op(x, y, std::minus<double>{});
}

object *float_klass::mul(object *x, object *y) {
  return binary_op(x, y, std::multiplies<double>{});
}

object *float_klass::div(object *x, object *y) {
  return binary_op(x, y, std::divides<double>{});
}

object *float_klass::mod(object *x, object *y) { return new float_num{0.0}; }

float_num::float_num(double x) : value{x} {
  set_klass(float_klass::get_instance());
}
