#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace cppython {

class code_object;
class list;
class dict;

class function_klass : public klass, public singleton<function_klass> {
private:
  function_klass();
  friend class singleton<function_klass>;

public:
  std::string to_string(std::shared_ptr<object> obj) override;
};

class native_function_klass : public klass,
                              public singleton<native_function_klass> {
private:
  native_function_klass();

  friend class singleton<native_function_klass>;
};

class method_klass : public klass, public singleton<method_klass> {
private:
  method_klass();

  friend class singleton<method_klass>;
};

std::shared_ptr<object>
len(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

std::shared_ptr<object>
iter(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

std::shared_ptr<object>
print(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

std::shared_ptr<object>
type_of(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

std::shared_ptr<object>
isinstance(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

/// @brief build a class
/// @param args first element is function object, second element is name, ...
/// are parent class type, last one is locals
/// @return an object of type, which represents the class
std::shared_ptr<object>
build_class(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

class function : public object {
  using native_function_t = std::shared_ptr<object>(
      std::shared_ptr<std::vector<std::shared_ptr<object>>>);

public:
  enum co_flags {
    var_args = 0x4,
    var_keywords = 0x8,
    generator = 0x20,
  };

  function(std::shared_ptr<object> obj);
  function(klass *klass) { set_klass(klass); }
  function(native_function_t *native_func);

  std::shared_ptr<object>
  call(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  auto get_func_code() { return func_code; }
  auto get_func_name() { return func_name; }
  auto get_flags() { return flags; }
  auto get_native_func() { return native_func; }

  auto get_globals() { return globals; }
  void set_globals(std::shared_ptr<dict> x) { globals = x; }

  auto get_default_args() { return default_args; }
  void set_default_args(
      const std::shared_ptr<std::vector<std::shared_ptr<object>>> &x) {
    default_args = x;
  }

  void set_closure(const std::shared_ptr<list> &x) { closure = x; }
  auto get_closure() { return closure; }

private:
  std::shared_ptr<code_object> func_code;
  std::shared_ptr<object> func_name;
  std::shared_ptr<dict> globals;

  std::shared_ptr<std::vector<std::shared_ptr<object>>> default_args;
  std::shared_ptr<list> closure;

  int flags{0};

  native_function_t *native_func;
};

class method : public object {
public:
  method(std::shared_ptr<function> func) : func(func) {
    set_klass(method_klass::get_instance());
  }

  method(std::shared_ptr<function> func, std::shared_ptr<object> owner)
      : owner(owner), func(func) {
    set_klass(method_klass::get_instance());
  }

  void set_owner(std::shared_ptr<object> x) { owner = x; }
  auto get_owner() { return owner; }

  auto get_func() { return func; }

  static bool is_function(std::shared_ptr<object> x);
  static bool is_yield_function(std::shared_ptr<object> x);

private:
  std::shared_ptr<object> owner;
  std::shared_ptr<function> func;
};

} // namespace  cppython
