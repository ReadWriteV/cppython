#pragma once

#include "object/object.hpp"
#include "utils/singleton.hpp"

#include <memory>
#include <string>

namespace cppython {

class string;
class tuple;

class code_klass : public klass, public singleton<code_klass> {
  friend class singleton<code_klass>;

private:
  code_klass();

public:
  std::string to_string(std::shared_ptr<object> obj) override;
};

class code_object : public object {
public:
  code_object(int argcount, int posonlyargcount, int kwonlyargcount,
              int nlocals, int stacksize, int flags,
              std::shared_ptr<string> code, std::shared_ptr<tuple> consts,
              std::shared_ptr<tuple> names, std::shared_ptr<object> varnames,
              std::shared_ptr<object> freevars,
              std::shared_ptr<object> cellvars,
              std::shared_ptr<object> filename, std::shared_ptr<object> name,
              int firstlineno, std::shared_ptr<object> lnotabs);

  int stacksize;
  int argcount;
  int posonlyargcount;
  int kwonlyargcount;
  int nlocals;
  int flags;

  std::shared_ptr<string> code;

  std::shared_ptr<tuple> consts;
  std::shared_ptr<tuple> names;
  std::shared_ptr<object> varnames;
  std::shared_ptr<object> freevars;
  std::shared_ptr<object> cellvars;

  std::shared_ptr<object> filename;
  std::shared_ptr<object> name;

  int firstlineno;
  std::shared_ptr<object> lnotab;
};
} // namespace cppython
