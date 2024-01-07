#include "runtime/cell.hpp"

#include "object/dict.hpp"
#include "object/string.hpp"

using namespace cppython;

cell_klass::cell_klass() {
  set_name("cell");
  set_dict(std::make_shared<dict>());
}

cell::cell(std::shared_ptr<list> t, int i) : table{t}, index{i} {
  set_klass(cell_klass::get_instance());
}
