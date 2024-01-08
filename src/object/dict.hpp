#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "runtime/static_value.hpp"
#include "utils/singleton.hpp"

#include <concepts>
#include <optional>
#include <string>
#include <unordered_map>

namespace cppython {

class string;

class dict_klass : public klass, public singleton<dict_klass> {
public:
  void initialize();

  std::string to_string(std::shared_ptr<object> obj) override;
  std::shared_ptr<object> subscr(std::shared_ptr<object> x,
                                 std::shared_ptr<object> y) override;

  std::shared_ptr<object> iter(std::shared_ptr<object> x) override;
  void store_subscr(std::shared_ptr<object> x, std::shared_ptr<object> y,
                    std::shared_ptr<object> z) override;
  void del_subscr(std::shared_ptr<object> x,
                  std::shared_ptr<object> y) override;

  std::shared_ptr<object> getattr(std::shared_ptr<object> x,
                                  std::shared_ptr<string> y);

  std::shared_ptr<object> allocate_instance(
      std::shared_ptr<object> obj_type,
      std::shared_ptr<std::vector<std::shared_ptr<object>>> args) override;
};

class dict : public object {
public:
  template <typename... Args>
  dict(Args &&...args) : value{std::forward<Args>(args)...} {
    set_klass(dict_klass::get_instance());
  }

  auto &get_value() { return value; }

  auto size() { return value.size(); }

  bool has_key(std::shared_ptr<object> k);

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, const std::shared_ptr<object> &,
                            const std::shared_ptr<object> &>
  void insert_or_assign(std::shared_ptr<object> k, std::shared_ptr<object> v,
                        PredicateOperation pred) {
    auto iter = std::find_if(value.begin(), value.end(),
                             [&](auto &&e) { return pred(e.first, k); });

    if (iter == value.end()) {
      value.insert({k, v});
    } else {
      iter->second = v;
    }
  }

  void insert(std::shared_ptr<object> k, std::shared_ptr<object> v) {
    insert_or_assign(k, v, value_equal{});
  }

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, const std::shared_ptr<object> &,
                            const std::shared_ptr<object> &>
  std::optional<std::shared_ptr<object>> get(std::shared_ptr<object> k,
                                             PredicateOperation pred) {
    auto iter = std::find_if(value.begin(), value.end(),
                             [&](auto &&e) { return pred(e.first, k); });

    if (iter == value.end()) {
      return std::nullopt;
    } else {
      return iter->second;
    }
  }

  std::shared_ptr<object> at(std::shared_ptr<object> k);
  std::shared_ptr<object> remove(std::shared_ptr<object> k);

  static std::shared_ptr<object>
  dict_set_default(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);
  static std::shared_ptr<object>
  dict_remove(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);
  static std::shared_ptr<object>
  dict_keys(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);
  static std::shared_ptr<object>
  dict_values(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);
  static std::shared_ptr<object>
  dict_items(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

private:
  std::unordered_map<std::shared_ptr<object>, std::shared_ptr<object>> value;
};

class dict_iterator_klass : public klass,
                            public singleton<dict_iterator_klass> {
  friend class singleton<dict_iterator_klass>;

private:
  dict_iterator_klass();

public:
  std::shared_ptr<object> iter(std::shared_ptr<object> x) override { return x; }
  std::shared_ptr<object> next(std::shared_ptr<object> x) override;
};

class dict_iterator : public object {

public:
  dict_iterator(std::shared_ptr<dict> owner);

  auto get_owner() { return dic; }
  int get_iter_cnt() { return iter_cnt; }
  void inc_cnt() { iter_cnt++; }

private:
  std::shared_ptr<dict> dic;
  int iter_cnt{0};
};

} // namespace cppython
