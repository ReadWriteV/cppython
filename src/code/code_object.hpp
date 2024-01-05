#pragma once

#include "object/object.hpp"

#include <string>

namespace cppython {

class string;
class tuple;
class oop_closure;

class code_klass : public klass {
public:
  static code_klass *get_instance();

public:
  code_klass(const code_klass &) = delete;
  code_klass(code_klass &&) = delete;
  code_klass &operator=(const code_klass &) = delete;
  code_klass &operator=(code_klass &&) = delete;

private:
  code_klass();

public:
  std::string to_string(object *obj) override;

  void oops_do(oop_closure *closure, object *obj) override;
  size_t size() const override;
};

class code_object : public object {
public:
  using klass_type = code_klass;

public:
  code_object(int argcount, int posonlyargcount, int kwonlyargcount,
              int nlocals, int stacksize, int flags, string *code,
              tuple *consts, tuple *names, object *varnames, object *freevars,
              object *cellvars, object *filename, object *name, int firstlineno,
              object *lnotabs);

  int stacksize;
  int argcount;
  int posonlyargcount;
  int kwonlyargcount;
  int nlocals;
  int flags;

  string *code;

  tuple *consts;
  tuple *names;
  object *varnames;
  object *freevars;
  object *cellvars;

  object *filename;
  object *name;

  int firstlineno;
  object *lnotab;
};
} // namespace cppython
