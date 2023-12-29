#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"
#include "utils/vector.hpp"

#include <concepts>
#include <string>

namespace cppython {

class tuple_klass : public klass, public singleton<tuple_klass> {
  friend class singleton<tuple_klass>;

private:
  tuple_klass();

public:
  std::string to_string(object *obj) override;

  object *subscr(object *x, object *y) override;

  void oops_do(oop_closure *closure, object *obj) override;
  size_t size() const override;
};

class tuple : public object {
public:
  using klass_type = tuple_klass;

public:
  tuple(vector<object *> *const lst) : value{lst} {
    set_klass(tuple_klass::get_instance());
  }

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, object *, object *>
  [[nodiscard]] auto find(object *x, PredicateOperation pred) const {
    return value->index(x, pred);
  }

  [[nodiscard]] auto get_value() const { return value; }
  [[nodiscard]] auto &data_address() { return value; }

  [[nodiscard]] auto empty() const { return value->empty(); }
  [[nodiscard]] auto size() const { return value->size(); }
  [[nodiscard]] auto at(const size_t pos) const { return value->at(pos); }

private:
  vector<object *> *value;
};

} // namespace cppython
