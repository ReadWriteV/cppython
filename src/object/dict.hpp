#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

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

  virtual std::shared_ptr<object> getattr(std::shared_ptr<object> x,
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
  void insert(std::shared_ptr<object> k, std::shared_ptr<object> v) {
    value.insert_or_assign(k, v);
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
  static std::shared_ptr<object>
  dict_iterkeys(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);
  static std::shared_ptr<object>
  dict_itervalues(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);
  static std::shared_ptr<object>
  dict_iteritems(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

private:
  std::unordered_map<std::shared_ptr<object>, std::shared_ptr<object>> value;
};

enum iter_type { iter_key = 0, iter_value, iter_item };

template <iter_type n>
class dict_iterator_klass : public klass,
                            public singleton<dict_iterator_klass<n>> {
private:
  dict_iterator_klass();
  friend class singleton<dict_iterator_klass<n>>;

public:
  std::shared_ptr<object> iter(std::shared_ptr<object> x) override { return x; }
};

class dict_iterator : public object {

public:
  dict_iterator(std::shared_ptr<dict> owner);

  auto get_owner() { return dic; }
  int get_iter_cnt() { return iter_cnt; }
  void inc_cnt() { iter_cnt++; }

  template <iter_type n>
  static std::shared_ptr<object> dict_iterator_next(
      std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

private:
  std::shared_ptr<dict> dic;
  int iter_cnt{0};
};

} // namespace cppython
