#pragma once

#include <stack>

namespace cppython {

class object;
class list;
class tuple;
class dict;
class function;
class code_object;
class oop_closure;
class string;

template <typename T>
class vector;

struct block {
  unsigned char type;
  size_t target;
  size_t level;
};

class frame {
public:
  frame(code_object *code);
  frame(function *func, vector<object *> *args, int real_arg_cnt = 0,
        bool has_kw_arg = false);

  void set_caller(frame *x) { caller = x; }
  [[nodiscard]] auto get_caller() { return caller; }

  void set_pc(size_t x) { pc = x; }
  [[nodiscard]] auto get_pc() { return pc; }

  void set_entry_frame(bool x) { entry = x; }
  [[nodiscard]] bool is_entry_frame() { return entry; }
  [[nodiscard]] bool is_first_frame() { return caller == nullptr; }

  auto get_data_stack() { return data_stack; }
  auto &get_loop_stack() { return loop_stack; }
  auto get_consts() { return consts; }
  auto get_names() { return names; }
  auto get_locals() { return locals; }
  auto get_globals() { return globals; }
  auto get_fast_locals() { return fast_locals; }
  auto get_closure() { return closure; }

  string *get_file_name();
  string *get_func_name();
  int get_source_lineno();

  object *get_cell_from_parameter(int i);

  bool has_more_codes() const;
  unsigned char get_op_code();
  int get_op_arg();

  void oops_do(oop_closure *f);

private:
  frame *caller{nullptr};
  bool entry{false};

  list *data_stack{nullptr};
  std::stack<block> loop_stack;

  code_object *codes{nullptr};

  tuple *consts{nullptr};
  tuple *names{nullptr};

  list *fast_locals{nullptr};

  list *closure{nullptr};

  dict *locals{nullptr};
  dict *globals{nullptr};

  size_t pc{0};
};

} // namespace cppython
