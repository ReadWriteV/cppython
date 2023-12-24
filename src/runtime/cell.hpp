#pragma once

#include "object/klass.hpp"
#include "object/list.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

#include <memory>

namespace cppython {

class object;
class list;

class cell_klass : public klass, public singleton<cell_klass> {
  friend class singleton<cell_klass>;

private:
  cell_klass();

public:
  //   virtual void oops_do(OopClosure *closure, HiObject *obj);
  virtual size_t size() { return sizeof(cell_klass); };
};

class cell : public object {
  //   friend class cell_klass;
public:
  cell(std::shared_ptr<list> t, int i);
  std::shared_ptr<object> value() {
    { return table->at(index); }
  }

private:
  std::shared_ptr<list> table;
  int index;
};

} // namespace cppython
