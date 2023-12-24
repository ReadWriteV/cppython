#include "code/code_object.hpp"

#include <cassert>

using namespace cppython;

std::string code_klass::to_string(std::shared_ptr<object> obj) {
  auto p = std::static_pointer_cast<code_object>(obj);
  assert(p && (p->get_klass() == this));
  return "code_object";
}

code_object::code_object(
    int argcount, int posonlyargcount, int kwonlyargcount, int nlocals,
    int stacksize, int flags, std::shared_ptr<string> code,
    std::shared_ptr<tuple> consts, std::shared_ptr<tuple> names,
    std::shared_ptr<object> varnames, std::shared_ptr<object> freevars,
    std::shared_ptr<object> cellvars, std::shared_ptr<object> filename,
    std::shared_ptr<object> name, int firstlineno,
    std::shared_ptr<object> lnotab)
    : argcount{argcount}, posonlyargcount{posonlyargcount},
      kwonlyargcount{kwonlyargcount}, nlocals{nlocals}, stacksize{stacksize},
      flags{flags}, code{std::move(code)}, consts{std::move(consts)},
      names{std::move(names)}, varnames{std::move(varnames)},
      freevars{std::move(freevars)}, cellvars{std::move(cellvars)},
      filename{std::move(filename)}, name{std::move(name)},
      firstlineno{firstlineno}, lnotab{std::move(lnotab)} {
  set_klass(code_klass::get_instance());
}
