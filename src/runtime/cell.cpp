#include "runtime/cell.hpp"

// #include "memory/oopClosure.hpp"
#include "object/dict.hpp"
#include "object/string.hpp"

using namespace cppython;

cell_klass::cell_klass() {
  set_dict(std::make_shared<dict>());
  set_name("cell");
}

cell::cell(std::shared_ptr<list> t, int i) : table{t}, index{i} {
  set_klass(cell_klass::get_instance());
}

// void CellKlass::oops_do(OopClosure *closure, HiObject *obj) {
//   CellObject *co = (CellObject *)obj;
//   closure->do_oop((HiObject **)&co->_table);
// }
