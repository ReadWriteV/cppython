#pragma once

#include "object/list.hpp"
#include "runtime/frame.hpp"
#include "utils/singleton.hpp"

namespace cppython {

class code_object;
class object;
class dict;
class oop_closure;

class interpreter : public singleton<interpreter> {
  friend class singleton<interpreter>;

public:
  void run(code_object *codes);

  object *call_virtual(object *func, vector<object *> *args);

  void oops_do(oop_closure *f);

private:
  interpreter();

  auto top_data() { return cur_frame->get_data_stack()->back(); }
  void push_data(object *v) { cur_frame->get_data_stack()->append(v); }
  object *pop_data() { return cur_frame->get_data_stack()->pop(); }

  void build_frame(object *callable, vector<object *> *args,
                   int real_arg_cnt = 0, bool has_kw_arg = false);
  void enter_frame(frame *new_frame);
  void eval_frame();
  void destroy_frame();
  void leave_frame();

private:
  frame *cur_frame{nullptr};
  object *ret_value{nullptr};
  dict *builtins{nullptr};
};
} // namespace cppython
