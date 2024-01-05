#include "runtime/generator.hpp"
#include "code/code_object.hpp"
#include "memory/oop_closure.hpp"
#include "object/dict.hpp"
#include "object/list.hpp"
#include "runtime/frame.hpp"
#include "runtime/function.hpp"
#include "runtime/interpreter.hpp"

using namespace cppython;

generator_klass *generator_klass::get_instance() {
  static generator_klass instance;
  return &instance;
}

object *generator_klass::iter(object *obj) { return obj; }

object *generator_klass::next(object *obj) {
  generator *g = obj->as<generator>();
  return interpreter::get_instance()->eval_generator(g);
}

size_t generator_klass::size() const { return sizeof(generator); }

void generator_klass::oops_do(oop_closure *f, object *obj) {
  auto g = obj->as<generator>();
  if (g->get_frame()) {
    g->get_frame()->oops_do(f);
  }
}

generator::generator(function *func, vector<object *> *args, int arg_cnt) {
  frm = new frame{func, args, arg_cnt};
  set_klass(generator_klass::get_instance());
}
