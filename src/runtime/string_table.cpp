#include "runtime/string_table.hpp"
#include "memory/oop_closure.hpp"
#include "object/string.hpp"

using namespace cppython;

string_table *string_table::get_instance() {
  static string_table instance;
  return &instance;
}

string_table::string_table() {
  mod_str = new string{"__module__"};
  init_str = new string{"__init__"};
  add_str = new string{"__add__"};
  len_str = new string{"__len__"};
  call_str = new string{"__call__"};
  name_str = new string{"__name__"};
  iter_str = new string{"__iter__"};
  next_str = new string{"__next__"};
  repr_str = new string{"__repr__"};

  getitem_str = new string{"__getitem__"};
  setitem_str = new string{"__setitem__"};
  getattr_str = new string{"__getattr__"};
  setattr_str = new string{"__setattr__"};

  empty_str = new string{""};
  dll_suf_str = new string{".dll"};
  pyc_suf_str = new string{".pyc"};
  libdir_pre_str = new string{"./lib/"};
}
void string_table::oops_do(oop_closure *f) {
  f->do_oop(reinterpret_cast<object *&>(next_str));
  f->do_oop(reinterpret_cast<object *&>(mod_str));
  f->do_oop(reinterpret_cast<object *&>(init_str));
  f->do_oop(reinterpret_cast<object *&>(add_str));
  f->do_oop(reinterpret_cast<object *&>(len_str));
  f->do_oop(reinterpret_cast<object *&>(call_str));
  f->do_oop(reinterpret_cast<object *&>(name_str));
  f->do_oop(reinterpret_cast<object *&>(iter_str));
  f->do_oop(reinterpret_cast<object *&>(repr_str));
  f->do_oop(reinterpret_cast<object *&>(getitem_str));
  f->do_oop(reinterpret_cast<object *&>(setitem_str));
  f->do_oop(reinterpret_cast<object *&>(setattr_str));
  f->do_oop(reinterpret_cast<object *&>(getattr_str));

  f->do_oop(reinterpret_cast<object *&>(empty_str));
  f->do_oop(reinterpret_cast<object *&>(dll_suf_str));
  f->do_oop(reinterpret_cast<object *&>(pyc_suf_str));
  f->do_oop(reinterpret_cast<object *&>(libdir_pre_str));
}
