#pragma once

#include <stack>

namespace cppython {

class object;
class klass;
class space;

template <typename T>
class vector;

template <typename K, typename V>
class map;

class oop_closure {
public:
  virtual void do_oop(object *&obj) = 0;

  virtual void do_vector(vector<klass *> *&alist) = 0;
  virtual void do_vector(vector<object *> *&alist) = 0;
  virtual void do_map(map<object *, object *> *&amap) = 0;
  virtual void do_raw_mem(char *&mem, size_t length) = 0;
  virtual void do_klass(klass *&k) = 0;
};

class scavenge_oop_closure : public oop_closure {
public:
  scavenge_oop_closure(space *from, space *to, space *meta);
  ~scavenge_oop_closure();

  virtual void do_oop(object *&oop) override;

  virtual void do_vector(vector<klass *> *&alist) override;
  virtual void do_vector(vector<object *> *&alist) override;
  virtual void do_map(map<object *, object *> *&amap) override;
  virtual void do_raw_mem(char *&mem, size_t length) override;
  // CAUTION : we do not move klass, because they locate at MetaSpace.
  virtual void do_klass(klass *&k) override;

  void scavenge();
  void process_roots();

private:
  object *copy_and_push(object *obj);

private:
  space *from;
  space *to;
  space *meta;

  std::stack<object *> oop_stack;
};

} // namespace cppython
