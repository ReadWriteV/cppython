#include "runtime/frame.hpp"
#include "code/code_object.hpp"
#include "object/dict.hpp"
#include "object/list.hpp"
#include "object/string.hpp"
#include "object/tuple.hpp"
#include "runtime/function.hpp"
#include "runtime/static_value.hpp"

#include <algorithm>
#include <cassert>

using namespace cppython;

frame::frame(std::shared_ptr<code_object> code) {
  codes = code;
  consts = code->consts;
  names = code->names;

  locals = std::make_shared<dict>();

  globals = locals;
}

frame::frame(std::shared_ptr<function> func,
             std::shared_ptr<std::vector<std::shared_ptr<object>>> args,
             int real_arg_cnt, bool has_kw_arg) {
  codes = func->get_func_code();
  consts = codes->consts;
  names = codes->names;

  locals = std::make_shared<dict>();
  globals = func->get_globals();
  fast_locals = std::make_shared<list>();

  auto arg_cnt = codes->argcount;

  fast_locals->resize(arg_cnt);

  // default args
  if (auto def_args = func->get_default_args(); def_args) {
    std::copy(def_args->rbegin(), def_args->rend(), fast_locals->rbegin());
  }

  auto var_args = std::make_shared<list>();
  auto kw_args = std::make_shared<dict>();

  std::shared_ptr<dict> kw_dict;

  if (has_kw_arg) {
    kw_dict = std::static_pointer_cast<dict>(args->back());
    args->pop_back();
  }

  if (args && args->size() > 0) {
    // positional args
    std::copy_n(
        args->begin(),
        std::min({real_arg_cnt, arg_cnt, static_cast<int>(args->size())}),
        fast_locals->begin());

    // extend positional args
    if (real_arg_cnt > arg_cnt) {
      var_args->resize(real_arg_cnt - arg_cnt);
      std::copy(args->begin() + arg_cnt, args->end(),
                var_args->get_value().begin());
    }
  }

  if (has_kw_arg) {
    // keyword args
    for (const auto &[k, v] : kw_dict->get_value()) {
      assert(codes->varnames &&
             codes->varnames->get_klass() == tuple_klass::get_instance());
      auto var_names = std::static_pointer_cast<tuple>(codes->varnames);
      auto iter = std::ranges::find_if(
          var_names->get_value(), [k](const std::shared_ptr<object> &x) {
            return x->equal(k) == static_value::true_value;
          });
      if (iter != var_names->get_value().end()) {
        auto index = std::distance(var_names->get_value().begin(), iter);
        fast_locals->at(index) = v;
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
  assert(cells && cells->get_klass() == tuple_klass::get_instance());
  auto tpl_cells = std::static_pointer_cast<tuple>(cells);

  if (tpl_cells->size() > 0) {
    closure = std::make_shared<list>();
    closure->resize(tpl_cells->size(), nullptr);
  }

  if (func->get_closure() && func->get_closure()->size() > 0) {
    if (!closure) {
      closure = func->get_closure();
    } else {
      closure =
          std::static_pointer_cast<list>(closure->add(func->get_closure()));
    }
  }
}

int frame::get_op_arg() { return codes->code->at(pc++) & 0xFF; }

unsigned char frame::get_op_code() { return codes->code->at(pc++); }

bool frame::has_more_codes() const { return pc < codes->code->size(); }

std::shared_ptr<object> frame::get_cell_from_parameter(int i) {
  auto cells = codes->cellvars;
  assert(cells && cells->get_klass() == tuple_klass::get_instance());
  auto tpl_cells = std::static_pointer_cast<tuple>(cells);

  auto cell_name = tpl_cells->at(i);

  auto vars = codes->varnames;
  assert(vars && vars->get_klass() == tuple_klass::get_instance());
  auto tpl_vars = std::static_pointer_cast<tuple>(vars);

  auto iter = std::ranges::find_if(tpl_vars->get_value(),
                                   std::bind_back(value_equal{}, cell_name));
  assert(iter != tpl_vars->get_value().end());
  return fast_locals->at(std::distance(tpl_vars->get_value().begin(), iter));
}

std::shared_ptr<string> frame::get_file_name() {
  return std::static_pointer_cast<string>(codes->filename);
}

std::shared_ptr<string> frame::get_func_name() {
  return std::static_pointer_cast<string>(codes->name);
}

int frame::get_source_lineno() {
  int pc_offset = 0;
  int src_line_no = codes->firstlineno;

  auto lnotab = std::static_pointer_cast<string>(codes->lnotab);
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
