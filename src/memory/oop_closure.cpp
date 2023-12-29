#include "memory/oop_closure.hpp"
#include "memory/heap.hpp"
#include "object/object.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/static_value.hpp"
#include "runtime/string_table.hpp"
// #include "utils/handles.hpp"
#include "utils/map.hpp"
#include "utils/vector.hpp"

#include <cstring>

using namespace cppython;

scavenge_oop_closure::scavenge_oop_closure(space *from, space *to, space *meta)
    : from{from}, to{to}, meta{meta} {}

scavenge_oop_closure::~scavenge_oop_closure() {
  from = nullptr;
  to = nullptr;
  meta = nullptr;
}

void scavenge_oop_closure::do_oop(object *&oop) {
  if (oop == nullptr) {
    return;
  }

  // this oop has been handled, since it may be
  // refered by klass
  if (!from->has_obj(reinterpret_cast<char *>(oop))) {
    return;
  }

  oop = copy_and_push(oop);
}

void scavenge_oop_closure::do_vector(vector<klass *> *&alist) {
  if (alist == nullptr) {
    return;
  }

  // no chance to visit list more than once.
  assert(from->has_obj(reinterpret_cast<char *>(alist)));

  size_t size = sizeof(vector<klass *>);
  char *target = static_cast<char *>(to->allocate(size));
  std::memcpy(target, alist, size);

  reinterpret_cast<char *&>(alist) = target;

  alist->oops_do(this);
}

void scavenge_oop_closure::do_vector(vector<object *> *&alist) {
  if (alist == nullptr) {
    return;
  }

  assert(from->has_obj(reinterpret_cast<char *>(alist)));

  size_t size = sizeof(vector<object *>);
  char *target = static_cast<char *>(to->allocate(size));
  std::memcpy(target, alist, size);

  reinterpret_cast<char *&>(alist) = target;

  alist->oops_do(this);
}

void scavenge_oop_closure::do_map(map<object *, object *> *&amap) {
  if (amap == nullptr) {
    return;
  }

  assert(from->has_obj(reinterpret_cast<char *>(amap)));

  size_t size = sizeof(map<object *, object *>);
  char *target = static_cast<char *>(to->allocate(size));
  std::memcpy(target, amap, size);

  reinterpret_cast<char *&>(amap) = target;

  amap->oops_do(this);
}

void scavenge_oop_closure::do_raw_mem(char *&mem, size_t length) {
  if (mem == nullptr) {
    return;
  }
  char *target = static_cast<char *>(to->allocate(length));
  std::memcpy(target, mem, length);
  mem = target;
}

// we do not move klass
void scavenge_oop_closure::do_klass(klass *&k) {
  if (k == nullptr) {
    return;
  }
  k->oops_do(this);
}

object *scavenge_oop_closure::copy_and_push(object *obj) {
  char *target = obj->new_address();
  if (target) {
    return reinterpret_cast<object *>(target);
  }

  // copy
  size_t size = obj->size();
  target = static_cast<char *>(to->allocate(size));
  std::memcpy(target, obj, size);
  obj->set_new_address(target);

  // push
  oop_stack.push(reinterpret_cast<object *>(target));

  return reinterpret_cast<object *>(target);
}

void scavenge_oop_closure::scavenge() {
  // step 1, mark roots
  process_roots();

  // step 2, process all objects
  while (!oop_stack.empty()) {
    auto tmp = oop_stack.top();
    oop_stack.pop();
    tmp->oops_do(this);
  }
}

void scavenge_oop_closure::process_roots() {
  static_value::oops_do(this);
  interpreter::get_instance()->oops_do(this);
  // HandleMark::get_instance()->oops_do(this);
  string_table::get_instance()->oops_do(this);
}
