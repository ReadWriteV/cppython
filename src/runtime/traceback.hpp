#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"

#include <string>

namespace cppython {

class list;
class string;
class frame;
class oop_closure;

class stack_element_klass : public klass {
public:
  static stack_element_klass *get_instance();

private:
  stack_element_klass() = default;
  ~stack_element_klass() = default;

public:
  stack_element_klass(const stack_element_klass &) = delete;
  stack_element_klass(stack_element_klass &&) = delete;
  stack_element_klass &operator=(const stack_element_klass &) = delete;
  stack_element_klass &operator=(stack_element_klass &&) = delete;

public:
  std::string to_string(object *x) override;
  size_t size() const override;
  void oops_do(oop_closure *f, object *obj) override;
};

class stack_element : public object {
public:
  using klass_type = stack_element_klass;

public:
  stack_element(string *fname, string *mname, int lineno);

public:
  string *file_name;
  string *func_name;
  int line_no;
};

class traceback_klass : public klass {
public:
  static traceback_klass *get_instance();

private:
  traceback_klass() = default;
  ~traceback_klass() = default;

public:
  traceback_klass(const traceback_klass &) = delete;
  traceback_klass(traceback_klass &&) = delete;
  traceback_klass &operator=(const traceback_klass &) = delete;
  traceback_klass &operator=(traceback_klass &&) = delete;

public:
  std::string to_string(object *x) override;
  size_t size() const override;
  void oops_do(oop_closure *f, object *obj) override;
};

class traceback : public object {
public:
  using klass_type = traceback_klass;

public:
  traceback();

  void record_frame(frame *frm);

  auto &get_stack_elements() { return stack_elements; }

private:
  list *stack_elements;
};

} // namespace cppython
