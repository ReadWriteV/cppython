#include "runtime/string_table.hpp"
#include "object/string.hpp"

using namespace cppython;

string_table *string_table::get_instance() {
  static string_table instance;
  return &instance;
}

string_table::string_table() {
  next_str = std::make_shared<string>("__next__");
  mod_str = std::make_shared<string>("__module__");
  init_str = std::make_shared<string>("__init__");
  add_str = std::make_shared<string>("__add__");
  len_str = std::make_shared<string>("__len__");
  call_str = std::make_shared<string>("__call__");
  name_str = std::make_shared<string>("__name__");
  iter_str = std::make_shared<string>("__iter__");
  str_str = std::make_shared<string>("__str__");
  repr_str = std::make_shared<string>("__repr__");

  getitem_str = std::make_shared<string>("__getitem__");
  setitem_str = std::make_shared<string>("__setitem__");
  getattr_str = std::make_shared<string>("__getattr__");
  setattr_str = std::make_shared<string>("__setattr__");
}
