#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "runtime/static_value.hpp"
#include "utils/singleton.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace cppython {

class list_klass : public klass, public singleton<list_klass> {
private:
  friend class singleton<list_klass>;

public:
  void initialize();

  std::shared_ptr<string> repr(std::shared_ptr<object> obj) override;

  std::shared_ptr<object> less(std::shared_ptr<object> x,
                               std::shared_ptr<object> y) override;

  std::shared_ptr<object> add(std::shared_ptr<object> x,
                              std::shared_ptr<object> y) override;
  std::shared_ptr<object> mul(std::shared_ptr<object> x,
                              std::shared_ptr<object> y) override;

  std::shared_ptr<object> subscr(std::shared_ptr<object> x,
                                 std::shared_ptr<object> y) override;
  void store_subscr(std::shared_ptr<object> x, std::shared_ptr<object> y,
                    std::shared_ptr<object> z) override;
  void del_subscr(std::shared_ptr<object> x,
                  std::shared_ptr<object> y) override;

  std::shared_ptr<object> contains(std::shared_ptr<object> x,
                                   std::shared_ptr<object> y) override;

  std::shared_ptr<object> iter(std::shared_ptr<object> x) override;
  std::shared_ptr<object> len(std::shared_ptr<object> x) override;

  std::shared_ptr<object> allocate_instance(
      std::shared_ptr<object> obj_type,
      std::shared_ptr<std::vector<std::shared_ptr<object>>> args) override;
};

class list : public object {
public:
  template <typename... Args>
  list(Args &&...args) : value{std::forward<Args>(args)...} {
    set_klass(list_klass::get_instance());
  }

  auto &get_value() { return value; }

  auto begin() { return value.begin(); }
  auto rbegin() { return value.rbegin(); }

  size_t size() { return value.size(); }
  void resize(size_t cnt) { value.resize(cnt); }
  void resize(size_t cnt, const std::shared_ptr<object> &x) {
    value.resize(cnt, x);
  }

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, std::shared_ptr<object>,
                            std::shared_ptr<object>>
  [[nodiscard]] int find(const std::shared_ptr<object> &x,
                         PredicateOperation pred) {
    auto iter = std::ranges::find_if(value, std::bind_back(pred, x));
    return iter == value.end()
               ? -1
               : static_cast<int>(std::distance(value.begin(), iter));
  }

  [[nodiscard]] bool has_value(const std::shared_ptr<object> &x) {
    return find(x, [](const std::shared_ptr<object> &l,
                      const std::shared_ptr<object> &r) {
             return l->equal(r) == static_value::true_value;
           }) != -1;
  }

  [[nodiscard]] bool has_pointer(const std::shared_ptr<object> &x) {
    return find(x, std::equal_to{}) != -1;
  }

  [[nodiscard]] bool empty() { return value.empty(); }
  void append(const std::shared_ptr<object> &x) { value.push_back(x); }
  void set_at(size_t pos, const std::shared_ptr<object> &x) {
    if (pos < value.size()) {
      value.at(pos) = x;
    } else {
      value.resize(pos + 1);
      value.at(pos) = x;
    }
  }

  std::shared_ptr<object> pop() {
    auto r = value.back();
    value.pop_back();
    return r;
  }

  decltype(auto) at(size_t index) { return value.at(index); }
  decltype(auto) get(size_t index) {
    return index >= value.size() ? nullptr : value.at(index);
  }
  void set(size_t i, const std::shared_ptr<object> &x) { value.at(i) = x; }

  decltype(auto) top() { return value.back(); }

  static std::shared_ptr<object>
  list_append(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  static std::shared_ptr<object>
  list_index(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  static std::shared_ptr<object>
  list_pop(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  static std::shared_ptr<object>
  list_remove(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  static std::shared_ptr<object>
  list_reverse(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  static std::shared_ptr<object>
  list_sort(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  static std::shared_ptr<object>
  list_extend(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  static std::shared_ptr<object>
  list_getitem(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  static std::shared_ptr<object>
  list_repr(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

private:
  std::vector<std::shared_ptr<object>> value;
};

class list_iterator_klass : public klass,
                            public singleton<list_iterator_klass> {
  friend class singleton<list_iterator_klass>;

private:
  list_iterator_klass();

public:
  std::shared_ptr<object> next(std::shared_ptr<object> x) override;
};

class list_iterator : public object {

public:
  list_iterator(std::shared_ptr<list> owner);

  std::shared_ptr<list> get_list() { return lst; }
  int get_iter_cnt() { return iter_cnt; }
  void inc_cnt() { iter_cnt++; }

  static std::shared_ptr<object> list_iterator_next(
      std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

private:
  std::shared_ptr<list> lst;
  int iter_cnt{0};
};

} // namespace cppython
