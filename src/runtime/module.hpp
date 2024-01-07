#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

#include <memory>

namespace cppython {

class dict;

class module_klass : public klass, public singleton<module_klass> {
  friend class singleton<module_klass>;

public:
  void initialize();
};

class Module : public object {
public:
  Module(std::shared_ptr<dict> x);

  static std::shared_ptr<Module> import(std::shared_ptr<string> module_name);
  static std::shared_ptr<Module>
  import_dll(std::shared_ptr<string> module_name);

  void extend(std::shared_ptr<Module> m);

  void insert(std::shared_ptr<object> x, std::shared_ptr<object> y);
  std::shared_ptr<object> get(std::shared_ptr<object> x);
  std::shared_ptr<string> &get_module_name() { return module_name; }

private:
  std::shared_ptr<string> module_name;
};

} // namespace cppython
