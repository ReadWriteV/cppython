#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

#include <concepts>
#include <memory>

namespace cppython {

class integer_klass : public klass, public singleton<integer_klass> {
  friend class singleton<integer_klass>;

private:
  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, int, int>
  std::shared_ptr<object> binary_predicate(const std::shared_ptr<object> &x,
                                           const std::shared_ptr<object> &y,
                                           PredicateOperation pred);

  template <typename BinaryOperation>
    requires std::invocable<BinaryOperation, int, int>
  std::shared_ptr<object> binary_op(const std::shared_ptr<object> &x,
                                    const std::shared_ptr<object> &y,
                                    BinaryOperation op);

public:
  void initialize();

  std::string to_string(std::shared_ptr<object> obj) override;

  std::shared_ptr<object> greater(std::shared_ptr<object> x,
                                  std::shared_ptr<object> y) override;
  std::shared_ptr<object> less(std::shared_ptr<object> x,
                               std::shared_ptr<object> y) override;
  std::shared_ptr<object> equal(std::shared_ptr<object> x,
                                std::shared_ptr<object> y) override;
  std::shared_ptr<object> not_equal(std::shared_ptr<object> x,
                                    std::shared_ptr<object> y) override;
  std::shared_ptr<object> ge(std::shared_ptr<object> x,
                             std::shared_ptr<object> y) override;
  std::shared_ptr<object> le(std::shared_ptr<object> x,
                             std::shared_ptr<object> y) override;

  std::shared_ptr<object> add(std::shared_ptr<object> x,
                              std::shared_ptr<object> y) override;
  std::shared_ptr<object> sub(std::shared_ptr<object> x,
                              std::shared_ptr<object> y) override;
  std::shared_ptr<object> mul(std::shared_ptr<object> x,
                              std::shared_ptr<object> y) override;
  std::shared_ptr<object> div(std::shared_ptr<object> x,
                              std::shared_ptr<object> y) override;
  std::shared_ptr<object> mod(std::shared_ptr<object> x,
                              std::shared_ptr<object> y) override;

  std::shared_ptr<object> allocate_instance(
      std::shared_ptr<object> obj_type,
      std::shared_ptr<std::vector<std::shared_ptr<object>>> args) override;
};

class integer : public object {
public:
  integer(const int x) : value{x} { set_klass(integer_klass::get_instance()); }

  int get_value() const { return value; }

private:
  int value;
};

} // namespace cppython
