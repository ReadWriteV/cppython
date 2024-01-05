#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"

namespace cppython {

class dict;
class oop_closure;

class module_klass : public klass {
public:
  static module_klass *get_instance();

private:
  module_klass() = default;
  ~module_klass() = default;

public:
  module_klass(const module_klass &) = delete;
  module_klass(module_klass &&) = delete;
  module_klass &operator=(const module_klass &) = delete;
  module_klass &operator=(module_klass &&) = delete;

public:
  void initialize();

  void oops_do(oop_closure *closure, object *obj) override;
  size_t size() const override;
};

class module : public object {
public:
  using klass_type = module_klass;

public:
  module(dict *x);

  static module *import(object *module_name);
  static module *import_dll(string *module_name);

  void extend(module *m);

  void insert(object *x, object *y);
  object *get(object *x);
  string *&get_module_name() { return module_name; }

private:
  string *module_name;
};

} // namespace cppython
