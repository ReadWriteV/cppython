#pragma once

#include "object/klass.hpp"
#include "object/list.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

namespace cppython {

class object;
class list;

class cell_klass : public klass, public singleton<cell_klass> {
  friend class singleton<cell_klass>;

private:
  cell_klass();

public:
  //   virtual void oops_do(oop_closure *closure, object *obj);
  virtual size_t size() { return sizeof(cell_klass); };
};

class cell : public object {
public:
  using klass_type = cell_klass;

public:
  cell(list *t, int i);
  object *value() {
    { return table->at(index); }
  }

private:
  list *table;
  int index;
};

} // namespace cppython
