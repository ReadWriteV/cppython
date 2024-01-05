#include "runtime/cell.hpp"

// #include "memory/oopClosure.hpp"
#include "object/dict.hpp"
#include "object/string.hpp"

using namespace cppython;

cell_klass *cell_klass::get_instance() {
  static cell_klass instance;
  return &instance;
}

cell_klass::cell_klass() {
  set_dict(new dict{});
  set_name("cell");
}

cell::cell(list *t, int i) : table{t}, index{i} {
  set_klass(cell_klass::get_instance());
}

// void CellKlass::oops_do(oop_closure *closure, object *obj) {
//   CellObject *co = (CellObject *)obj;
//   closure->do_oop((object **)&co->_table);
// }
