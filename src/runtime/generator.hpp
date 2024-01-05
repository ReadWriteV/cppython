#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"

namespace cppython {

class oop_closure;
class function;
class code_object;
class dict;
class list;

class frame;
class interpreter;

class block;

template <typename T>
class vector;

class generator_klass : public klass {
public:
  static generator_klass *get_instance();

private:
  generator_klass() = default;
  ~generator_klass() = default;

public:
  generator_klass(const generator_klass &) = delete;
  generator_klass(generator_klass &&) = delete;
  generator_klass &operator=(const generator_klass &) = delete;
  generator_klass &operator=(generator_klass &&) = delete;

public:
  object *next(object *obj) override;
  object *iter(object *obj) override;

  size_t size() const override;
  void oops_do(oop_closure *f, object *obj) override;
};

class generator : public object {
public:
  using klass_type = generator_klass;

public:
  generator(function *func, vector<object *> *args, int arg_cnt);

  frame *get_frame() const { return frm; }
  void set_frame(frame *x) { frm = x; }

private:
  frame *frm;
};

} // namespace cppython
