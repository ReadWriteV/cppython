#pragma once

#include <compare>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace cppython {

class object;
class string;
class dict;
class type;
class list;

// TODO: implement other overload functions
class klass {
public:
  ~klass() = default;

  void add_super(klass *x);
  std::shared_ptr<type> get_super();

  void order_supers();

  void set_super_list(const std::shared_ptr<list> &x) { super = x; }
  auto get_mro() { return mro; }

  void set_type_object(const std::shared_ptr<type> &x) { type_object = x; }
  std::shared_ptr<type> get_type_object() { return type_object; }

  void set_name(std::string_view x) { name = x; }
  std::string get_name() const { return name; }

  void set_dict(std::shared_ptr<dict> x) { attr = x; }
  std::shared_ptr<dict> get_dict() { return attr; }

  static std::weak_ordering compare(klass *x, klass *y);

  virtual std::string to_string(std::shared_ptr<object> obj);

  virtual std::shared_ptr<object> greater(std::shared_ptr<object> x,
                                          std::shared_ptr<object> y) {
    return nullptr;
  }
  virtual std::shared_ptr<object> less(std::shared_ptr<object> x,
                                       std::shared_ptr<object> y) {
    return nullptr;
  }
  virtual std::shared_ptr<object> equal(std::shared_ptr<object> x,
                                        std::shared_ptr<object> y) {
    return nullptr;
  }
  virtual std::shared_ptr<object> not_equal(std::shared_ptr<object> x,
                                            std::shared_ptr<object> y) {
    return nullptr;
  }
  virtual std::shared_ptr<object> ge(std::shared_ptr<object> x,
                                     std::shared_ptr<object> y) {
    return nullptr;
  }
  virtual std::shared_ptr<object> le(std::shared_ptr<object> x,
                                     std::shared_ptr<object> y) {
    return nullptr;
  }

  virtual std::shared_ptr<object> add(std::shared_ptr<object> x,
                                      std::shared_ptr<object> y);
  virtual std::shared_ptr<object> sub(std::shared_ptr<object> x,
                                      std::shared_ptr<object> y) {
    return nullptr;
  }
  virtual std::shared_ptr<object> mul(std::shared_ptr<object> x,
                                      std::shared_ptr<object> y) {
    return nullptr;
  }
  virtual std::shared_ptr<object> div(std::shared_ptr<object> x,
                                      std::shared_ptr<object> y) {
    return nullptr;
  }
  virtual std::shared_ptr<object> mod(std::shared_ptr<object> x,
                                      std::shared_ptr<object> y) {
    return nullptr;
  }
  virtual std::shared_ptr<object> subscr(std::shared_ptr<object> x,
                                         std::shared_ptr<object> y);
  virtual void store_subscr(std::shared_ptr<object> x,
                            std::shared_ptr<object> y,
                            std::shared_ptr<object> z);
  virtual void del_subscr(std::shared_ptr<object> x,
                          std::shared_ptr<object> y) {
    return;
  }

  virtual std::shared_ptr<object> getattr(std::shared_ptr<object> x,
                                          std::shared_ptr<object> y);
  virtual std::shared_ptr<object> setattr(std::shared_ptr<object> x,
                                          std::shared_ptr<object> y,
                                          std::shared_ptr<object> z);

  virtual std::shared_ptr<object> get_klass_attr(std::shared_ptr<object> x,
                                                 std::shared_ptr<object> y);

  virtual std::shared_ptr<object> contains(std::shared_ptr<object> x,
                                           std::shared_ptr<object> y) {
    return nullptr;
  }
  virtual std::shared_ptr<object> iter(std::shared_ptr<object> x);
  virtual std::shared_ptr<object> next(std::shared_ptr<object> x);
  virtual std::shared_ptr<object> len(std::shared_ptr<object> x);
  virtual std::shared_ptr<object>
  call(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
    return nullptr;
  }
  virtual std::shared_ptr<object>
  allocate_instance(std::shared_ptr<object> obj_type,
                    std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

private:
  std::shared_ptr<object>
  find_and_call(std::shared_ptr<object> x,
                std::shared_ptr<std::vector<std::shared_ptr<object>>> args,
                std::shared_ptr<string> func_name);
  std::shared_ptr<object> find_in_parents(std::shared_ptr<object> x,
                                          std::shared_ptr<object> y);

private:
  std::shared_ptr<list> super;
  std::shared_ptr<list> mro;

  std::shared_ptr<type> type_object;

  std::string name;
  std::shared_ptr<dict> attr;
};

} // namespace cppython