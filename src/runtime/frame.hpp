#pragma once

#include <memory>
#include <stack>
#include <vector>

namespace cppython {

class object;
class list;
class tuple;
class dict;
class function;
class code_object;
class string;

struct loop_block {
  unsigned char type;
  size_t target;
  size_t level;
};

class frame {
public:
  frame(std::shared_ptr<code_object> code);
  frame(std::shared_ptr<function> func,
        std::shared_ptr<std::vector<std::shared_ptr<object>>> args,
        int real_arg_cnt = 0, bool has_kw_arg = false);

  void set_caller(std::shared_ptr<frame> x) { caller = x; }
  [[nodiscard]] auto get_caller() { return caller; }

  void set_pc(size_t x) { pc = x; }
  [[nodiscard]] auto get_pc() { return pc; }

  void set_entry_frame(bool x) { entry = x; }
  [[nodiscard]] bool is_entry_frame() { return entry; }
  [[nodiscard]] bool is_first_frame() { return caller == nullptr; }

  auto &get_data_stack() { return data_stack; }
  auto &get_loop_stack() { return loop_stack; }
  auto get_consts() { return consts; }
  auto &get_names() { return names; }
  auto &get_locals() { return locals; }
  auto &get_globals() { return globals; }
  auto &get_fast_locals() { return fast_locals; }
  auto &get_closure() { return closure; }
  std::shared_ptr<object> get_cell_from_parameter(int i);

  std::shared_ptr<string> get_file_name();
  std::shared_ptr<string> get_func_name();
  int get_source_lineno();

  bool has_more_codes() const;
  unsigned char get_op_code();
  int get_op_arg();

private:
  std::shared_ptr<frame> caller;
  bool entry{false};

  std::stack<std::shared_ptr<object>> data_stack;
  std::stack<loop_block> loop_stack;

  std::shared_ptr<code_object> codes;

  std::shared_ptr<tuple> consts;
  std::shared_ptr<tuple> names;

  std::shared_ptr<list> fast_locals;

  std::shared_ptr<list> closure;

  std::shared_ptr<dict> locals;
  std::shared_ptr<dict> globals;

  size_t pc{0};
};

} // namespace cppython
