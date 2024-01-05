#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/vector.hpp"

#include <concepts>
#include <string>

namespace cppython {

class tuple_klass : public klass {
public:
  static tuple_klass *get_instance();

public:
  tuple_klass(const tuple_klass &) = delete;
  tuple_klass(tuple_klass &&) = delete;
  tuple_klass &operator=(const tuple_klass &) = delete;
  tuple_klass &operator=(tuple_klass &&) = delete;

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
