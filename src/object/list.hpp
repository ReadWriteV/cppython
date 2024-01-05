#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/vector.hpp"

#include <string>

namespace cppython {

class oop_closure;

template <typename T>
class vector;

class list_klass : public klass {
public:
  static list_klass *get_instance();

private:
  list_klass() = default;
  ~list_klass() = default;

public:
  list_klass(const list_klass &) = delete;
  list_klass(list_klass &&) = delete;
  list_klass &operator=(const list_klass &) = delete;
  list_klass &operator=(list_klass &&) = delete;

public:
  void initialize();

  std::string to_string(object *obj) override;

  object *less(object *x, object *y) override;

  object *add(object *x, object *y) override;
  object *mul(object *x, object *y) override;

  object *subscr(object *x, object *y) override;
  void store_subscr(object *x, object *y, object *z) override;
  void del_subscr(object *x, object *y) override;

  object *contains(object *x, object *y) override;

  object *iter(object *x) override;
  object *len(object *x) override;

  object *allocate_instance(object *obj_type, vector<object *> *args) override;

  void oops_do(oop_closure *closure, object *obj) override;
  size_t size() const override;
};

class list : public object {
public:
  using klass_type = list_klass;

public:
  list() : value{new vector<object *>{}} {
    set_klass(list_klass::get_instance());
  }

  list(vector<object *> *lst) : value{lst} {
    set_klass(list_klass::get_instance());
  }

  auto get_value() const { return value; }
  auto &data_address() { return value; }

  size_t size() const { return value->size(); }

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, object *, object *>
  [[nodiscard]] auto find(object *x, PredicateOperation pred) {
    return value->index(x, pred);
  }

  [[nodiscard]] bool empty() { return value->empty(); }

  void append(object *x) { value->push_back(x); }
  object *pop() { return value->pop_back(); }

  object *at(size_t pos) { return value->at(pos); }
  void set_at(size_t pos, object *x) { value->set(pos, x); }

  object *get(size_t index) {
    return index >= value->size() ? nullptr : value->at(index);
  }

  object *back() { return value->back(); }

  static object *list_append(vector<object *> *args);

  static object *list_index(vector<object *> *args);

  static object *list_pop(vector<object *> *args);

  static object *list_remove(vector<object *> *args);

  static object *list_reverse(vector<object *> *args);

  static object *list_sort(vector<object *> *args);

  static object *list_extend(vector<object *> *args);

  static object *list_getitem(vector<object *> *args);

private:
  vector<object *> *value{nullptr};
};

class list_iterator_klass : public klass {
public:
  static list_iterator_klass *get_instance();

public:
  list_iterator_klass(const list_iterator_klass &) = delete;
  list_iterator_klass(list_iterator_klass &&) = delete;
  list_iterator_klass &operator=(const list_iterator_klass &) = delete;
  list_iterator_klass &operator=(list_iterator_klass &&) = delete;

private:
  list_iterator_klass();

public:
  object *next(object *x) override;
};

class list_iterator : public object {
public:
  using klass_type = list_iterator_klass;

public:
  list_iterator(list *owner);

  list *get_list() { return lst; }
  int get_iter_cnt() { return iter_cnt; }
  void inc_cnt() { iter_cnt++; }

  static object *list_iterator_next(vector<object *> *args);

private:
  list *lst{nullptr};
  int iter_cnt{0};
};

} // namespace cppython
