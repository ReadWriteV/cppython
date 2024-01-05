#include "runtime/frame.hpp"
#include "code/code_object.hpp"
#include "memory/oop_closure.hpp"
#include "object/dict.hpp"
#include "object/list.hpp"
#include "object/string.hpp"
#include "object/tuple.hpp"
#include "runtime/function.hpp"
#include "runtime/static_value.hpp"

using namespace cppython;

frame::frame(code_object *code) {
  codes = code;
  consts = code->consts;
  names = code->names;

  data_stack = new list{};

  locals = new dict{};

  globals = locals;
}

frame::frame(function *func, vector<object *> *args, int real_arg_cnt,
             bool has_kw_arg) {
  codes = func->get_func_code();
  consts = codes->consts;
  names = codes->names;

  data_stack = new list{};

  locals = new dict{};
  globals = func->get_globals();
  fast_locals = new list{};

  auto arg_cnt = codes->argcount;

  // default args
  if (auto def_args = func->get_default_args(); def_args) {
    int dft_num = static_cast<int>(func->get_default_args()->size());
    int arg_num = arg_cnt;
    while (dft_num--) {
      fast_locals->set_at(--arg_num, def_args->at(dft_num));
    }
  }

  auto var_args = new list{};
  auto kw_args = new dict{};

  dict *kw_dict;

  if (has_kw_arg) {
    kw_dict = args->back()->as<dict>();
    args->pop_back();
  }

  if (args && args->size() > 0) {
    // positional args
    for (size_t i{0};
         i < std::min({real_arg_cnt, arg_cnt, static_cast<int>(args->size())});
         i++) {
      fast_locals->set_at(i, args->at(i));
    }

    // extend positional args
    if (real_arg_cnt > arg_cnt) {
      for (size_t i{0}; i + arg_cnt < args->size(); i++) {
        var_args->set_at(i, args->at(i + arg_cnt));
      }
    }
  }

  if (has_kw_arg) {
    // keyword args
    for (size_t i{0}; i < kw_dict->size(); i++) {
      auto k = kw_dict->get_value()->get_key(i);
      auto v = kw_dict->get_value()->get_value(i);

      auto var_names = codes->varnames->as<tuple>();
      auto r = var_names->find(k, value_equal{});
      if (r) {
        fast_locals->set_at(r.value(), v);
      } else {
        kw_args->insert(k, v);
      }
    }
  }

  if (codes->flags & function::co_flags::var_args) {
    fast_locals->append(var_args);
  }
  if (codes->flags & function::co_flags::var_keywords) {
    fast_locals->append(kw_args);
  }

  auto cells = codes->cellvars;
  auto tpl_cells = cells->as<tuple>();

  if (tpl_cells->size() > 0) {
    closure = new list{};
    for (size_t i{0}; i < tpl_cells->size(); i++) {
      closure->append(nullptr);
    }
  }

  if (func->get_closure() && func->get_closure()->size() > 0) {
    if (!closure) {
      closure = func->get_closure();
    } else {
      closure = closure->add(func->get_closure())->as<list>();
    }
  }
}

int frame::get_op_arg() { return codes->code->at(pc++) & 0xFF; }

unsigned char frame::get_op_code() { return codes->code->at(pc++); }

bool frame::has_more_codes() const { return pc < codes->code->size(); }

string *frame::get_file_name() { return codes->filename->as<string>(); }
string *frame::get_func_name() { return codes->name->as<string>(); }
int frame::get_source_lineno() {
  int pc_offset = 0;
  int src_line_no = codes->firstlineno;

  auto lnotab = codes->lnotab->as<string>();
  int length = lnotab->size();

  for (int i = 0; i < length; i++) {
    pc_offset += lnotab->at(i++);
    if (pc_offset >= pc) {
      return src_line_no;
    }

    src_line_no += lnotab->at(i);
  }

  return src_line_no;
}

object *frame::get_cell_from_parameter(int i) {
  auto cells = codes->cellvars;
  auto tpl_cells = cells->as<tuple>();

  auto cell_name = tpl_cells->at(i);

  auto vars = codes->varnames;
  auto tpl_vars = vars->as<tuple>();

  auto r = tpl_vars->find(cell_name, value_equal{});
  assert(r);
  return fast_locals->at(r.value());
}

void cppython::frame::oops_do(oop_closure *f) {
  f->do_oop(reinterpret_cast<object *&>(consts));
  f->do_oop(reinterpret_cast<object *&>(names));

  f->do_oop(reinterpret_cast<object *&>(globals));
  f->do_oop(reinterpret_cast<object *&>(locals));

  f->do_oop(reinterpret_cast<object *&>(fast_locals));
  f->do_oop(reinterpret_cast<object *&>(closure));
  f->do_oop(reinterpret_cast<object *&>(data_stack));

  f->do_oop(reinterpret_cast<object *&>(codes));

  if (caller) {
    caller->oops_do(f);
  }
}
