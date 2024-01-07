#pragma once

#include "object/klass.hpp"
#include "utils/singleton.hpp"

#include <memory>
#include <string>

namespace cppython {

class object_klass : public klass, public singleton<object_klass> {
  friend class singleton<object_klass>;
};

class object : public std::enable_shared_from_this<object> {
public:
  virtual ~object() = default;

  void set_klass(klass *x);
  [[nodiscard]] klass *get_klass() { return klass_; }

  void set_obj_dict(std::shared_ptr<dict> x) { obj_dict = x; }
  [[nodiscard]] auto get_obj_dict() { return obj_dict; }

  [[nodiscard]] std::string to_string();

  std::shared_ptr<object> add(std::shared_ptr<object> x);
  std::shared_ptr<object> sub(std::shared_ptr<object> x);
  std::shared_ptr<object> mul(std::shared_ptr<object> x);
  std::shared_ptr<object> div(std::shared_ptr<object> x);
  std::shared_ptr<object> mod(std::shared_ptr<object> x);

  std::shared_ptr<object> greater(std::shared_ptr<object> x);
  std::shared_ptr<object> less(std::shared_ptr<object> x);
  std::shared_ptr<object> equal(std::shared_ptr<object> x);
  std::shared_ptr<object> not_equal(std::shared_ptr<object> x);
  std::shared_ptr<object> ge(std::shared_ptr<object> x);
  std::shared_ptr<object> le(std::shared_ptr<object> x);

  std::shared_ptr<object> getattr(std::shared_ptr<object> x);
  std::shared_ptr<object> setattr(std::shared_ptr<object> x,
                                  std::shared_ptr<object> y);

  std::shared_ptr<object> get_klass_attr(std::shared_ptr<object> x);

  std::shared_ptr<object> subscr(std::shared_ptr<object> x);
  void store_subscr(std::shared_ptr<object> x, std::shared_ptr<object> y);
  void del_subscr(std::shared_ptr<object> x);

  std::shared_ptr<object> contains(std::shared_ptr<object> x);

  std::shared_ptr<object> iter();
  std::shared_ptr<object> len();

private:
  klass *klass_;
  std::shared_ptr<dict> obj_dict;
};

class type_klass : public klass, public singleton<type_klass> {
  friend class singleton<type_klass>;

private:
  type_klass() {}

public:
  std::string to_string(std::shared_ptr<object> obj) override;

  std::shared_ptr<object> setattr(std::shared_ptr<object> x,
                                  std::shared_ptr<object> y,
                                  std::shared_ptr<object> z) override;
};

class type : public object {
public:
  type() { set_klass(type_klass::get_instance()); }

  void set_own_klass(klass *k) {
    own_klass = k;
    k->set_type_object(std::static_pointer_cast<type>(shared_from_this()));
  }

  klass *get_own_klass() { return own_klass; }

  std::string get_type_name();

private:
  klass *own_klass;
};

} // namespace cppython
