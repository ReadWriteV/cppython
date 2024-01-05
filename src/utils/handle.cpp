#include "utils/handle.hpp"
#include "memory/oop_closure.hpp"

using namespace cppython;

handle::handle(object *t) {
  value = t;
  next = handle_mark::get_instance()->get_head();
  handle_mark::get_instance()->set_head(this);
}

handle::~handle() {
  value = nullptr;
  handle_mark::get_instance()->set_head(next);
  next = nullptr;
}

void handle::oops_do(oop_closure *f) { f->do_oop(value); }

object *handle::operator->() { return value; }

object *handle::resolve() { return value; }

handle_mark *handle_mark::get_instance() {
  static handle_mark instance;
  return &instance;
}

void handle_mark::oops_do(oop_closure *f) {
  handle *cur = head;
  while (cur) {
    cur->oops_do(f);
    cur = cur->get_next();
  }
}
