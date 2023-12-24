#pragma once

#include "object/object.hpp"
#include "object/string.hpp"
#include "object/tuple.hpp"
#include "utils/singleton.hpp"

#include <memory>
#include <string>

namespace cppython {

// struct PyCodeObject {
//     PyObject_HEAD
//     int co_argcount;            /* #arguments, except *args */
//     int co_posonlyargcount;     /* #positional only arguments */
//     int co_kwonlyargcount;      /* #keyword only arguments */
//     int co_nlocals;             /* #local variables */
//     int co_stacksize;           /* #entries needed for evaluation stack */
//     int co_flags;               /* CO_..., see below */
//     int co_firstlineno;         /* first source line number */
//     PyObject *co_code;          /* instruction opcodes */
//     PyObject *co_consts;        /* list (constants used) */
//     PyObject *co_names;         /* list of strings (names used) */
//     PyObject *co_varnames;      /* tuple of strings (local variable names) */
//     PyObject *co_freevars;      /* tuple of strings (free variable names) */
//     PyObject *co_cellvars;      /* tuple of strings (cell variable names) */
//     /* The rest aren't used in either hash or comparisons, except for
//     co_name,
//        used in both. This is done to preserve the name and line number
//        for tracebacks and debuggers; otherwise, constant de-duplication
//        would collapse identical functions/lambdas defined on different lines.
//     */
//     Py_ssize_t *co_cell2arg;    /* Maps cell vars which are arguments. */
//     PyObject *co_filename;      /* unicode (where it was loaded from) */
//     PyObject *co_name;          /* unicode (name, for reference) */
//     PyObject *co_lnotab;        /* string (encoding addr<->lineno mapping)
//     See
//                                    Objects/lnotab_notes.txt for details. */
//     void *co_zombieframe;       /* for optimization only (see frameobject.c)
//     */ PyObject *co_weakreflist;   /* to support weakrefs to code objects */
//     /* Scratch space for extra data relating to the code object.
//        Type is a void* to keep the format private in codeobject.c to force
//        people to go through the proper APIs. */
//     void *co_extra;

//     /* Per opcodes just-in-time cache
//      *
//      * To reduce cache size, we use indirect mapping from opcode index to
//      * cache object:
//      *   cache = co_opcache[co_opcache_map[next_instr - first_instr] - 1]
//      */

//     // co_opcache_map is indexed by (next_instr - first_instr).
//     //  * 0 means there is no cache for this opcode.
//     //  * n > 0 means there is cache in co_opcache[n-1].
//     unsigned char *co_opcache_map;
//     _PyOpcache *co_opcache;
//     int co_opcache_flag;  // used to determine when create a cache.
//     unsigned char co_opcache_size;  // length of co_opcache.
// };

class code_klass : public klass, public singleton<code_klass> {
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
