#include "object/float.hpp"
#include "object/dict.hpp"
#include "runtime/static_value.hpp"

#include <cassert>
#include <compare>
#include <concepts>
#include <functional>
#include <print>

using namespace cppython;

void float_klass::initialize() {
  set_name("float");
  set_dict(std::make_shared<dict>());
  std::make_shared<type>()->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::string float_klass::to_string(std::shared_ptr<object> obj) {
  auto p = std::static_pointer_cast<float_num>(obj);
  assert(p && (p->get_klass() == this));
  return std::to_string(p->get_value());
}

template <typename PredicateOperation>
  requires std::predicate<PredicateOperation, double, double>
std::shared_ptr<object>
float_klass::binary_predicate(const std::shared_ptr<object> &x,
                              const std::shared_ptr<object> &y,
                              PredicateOperation pred) {
  assert(x && (x->get_klass() == this));
  auto p = std::static_pointer_cast<float_num>(x);

  // check y is float_num or not
  if (y && y->get_klass() != this) {
    return static_value::get_bool_value(
        std::is_lt(klass::compare(x->get_klass(), y->get_klass())));
  }

  assert(y && (y->get_klass() == this));
  auto q = std::static_pointer_cast<float_num>(y);

  return static_value::get_bool_value(pred(p->get_value(), q->get_value()));
}

std::shared_ptr<object> float_klass::greater(std::shared_ptr<object> x,
                                             std::shared_ptr<object> y) {
  return binary_predicate(x, y, std::greater<double>{});
}

std::shared_ptr<object> float_klass::less(std::shared_ptr<object> x,
                                          std::shared_ptr<object> y) {
  return binary_predicate(x, y, std::less<double>{});
}

std::shared_ptr<object> float_klass::equal(std::shared_ptr<object> x,
                                           std::shared_ptr<object> y) {

  if (x->get_klass() != y->get_klass()) {
    return static_value::false_value;
  }

  return binary_predicate(x, y, std::equal_to<double>{});
}

std::shared_ptr<object> float_klass::not_equal(std::shared_ptr<object> x,
                                               std::shared_ptr<object> y) {
  if (x->get_klass() != y->get_klass()) {
    return static_value::true_value;
  }

  return binary_predicate(x, y, std::not_equal_to<double>{});
}

std::shared_ptr<object> float_klass::ge(std::shared_ptr<object> x,
                                        std::shared_ptr<object> y) {
  return binary_predicate(x, y, std::greater_equal<double>{});
}

std::shared_ptr<object> float_klass::le(std::shared_ptr<object> x,
                                        std::shared_ptr<object> y) {
  return binary_predicate(x, y, std::less_equal<double>{});
}

template <typename BinaryOperation>
  requires std::invocable<BinaryOperation, double, double>
std::shared_ptr<object> float_klass::binary_op(const std::shared_ptr<object> &x,
                                               const std::shared_ptr<object> &y,
                                               BinaryOperation op) {
  assert(x && (x->get_klass() == this));
  assert(y && (y->get_klass() == this));

  auto p = std::static_pointer_cast<float_num>(x);
  auto q = std::static_pointer_cast<float_num>(y);

  return std::make_shared<float_num>(op(p->get_value(), q->get_value()));
}

std::shared_ptr<object> float_klass::add(std::shared_ptr<object> x,
                                         std::shared_ptr<object> y) {
  return binary_op(x, y, std::plus<double>{});
}

std::shared_ptr<object> float_klass::sub(std::shared_ptr<object> x,
                                         std::shared_ptr<object> y) {
  return binary_op(x, y, std::minus<double>{});
}

std::shared_ptr<object> float_klass::mul(std::shared_ptr<object> x,
                                         std::shared_ptr<object> y) {
  return binary_op(x, y, std::multiplies<double>{});
}

std::shared_ptr<object> float_klass::div(std::shared_ptr<object> x,
                                         std::shared_ptr<object> y) {
  return binary_op(x, y, std::divides<double>{});
}

std::shared_ptr<object> float_klass::mod(std::shared_ptr<object> x,
                                         std::shared_ptr<object> y) {
  return std::make_shared<float_num>(0.);
}

std::shared_ptr<object> float_klass::allocate_instance(
    std::shared_ptr<object> obj_type,
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  if (!args || args->size() == 0) {
    return std::make_shared<float_num>(0);
  } else {
    return nullptr;
  }
}