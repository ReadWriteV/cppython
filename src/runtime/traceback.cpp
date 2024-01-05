#include "runtime/traceback.hpp"
#include "memory/oop_closure.hpp"
#include "object/list.hpp"
#include "object/string.hpp"
#include "runtime/frame.hpp"

#include <format>

using namespace cppython;

stack_element_klass *stack_element_klass::get_instance() {
  static stack_element_klass instance;
  return &instance;
}

std::string stack_element_klass::to_string(object *x) {
  auto stack_ele = x->as<stack_element>();
  return std::format(R"(  File "{}", line {}, in {})",
                     stack_ele->file_name->to_string(), stack_ele->line_no,
                     stack_ele->func_name->to_string());
}

size_t stack_element_klass::size() const { return sizeof(stack_element); }

void stack_element_klass::oops_do(oop_closure *f, object *obj) {
  auto stack_ele = obj->as<stack_element>();
  f->do_oop(reinterpret_cast<object *&>(stack_ele->file_name));
  f->do_oop(reinterpret_cast<object *&>(stack_ele->func_name));
}

stack_element::stack_element(string *file_name, string *func_name, int line_no)
    : file_name(file_name), func_name(func_name), line_no(line_no) {
  set_klass(stack_element_klass::get_instance());
}

traceback_klass *traceback_klass::get_instance() {
  static traceback_klass instance;
  return &instance;
}

std::string traceback_klass::to_string(object *x) {
  traceback *tbx = x->as<traceback>();
  std::string r{"Traceback (most recent call last):\n"};
  for (int i = tbx->get_stack_elements()->size() - 1; i >= 0; i--) {
    r += tbx->get_stack_elements()->at(i)->to_string() + '\n';
  }
  return r;
}

size_t traceback_klass::size() const { return sizeof(traceback); }

void traceback_klass::oops_do(oop_closure *f, object *obj) {
  auto tb_obj = obj->as<traceback>();
  f->do_oop(reinterpret_cast<object *&>(tb_obj->get_stack_elements()));
}

traceback::traceback() {
  stack_elements = new list();
  set_klass(traceback_klass::get_instance());
}

void traceback::record_frame(frame *frm) {
  stack_elements->append(new stack_element(
      frm->get_file_name(), frm->get_func_name(), frm->get_source_lineno()));
}
