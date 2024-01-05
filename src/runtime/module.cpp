#include "runtime/module.hpp"
#include "code/pyc_parser.hpp"
#include "inc/cppython.hpp"
#include "memory/oop_closure.hpp"
#include "object/dict.hpp"
#include "object/string.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/string_table.hpp"

#include <cassert>
#include <filesystem>
#include <print>

#include <windows.h>

using namespace cppython;

module_klass *module_klass::get_instance() {
  static module_klass instance;
  return &instance;
}

void module_klass::initialize() {
  set_dict(new dict{});
  set_name("module");
  (new type{})->set_own_klass(this);
  add_super(object_klass::get_instance());
}

void cppython::module_klass::oops_do(oop_closure *closure, object *obj) {
  auto module_obj = obj->as<module>();
  closure->do_oop(reinterpret_cast<object *&>(module_obj->get_module_name()));
}

size_t cppython::module_klass::size() const { return sizeof(module); }

module::module(dict *x) {
  set_obj_dict(x);
  set_klass(module_klass::get_instance());
}

module *module::import(object *module_name) {
  auto name = module_name->as<string>();

  auto dll_list = new list{};
  dll_list->append(string_table::get_instance()->libdir_pre_str);
  dll_list->append(name);
  dll_list->append(string_table::get_instance()->dll_suf_str);
  auto file_name = string_table::get_instance()->empty_str->join(dll_list);

  if (std::filesystem::exists(file_name->data())) {
    return import_dll(name);
  }

  file_name =
      name->add(string_table::get_instance()->pyc_suf_str)->as<string>();
  if (!std::filesystem::exists(file_name->data())) {
    auto pyc_list = new list{};
    pyc_list->append(string_table::get_instance()->libdir_pre_str);
    pyc_list->append(file_name);
    file_name = string_table::get_instance()->empty_str->join(pyc_list);
  }

  assert(std::filesystem::exists(file_name->data()));

  pyc_parser parser{file_name->data()};
  auto module_code = parser.parse();
  auto module_dict = interpreter::get_instance()->run_module(module_code, name);
  return new module{module_dict};
}

module *module::import_dll(string *module_name) {
  auto dll_list = new list{};
  dll_list->append(string_table::get_instance()->libdir_pre_str);
  dll_list->append(module_name);
  dll_list->append(string_table::get_instance()->dll_suf_str);
  auto dll_name = string_table::get_instance()->empty_str->join(dll_list);

  auto hdll = ::LoadLibrary(dll_name->data());
  if (hdll == nullptr) {
    std::println("error to open file: ", dll_name->data());
    return nullptr;
  }

  auto method_prefix = new string{"init_lib"};
  auto init_method = method_prefix->add(module_name)->as<string>();
  auto module_init_func = reinterpret_cast<init_func *>(
      ::GetProcAddress(hdll, init_method->data()));

  auto method_list = module_init_func();

  auto mod = new module{new dict{}};

  while (method_list->method_func != nullptr) {
    mod->insert(new string{method_list->method_name},
                new native_function{method_list->method_func});
    method_list++;
  }
  return mod;
}

void module::extend(module *m) {
  auto d = m->get_obj_dict()->get_value();
  for (auto k_v : *d) {
    get_obj_dict()->insert(k_v.key, k_v.value);
  }
}

void module::insert(object *x, object *y) { get_obj_dict()->insert(x, y); }

object *module::get(object *x) { return get_obj_dict()->at(x); }
