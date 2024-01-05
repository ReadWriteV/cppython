#pragma once

#include "object/klass.hpp"
#include "object/list.hpp"
#include "object/object.hpp"

namespace cppython {

class object;
class list;

class cell_klass : public klass {
public:
  static cell_klass *get_instance();

public:
  cell_klass(const cell_klass &) = delete;
  cell_klass(cell_klass &&) = delete;
  cell_klass &operator=(const cell_klass &) = delete;
  cell_klass &operator=(cell_klass &&) = delete;

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
