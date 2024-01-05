#include "code/code_object.hpp"
#include "memory/oop_closure.hpp"

using namespace cppython;

code_klass *code_klass::get_instance() {
  static code_klass instance;
  return &instance;
}

code_klass::code_klass() {
  set_name("code");
  add_super(object_klass::get_instance());
  set_type_object(new type{});
}

std::string code_klass::to_string(object *obj) {
  auto p = obj->as<code_object>();
  return "code_object";
}

void code_klass::oops_do(oop_closure *closure, object *obj) {
  auto co = obj->as<code_object>();

  closure->do_oop(reinterpret_cast<object *&>(co->code));
  closure->do_oop(reinterpret_cast<object *&>(co->names));
  closure->do_oop(reinterpret_cast<object *&>(co->consts));
  closure->do_oop(co->varnames);
  closure->do_oop(co->freevars);
  closure->do_oop(co->cellvars);
  closure->do_oop(co->name);
  closure->do_oop(co->filename);
  closure->do_oop(co->lnotab);
}

size_t code_klass::size() const { return sizeof(code_object); }

code_object::code_object(int argcount, int posonlyargcount, int kwonlyargcount,
                         int nlocals, int stacksize, int flags, string *code,
                         tuple *consts, tuple *names, object *varnames,
                         object *freevars, object *cellvars, object *filename,
                         object *name, int firstlineno, object *lnotab)
    : argcount{argcount}, posonlyargcount{posonlyargcount},
      kwonlyargcount{kwonlyargcount}, nlocals{nlocals}, stacksize{stacksize},
      flags{flags}, code{code}, consts{consts}, names{names},
      varnames{varnames}, freevars{freevars}, cellvars{cellvars},
      filename{filename}, name{name}, firstlineno{firstlineno}, lnotab{lnotab} {
  set_klass(code_klass::get_instance());
}
