#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

#include <concepts>

namespace cppython {

class oop_closure;

template <typename T>
class vector;

class integer_klass : public klass, public singleton<integer_klass> {
  friend class singleton<integer_klass>;

private:
  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, int, int>
  object *binary_predicate(object *x, object *y, PredicateOperation pred);

  template <typename BinaryOperation>
    requires std::invocable<BinaryOperation, int, int>
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

  object *allocate_instance(object *obj_type, vector<object *> *args) override;

  void oops_do(oop_closure *closure, object *obj) override;
  size_t size() const override;
};

class integer : public object {
public:
  using klass_type = integer_klass;

public:
  integer(const int x) : value{x} { set_klass(integer_klass::get_instance()); }

  int get_value() const { return value; }

private:
  int value;
};

} // namespace cppython
