#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"

#include <string>

namespace cppython {

class oop_closure;

template <typename T>
class vector;

class float_klass : public klass {
public:
  static float_klass *get_instance();

private:
  float_klass() = default;
  ~float_klass() = default;

public:
  float_klass(const float_klass &) = delete;
  float_klass(float_klass &&) = delete;
  float_klass &operator=(const float_klass &) = delete;
  float_klass &operator=(float_klass &&) = delete;

private:
  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, double, double>
  object *binary_predicate(object *x, object *y, PredicateOperation pred);

  template <typename BinaryOperation>
    requires std::invocable<BinaryOperation, double, double>
  object *binary_op(object *x, object *y, BinaryOperation op);

public:
  void initialize();

  std::string to_string(object *obj) override;

  object *greater(object *x, object *y) override;
  object *less(object *x, object *y) override;
  object *equal(object *x, object *y) override;
  object *not_equal(object *x, object *y) override;
  object *ge(object *x, object *y) override;
  object *le(object *x, object *y) override;

  object *add(object *x, object *y) override;
  object *sub(object *x, object *y) override;
  object *mul(object *x, object *y) override;
  object *div(object *x, object *y) override;
  object *mod(object *x, object *y) override;

  object *allocate_instance(object *callable, vector<object *> *args) override;

  void oops_do(oop_closure *f, object *obj) override;
  size_t size() const override;
};

class float_num : public object {
public:
  using klass_type = float_klass;

public:
  float_num(double x);
  double get_value() { return value; }

private:
  double value;
};

} // namespace cppython
