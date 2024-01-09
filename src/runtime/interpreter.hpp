#pragma once

#include "runtime/frame.hpp"
#include "utils/singleton.hpp"

#include <memory>
#include <unordered_map>

namespace cppython {

class code_object;
class object;
class dict;
class string;
class Module;
class Generator;

class interpreter : public singleton<interpreter> {
  enum class status { is_ok, is_exception, is_return, is_yield };

public:
  void initialize();

  void run(std::shared_ptr<code_object> codes);
  std::shared_ptr<object>
  call_virtual(std::shared_ptr<object> callable,
               std::shared_ptr<std::vector<std::shared_ptr<object>>> args);
  std::shared_ptr<dict> run_module(std::shared_ptr<code_object> codes,
                                   std::shared_ptr<string> module_name);
  std::shared_ptr<object> eval_generator(std::shared_ptr<Generator> g);

private:
  auto top_data() { return cur_frame->get_data_stack().top(); }
  void push_data(const std::shared_ptr<object> &v) {
    cur_frame->get_data_stack().push(v);
  }
  std::shared_ptr<object> pop_data() {
    auto r = cur_frame->get_data_stack().top();
    cur_frame->get_data_stack().pop();
    return r;
  }

  void build_frame(std::shared_ptr<object> callable,
                   std::shared_ptr<std::vector<std::shared_ptr<object>>> args,
                   int real_arg_cnt = 0, bool has_kw_arg = false);
  void enter_frame(std::shared_ptr<frame> new_frame);
  void eval_frame();
  void destroy_frame();
  void leave_frame();

  status do_raise(std::shared_ptr<object> exc, std::shared_ptr<object> val,
                  std::shared_ptr<object> tb);

private:
  std::shared_ptr<frame> cur_frame;
  std::shared_ptr<object> ret_value;

  std::shared_ptr<Module> builtins;
  std::shared_ptr<dict> modules;

  std::shared_ptr<object> exception_class;
  std::shared_ptr<object> pending_exception;
  std::shared_ptr<object> trace_back;
  status cur_status{status::is_ok};
};
} // namespace cppython
