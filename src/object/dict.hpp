#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "runtime/static_value.hpp"
#include "utils/map.hpp"

#include <string>

namespace cppython {

class string;
class oop_closure;

class dict_klass : public klass {
public:
  static dict_klass *get_instance();

private:
  dict_klass() = default;
  ~dict_klass() = default;

public:
  dict_klass(const dict_klass &) = delete;
  dict_klass(dict_klass &&) = delete;
  dict_klass &operator=(const dict_klass &) = delete;
  dict_klass &operator=(dict_klass &&) = delete;

public:
  void initialize();

  std::string to_string(object *obj) override;
  object *subscr(object *x, object *y) override;

  object *iter(object *x) override;
  void store_subscr(object *x, object *y, object *z) override;
  void del_subscr(object *x, object *y) override;

  virtual object *getattr(object *x, string *y);

  object *allocate_instance(object *obj_type, vector<object *> *args) override;

  void oops_do(oop_closure *closure, object *obj) override;
  size_t size() const override;
};

class dict : public object {
public:
  using klass_type = dict_klass;

public:
  dict() : value{new map<object *, object *>{}} {
    set_klass(dict_klass::get_instance());
  }
  dict(map<object *, object *> *m) : value{m} {
    set_klass(dict_klass::get_instance());
  }

  auto get_value() { return value; }
  auto &data_address() { return value; }

  auto size() { return value->size(); }

  /// @brief return value of key k, or std::nullopt if not exists
  /// @tparam PredicateOperation
  /// @param k
  /// @param pred
  /// @return
  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, object *, object *>
  std::optional<object *> get(object *k, PredicateOperation pred) {
    return value->get(k, pred);
  }

  bool has_key(object *k) { return value->has_key(k, value_equal{}); }
  void insert(object *k, object *v) { value->insert(k, v, value_equal{}); }
  /// @brief return value of key k, or none_value if not exists
  /// @param k
  /// @return
  object *at(object *k);
  object *remove(object *k) { return value->remove(k, value_equal{}); }

  static object *dict_set_default(vector<object *> *args);
  static object *dict_remove(vector<object *> *args);
  static object *dict_keys(vector<object *> *args);
  static object *dict_values(vector<object *> *args);
  static object *dict_items(vector<object *> *args);
  static object *dict_iterkeys(vector<object *> *args);
  static object *dict_itervalues(vector<object *> *args);
  static object *dict_iteritems(vector<object *> *args);

private:
  map<object *, object *> *value{nullptr};
};

enum iter_type { iter_key = 0, iter_value, iter_item };

template <iter_type n>
class dict_iterator_klass : public klass {
public:
  static dict_iterator_klass *get_instance();

public:
  dict_iterator_klass(const dict_iterator_klass &) = delete;
  dict_iterator_klass(dict_iterator_klass &&) = delete;
  dict_iterator_klass &operator=(const dict_iterator_klass &) = delete;
  dict_iterator_klass &operator=(dict_iterator_klass &&) = delete;

private:
  dict_iterator_klass();

public:
  object *iter(object *x) override { return x; }
  object *next(object *x) override;
};

class dict_iterator : public object {
public:
  using klass_type = object_klass; // workaround

public:
  dict_iterator(dict *owner);

  auto get_owner() { return dic; }
  int get_iter_cnt() { return iter_cnt; }
  void inc_cnt() { iter_cnt++; }

private:
  dict *dic{nullptr};
  int iter_cnt{0};
};

} // namespace cppython
