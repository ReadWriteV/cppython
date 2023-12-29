#include "runtime/string_table.hpp"
#include "memory/oop_closure.hpp"
#include "object/string.hpp"

using namespace cppython;

string_table::string_table() {
  next_str = new string{"next"};
  mod_str = new string{"__module__"};
  init_str = new string{"__init__"};
  add_str = new string{"__add__"};
  len_str = new string{"__len__"};
  call_str = new string{"__call__"};
  getitem_str = new string{"__getitem__"};
  setitem_str = new string{"__setitem__"};
  getattr_str = new string{"__getattr__"};
  setattr_str = new string{"__setattr__"};
}
void string_table::oops_do(oop_closure *f) {
  f->do_oop(reinterpret_cast<object *&>(next_str));
  f->do_oop(reinterpret_cast<object *&>(mod_str));
  f->do_oop(reinterpret_cast<object *&>(init_str));
  f->do_oop(reinterpret_cast<object *&>(add_str));
  f->do_oop(reinterpret_cast<object *&>(len_str));
  f->do_oop(reinterpret_cast<object *&>(call_str));
  f->do_oop(reinterpret_cast<object *&>(getitem_str));
  f->do_oop(reinterpret_cast<object *&>(setitem_str));
  f->do_oop(reinterpret_cast<object *&>(setattr_str));
  f->do_oop(reinterpret_cast<object *&>(getattr_str));
}
