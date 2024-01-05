#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"

#include <string>

namespace cppython {

class code_object;
class list;
class dict;
class oop_closure;

template <typename T>
class vector;

class function_klass : public klass {
public:
  static function_klass *get_instance();

public:
  function_klass(const function_klass &) = delete;
  function_klass(function_klass &&) = delete;
  function_klass &operator=(const function_klass &) = delete;
  function_klass &operator=(function_klass &&) = delete;

private:
  function_klass();

public:
  std::string to_string(object *obj) override;

  void oops_do(oop_closure *f, object *obj) override;
  size_t size() const override;
};

class function : public object {
public:
  using klass_type = function_klass;

public:
  enum co_flags {
    var_args = 0x4,
    var_keywords = 0x8,
    generator = 0x20,
  };

  function(object *obj);
  function(klass *klass) { set_klass(klass); }

  auto &get_func_code() { return func_code; }
  auto &get_func_name() { return func_name; }

  auto get_flags() { return flags; }

  auto &get_globals() { return globals; }
  void set_globals(dict *x) { globals = x; }

  auto &get_default_args() { return default_args; }
  void set_default_args(vector<object *> *x) { default_args = x; }

  auto &get_closure() { return closure; }
  void set_closure(list *x) { closure = x; }

private:
  code_object *func_code{nullptr};
  object *func_name{nullptr};
  dict *globals{nullptr};

  vector<object *> *default_args{nullptr};
  list *closure{nullptr};

  int flags{0};
};

// native functions vvv
object *len(vector<object *> *args);

object *iter(vector<object *> *args);

object *print(vector<object *> *args);

object *type_of(vector<object *> *args);

object *isinstance(vector<object *> *args);

object *builtin_super(vector<object *> *args);

/// @brief build a class
/// @param args first element is function object, second element is name, ...
/// are parent class type, last one is locals
/// @return an object of type, which represents the class
object *build_class(vector<object *> *args);

object *sysgc(vector<object *> *args);
// native functions ^^^

class native_function_klass : public klass {
public:
  static native_function_klass *get_instance();

public:
  native_function_klass(const native_function_klass &) = delete;
  native_function_klass(native_function_klass &&) = delete;
  native_function_klass &operator=(const native_function_klass &) = delete;
  native_function_klass &operator=(native_function_klass &&) = delete;

private:
  native_function_klass();

public:
  void oops_do(oop_closure *f, object *obj) override;
  size_t size() const override;
};

class native_function : public object {
public:
  using klass_type = native_function_klass;
  using native_function_t = object *(vector<object *> *);

public:
  native_function(native_function_t *native_func);

  object *call(vector<object *> *args);

  bool is_build_class() const { return native_func == build_class; }

private:
  native_function_t *native_func{nullptr};
};

class method_klass : public klass {
public:
  static method_klass *get_instance();

public:
  method_klass(const method_klass &) = delete;
  method_klass(method_klass &&) = delete;
  method_klass &operator=(const method_klass &) = delete;
  method_klass &operator=(method_klass &&) = delete;

private:
  method_klass();

public:
  void oops_do(oop_closure *f, object *obj) override;
  size_t size() const override;
};

class method : public object {
public:
  using klass_type = method_klass;

public:
  method(function *func) : func(func) {
    set_klass(method_klass::get_instance());
  }

  method(function *func, object *owner) : owner(owner), func(func) {
    set_klass(method_klass::get_instance());
  }

  void set_owner(object *x) { owner = x; }
  auto &get_owner() { return owner; }

  auto &get_func() { return func; }

  static bool is_function(object *x);
  static bool is_yield_function(object *x);

private:
  object *owner{nullptr};
  function *func{nullptr};
};

} // namespace  cppython
