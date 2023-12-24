#include "runtime/string_table.hpp"
#include "object/string.hpp"

using namespace cppython;

string_table *string_table::get_instance() {
  static string_table instance;
  return &instance;
}

string_table::string_table() {
  next_str = std::make_shared<string>("next");
  mod_str = std::make_shared<string>("__module__");
  init_str = std::make_shared<string>("__init__");
  add_str = std::make_shared<string>("__add__");
  len_str = std::make_shared<string>("__len__");
  call_str = std::make_shared<string>("__call__");
  getitem_str = std::make_shared<string>("__getitem__");
  setitem_str = std::make_shared<string>("__setitem__");
  getattr_str = std::make_shared<string>("__getattr__");
  setattr_str = std::make_shared<string>("__setattr__");
}
