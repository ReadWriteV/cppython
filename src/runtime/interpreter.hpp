#pragma once

#include "object/list.hpp"
#include "runtime/frame.hpp"

namespace cppython {

class code_object;
class object;
class dict;
class oop_closure;
class module;
class generator;

class interpreter {
  enum class status { is_ok, is_exception, is_return, is_yield };

public:
  static interpreter *get_instance();

private:
  interpreter() = default;
  ~interpreter() = default;

public:
  interpreter(const interpreter &) = delete;
  interpreter(interpreter &&) = delete;
  interpreter &operator=(const interpreter &) = delete;
  interpreter &operator=(interpreter &&) = delete;

public:
  void initialize();

  void run(code_object *codes);
  dict *run_module(code_object *codes, string *module_name);

  object *call_virtual(object *func, vector<object *> *args);

  object *eval_generator(generator *g);

  void oops_do(oop_closure *f);

private:
  auto top_data() { return cur_frame->get_data_stack()->back(); }
  void push_data(object *v) { cur_frame->get_data_stack()->append(v); }
  object *pop_data() { return cur_frame->get_data_stack()->pop(); }

  void build_frame(object *callable, vector<object *> *args,
                   int real_arg_cnt = 0, bool has_kw_arg = false);
  void enter_frame(frame *new_frame);
  void eval_frame();
  void destroy_frame();
  void leave_frame();

  status do_raise(object *exc, object *val, object *tb);

private:
  frame *cur_frame{nullptr};
  object *ret_value{nullptr};
  module *builtins{nullptr};
  dict *modules{nullptr};

  object *exception_class{nullptr};
  object *pending_exception{nullptr};
  object *trace_back{nullptr};
  status cur_status{status::is_ok};
};
} // namespace cppython
