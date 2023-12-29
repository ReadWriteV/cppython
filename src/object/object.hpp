#pragma once

#include "object/klass.hpp"
#include "utils/singleton.hpp"

#include <cassert>
#include <concepts>
#include <string>

namespace cppython {

class oop_closure;

class object_klass : public klass, public singleton<object_klass> {
  friend class singleton<object_klass>;
};

class object {
public:
  using klass_type = object_klass;

public:
  virtual ~object() = default;

  template <typename T>
    requires requires { typename T::klass_type; } &&
             std::derived_from<typename T::klass_type, klass> &&
             std::derived_from<typename T::klass_type,
                               singleton<typename T::klass_type>>
  [[nodiscard]] bool is() {
    assert(this != nullptr);
    return get_klass() == T::klass_type::get_instance();
  }

  template <typename T>
    requires requires { typename T::klass_type; } &&
             std::derived_from<typename T::klass_type, klass> &&
             std::derived_from<typename T::klass_type,
                               singleton<typename T::klass_type>>
  [[nodiscard]] T *as() {
    assert(is<T>());
    return static_cast<T *>(this);
  }

  void set_klass(klass *x) {
    assert(x != nullptr);
    klass_ = x;
  }
  [[nodiscard]] klass *get_klass() const { return klass_; }

  void set_obj_dict(dict *x) { obj_dict = x; }
  [[nodiscard]] dict *get_obj_dict() const { return obj_dict; }

  [[nodiscard]] std::string to_string();

  [[nodiscard]] object *add(object *x);
  [[nodiscard]] object *sub(object *x);
  [[nodiscard]] object *mul(object *x);
  [[nodiscard]] object *div(object *x);
  [[nodiscard]] object *mod(object *x);

  [[nodiscard]] object *greater(object *x);
  [[nodiscard]] object *less(object *x);
  [[nodiscard]] object *equal(object *x);
  [[nodiscard]] object *not_equal(object *x);
  [[nodiscard]] object *ge(object *x);
  [[nodiscard]] object *le(object *x);

  [[nodiscard]] object *getattr(object *x);
  object *setattr(object *x, object *y);

  [[nodiscard]] object *subscr(object *x);
  void store_subscr(object *x, object *y);
  void del_subscr(object *x);

  [[nodiscard]] object *contains(object *x);

  [[nodiscard]] object *iter();
  [[nodiscard]] object *len();

  void *operator new(size_t size);

  // interfaces for GC.
  void oops_do(oop_closure *closure);
  size_t size() const;
  char *new_address();
  void set_new_address(char *addr);

private:
  intmax_t mark_word{0};
  klass *klass_{nullptr};
  dict *obj_dict{nullptr};
};

class type_klass : public klass, public singleton<type_klass> {
  friend class singleton<type_klass>;

public:
  [[nodiscard]] std::string to_string(object *obj) override;

  [[nodiscard]] object *setattr(object *x, object *y, object *z) override;

  void oops_do(oop_closure *f, object *obj) override;
  size_t size() const override;
};

class type : public object {
public:
  using klass_type = type_klass;

public:
  type() { set_klass(type_klass::get_instance()); }

  void set_own_klass(klass *k) {
    own_klass = k;
    k->set_type_object(this);
  }

  [[nodiscard]] klass *get_own_klass() const { return own_klass; }

  [[nodiscard]] std::string get_type_name() const;

private:
  klass *own_klass{nullptr};
};

} // namespace cppython
