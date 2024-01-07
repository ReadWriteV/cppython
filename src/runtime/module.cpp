#include "runtime/module.hpp"
#include "code/pyc_parser.hpp"
#include "inc/cppython.hpp"
#include "object/dict.hpp"
#include "object/string.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/string_table.hpp"

#include <cassert>
#include <filesystem>
#include <print>

#include <windows.h>

using namespace cppython;

void module_klass::initialize() {
  set_dict(std::make_shared<dict>());
  set_name("module");
  std::make_shared<type>()->set_own_klass(this);
  add_super(object_klass::get_instance());
}

Module::Module(std::shared_ptr<dict> x) {
  set_obj_dict(x);
  set_klass(module_klass::get_instance());
}

std::shared_ptr<Module> Module::import(std::shared_ptr<string> module_name) {
  auto file_name = std::format(R"(./lib/{}.dll)", module_name->get_value());

  if (std::filesystem::exists(file_name)) {
    return import_dll(module_name);
  }

  file_name = module_name->get_value() + ".pyc";
  if (!std::filesystem::exists(file_name)) {
    file_name = std::format(R"(./lib/{}.pyc)", module_name->get_value());
  }

  assert(std::filesystem::exists(file_name));

  pyc_parser parser{file_name};
  auto module_code = parser.parse();
  auto module_dict =
      interpreter::get_instance()->run_module(module_code, module_name);
  return std::make_shared<Module>(module_dict);
}

std::shared_ptr<Module>
Module::import_dll(std::shared_ptr<string> module_name) {
  auto dll_file_name = std::format(R"(./lib/{}.dll)", module_name->get_value());

  auto hdll = ::LoadLibrary(dll_file_name.c_str());
  if (hdll == nullptr) {
    std::println("error to open file: ", dll_file_name);
    return nullptr;
  }

  auto init_method = "init_lib" + module_name->get_value();
  auto module_init_func = reinterpret_cast<init_func *>(
      ::GetProcAddress(hdll, init_method.c_str()));

  auto method_list = module_init_func();

  auto mod = std::make_shared<Module>(std::make_shared<dict>());

  while (method_list->method_func != nullptr) {
    mod->insert(std::make_shared<string>(method_list->method_name),
                std::make_shared<function>(method_list->method_func));
    method_list++;
  }
  return mod;
}

void Module::extend(std::shared_ptr<Module> m) {
  for (auto k_v : m->get_obj_dict()->get_value()) {
    get_obj_dict()->insert(k_v.first, k_v.second);
  }
}

void Module::insert(std::shared_ptr<object> x, std::shared_ptr<object> y) {
  get_obj_dict()->insert(x, y);
}

std::shared_ptr<object> Module::get(std::shared_ptr<object> x) {
  return get_obj_dict()->at(x);
}
