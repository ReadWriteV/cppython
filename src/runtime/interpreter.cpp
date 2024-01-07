#include "runtime/interpreter.hpp"
#include "code/bytecode.hpp"
#include "code/code_object.hpp"
#include "object/dict.hpp"
#include "object/integer.hpp"
#include "object/list.hpp"
#include "object/object.hpp"
#include "object/tuple.hpp"
#include "runtime/cell.hpp"
#include "runtime/function.hpp"
#include "runtime/module.hpp"
#include "runtime/static_value.hpp"
#include "runtime/string_table.hpp"

#include <cassert>
#include <functional>
#include <optional>
#include <print>

using namespace cppython;

void interpreter::initialize() {
  builtins = std::make_shared<Module>(std::make_shared<dict>());

  // builtin values
  builtins->insert(std::make_shared<string>("True"), static_value::true_value);
  builtins->insert(std::make_shared<string>("False"),
                   static_value::false_value);
  builtins->insert(std::make_shared<string>("None"), static_value::none_value);

  // builtin functions
  builtins->insert(std::make_shared<string>("print"),
                   std::make_shared<function>(print));
  builtins->insert(std::make_shared<string>("len"),
                   std::make_shared<function>(len));
  builtins->insert(std::make_shared<string>("iter"),
                   std::make_shared<function>(iter));
  builtins->insert(std::make_shared<string>("type"),
                   std::make_shared<function>(type_of));
  builtins->insert(std::make_shared<string>("isinstance"),
                   std::make_shared<function>(isinstance));

  // builtin classes
  builtins->insert(std::make_shared<string>("object"),
                   object_klass::get_instance()->get_type_object());
  builtins->insert(std::make_shared<string>("int"),
                   integer_klass::get_instance()->get_type_object());
  builtins->insert(std::make_shared<string>("str"),
                   string_klass::get_instance()->get_type_object());
  builtins->insert(std::make_shared<string>("list"),
                   list_klass::get_instance()->get_type_object());
  builtins->insert(std::make_shared<string>("dict"),
                   dict_klass::get_instance()->get_type_object());

  builtins->extend(Module::import(std::make_shared<string>("builtin")));

  static_value::stop_iteration =
      builtins->get(std::make_shared<string>("StopIteration"));

  modules = std::make_shared<dict>();
  modules->insert(std::make_shared<string>("__builtins__"), builtins);
}

void interpreter::run(std::shared_ptr<code_object> codes) {
  cur_frame = std::make_shared<frame>(codes);

  cur_frame->get_locals()->insert(string_table::get_instance()->name_str,
                                  std::make_shared<string>("__main__"));

  eval_frame();
  destroy_frame();
}

void interpreter::eval_frame() {

  while (cur_frame->has_more_codes()) {
    auto op_code = cur_frame->get_op_code();
    bool has_argument =
        (op_code & 0xFF) >= std::to_underlying(bytecode::HAVE_ARGUMENT);

    int op_arg = -1;
    if (has_argument) {
      op_arg = cur_frame->get_op_arg();
    }

    switch (auto op = static_cast<bytecode>(op_code); op) {
      using enum bytecode;
    case NOP:
      break;

    case POP_TOP:
      pop_data();
      break;

    case ROT_TWO: {
      auto x = pop_data();
      auto y = pop_data();
      push_data(x);
      push_data(y);
      break;
    }
    case ROT_THREE: {
      auto x = pop_data();
      auto y = pop_data();
      auto z = pop_data();
      push_data(x);
      push_data(z);
      push_data(y);
      break;
    }
    case DUP_TOP: {
      push_data(top_data());
      break;
    }
    case DUP_TOP_TWO: {
      auto x = pop_data();
      auto y = pop_data();
      push_data(y);
      push_data(x);
      push_data(y);
      push_data(x);
      break;
    }
    case INPLACE_ADD:
    case BINARY_ADD: {
      auto v = pop_data();
      auto w = pop_data();
      push_data(w->add(v));
      break;
    }
    case INPLACE_SUBTRACT:
    case BINARY_SUBTRACT: {
      auto v = pop_data();
      auto w = pop_data();
      push_data(w->sub(v));
      break;
    }
    case INPLACE_MULTIPLY:
    case BINARY_MULTIPLY: {
      auto v = pop_data();
      auto w = pop_data();
      push_data(w->mul(v));
      break;
    }
    case INPLACE_DIVIDE:
    case BINARY_DIVIDE: {
      auto v = pop_data();
      auto w = pop_data();
      push_data(w->div(v));
      break;
    }
    case INPLACE_MODULO:
    case BINARY_MODULO: {
      auto v = pop_data();
      auto w = pop_data();
      push_data(w->mod(v));
      break;
    }
    case BINARY_SUBSCR: {
      auto v = pop_data();
      auto w = pop_data();
      push_data(w->subscr(v));
      break;
    }
    case STORE_MAP: {
      auto k = pop_data();
      auto v = pop_data();
      auto m = pop_data();
      std::static_pointer_cast<dict>(m)->insert(k, v);
      break;
    }

    case STORE_SUBSCR: {
      auto u = pop_data();
      auto v = pop_data();
      auto w = pop_data();
      v->store_subscr(u, w);
      break;
    }
    case DELETE_SUBSCR: {
      auto v = pop_data();
      auto w = pop_data();
      w->del_subscr(v);
      break;
    }

    case GET_ITER: {
      auto v = pop_data();
      push_data(v->iter());
      break;
    }

    case LOAD_BUILD_CLASS: {
      push_data(std::make_shared<function>(cppython::build_class));
      break;
    }

    case UNPACK_SEQUENCE: {
      auto v = pop_data();
      while (op_arg--) {
        push_data(v->subscr(std::make_shared<integer>(op_arg)));
      }
      break;
    }
    case FOR_ITER: {
      auto v = cur_frame->get_data_stack().top();

      auto w = v->getattr(string_table::get_instance()->next_str);

      build_frame(w, nullptr);

      if (cur_frame->get_data_stack().top() == nullptr) {
        cur_frame->set_pc(cur_frame->get_pc() + op_arg);
        cur_frame->get_data_stack().pop();
      }
      break;
    }
    case IS_OP: {
      auto v = pop_data();
      auto w = pop_data();
      if (v == w) {
        push_data(static_value::true_value);
      } else {
        push_data(static_value::false_value);
      };
      break;
    }

    case BREAK_LOOP:
      while (!cur_frame->get_data_stack().empty() &&
             cur_frame->get_data_stack().size() >
                 cur_frame->get_loop_stack().top().level) {
        cur_frame->get_data_stack().pop();
      }
      cur_frame->set_pc(cur_frame->get_loop_stack().top().target);
      cur_frame->get_loop_stack().pop();
      break;

    case LOAD_LOCALS:
      push_data(cur_frame->get_locals());
      break;

    case RETURN_VALUE: {
      ret_value = pop_data();
      if (cur_frame->is_first_frame() || cur_frame->is_entry_frame()) {
        return;
      }
      leave_frame();
      break;
    }

    case POP_BLOCK:
      while (!cur_frame->get_data_stack().empty() &&
             cur_frame->get_data_stack().size() >
                 cur_frame->get_loop_stack().top().level) {
        cur_frame->get_data_stack().pop();
      }
      cur_frame->get_loop_stack().pop();
      break;

    case STORE_NAME: {
      auto v = cur_frame->get_names()->at(op_arg);
      cur_frame->get_locals()->insert(v, pop_data());
      break;
    }
    case STORE_ATTR: {
      auto u = pop_data();
      auto v = cur_frame->get_names()->at(op_arg);
      auto w = pop_data();
      u->setattr(v, w);
      break;
    }
    case STORE_GLOBAL: {
      auto v = cur_frame->get_names()->at(op_arg);
      cur_frame->get_globals()->insert(v, pop_data());
      break;
    }

    case LOAD_CONST:
      push_data(cur_frame->get_consts()->at(op_arg));
      break;

    // Local, Enclosing, Global, Builtin
    case LOAD_NAME: {
      auto target_name = cur_frame->get_names()->at(op_arg);

      auto target_value = cur_frame->get_locals()
                              ->get(target_name, value_equal{})
                              .or_else([this, &target_name]() {
                                return cur_frame->get_globals()->get(
                                    target_name, value_equal{});
                              })
                              .value_or(builtins->get(target_name));
      push_data(target_value);
      break;
    }
    case BUILD_TUPLE: {
      std::vector<std::shared_ptr<object>> tmp;
      tmp.resize(op_arg);

      while (op_arg--) {
        tmp.at(op_arg) = pop_data();
      }
      push_data(std::make_shared<tuple>(std::move(tmp)));
      break;
    }
    case BUILD_LIST: {
      auto lst = std::make_shared<list>();
      lst->resize(op_arg);

      while (op_arg--) {
        lst->at(op_arg) = pop_data();
      }
      push_data(lst);
      break;
    }
    case BUILD_MAP: {
      auto v = std::make_shared<dict>();
      push_data(v);
      break;
    }

    case LOAD_ATTR: {
      auto v = pop_data();
      auto w = cur_frame->get_names()->at(op_arg);
      push_data(v->getattr(w));
      break;
    }
    case IMPORT_NAME: {
      pop_data();
      pop_data();
      auto v = cur_frame->get_names()->at(op_arg);
      auto w = modules->at(v);
      if (w == static_value::none_value) {
        w = Module::import(std::static_pointer_cast<string>(v));
        modules->insert(v, w);
      }
      push_data(w);
      break;
    }
    case IMPORT_FROM: {
      auto v = cur_frame->get_names()->at(op_arg);
      auto w = top_data();
      auto u = std::static_pointer_cast<Module>(w)->get(v);
      push_data(u);
      break;
    }

    case COMPARE_OP: {
      auto w = pop_data();
      auto v = pop_data();

      switch (auto cmp_flag = static_cast<compare>(op_arg); cmp_flag) {
        using enum compare;
      case less:
        push_data(v->less(w));
        break;
      case less_equal:
        push_data(v->le(w));
        break;
      case equal:
        push_data(v->equal(w));
        break;
      case not_equal:
        push_data(v->not_equal(w));
        break;
      case greater:
        push_data(v->greater(w));
        break;
      case greater_equal:
        push_data(v->ge(w));
        break;
      default:
        std::println("Error: Unrecognized compare op {:#4x}", op_arg);
      }
      break;
    }

    case JUMP_FORWARD:
      cur_frame->set_pc(cur_frame->get_pc() + op_arg);
      break;

    case JUMP_ABSOLUTE:
      cur_frame->set_pc(op_arg);
      break;

    case POP_JUMP_IF_FALSE: {
      auto v = pop_data();
      if (v == static_value::false_value) {
        cur_frame->set_pc(op_arg);
      }
      break;
    }

    case LOAD_GLOBAL: {
      auto target_name = cur_frame->get_names()->at(op_arg);

      auto target_value = cur_frame->get_globals()
                              ->get(target_name, value_equal{})
                              .value_or(builtins->get(target_name));
      push_data(target_value);

      break;
    }

    case CONTAINS_OP: {
      auto lst = pop_data();
      auto value = pop_data();
      push_data(lst->contains(value));
      break;
    }

    case LOAD_FAST:
      push_data(cur_frame->get_fast_locals()->at(op_arg));
      break;

    case STORE_FAST:
      cur_frame->get_fast_locals()->set_at(op_arg, pop_data());
      break;

    case CALL_FUNCTION: {
      std::shared_ptr<std::vector<std::shared_ptr<object>>> args;
      if (op_arg > 0) {
        args = std::make_shared<std::vector<std::shared_ptr<object>>>();
        args->resize(op_arg);
        int i{op_arg};
        while (i--) {
          args->at(i) = pop_data();
        }
      }
      auto func = pop_data();

      build_frame(func, args, op_arg);
      break;
    }

    case MAKE_FUNCTION: {
      //[Changed in version 3.11: Qualified name at STACK[-1] was removed.]
      pop_data(); // Qualified name

      auto v = pop_data();

      auto func = std::make_shared<function>(v);
      func->set_globals(cur_frame->get_globals());

      if (op_arg & 0x08) {
        // a tuple containing cells for free variables, making a closure
        auto free_args = pop_data();
        assert(free_args &&
               free_args->get_klass() == tuple_klass::get_instance());
        auto tpl_def_args = std::static_pointer_cast<tuple>(free_args);
        auto args = std::make_shared<list>(tpl_def_args->get_value());
        func->set_closure(args);
      }
      if (op_arg & 0x04) {
        // a tuple of strings containing parameters' annotations
        assert(false);
      }
      if (op_arg & 0x02) {
        // a dictionary of keyword-only parameters' default values
        assert(false);
      }
      if (op_arg & 0x01) {
        // a tuple of default values for positional-only and
        // positional-or-keyword parameters in positional order
        auto def_args = pop_data();
        assert(def_args &&
               def_args->get_klass() == tuple_klass::get_instance());
        auto tpl_def_args = std::static_pointer_cast<tuple>(def_args);
        auto args = std::make_shared<std::vector<std::shared_ptr<object>>>(
            tpl_def_args->get_value());
        func->set_default_args(args);
      }

      push_data(func);
      break;
    }

    case LOAD_CLOSURE: {
      auto v = cur_frame->get_closure()->at(op_arg);
      if (!v) {
        v = cur_frame->get_cell_from_parameter(op_arg);
        cur_frame->get_closure()->set(op_arg, v);
      }
      if (v->get_klass() == cell_klass::get_instance()) {
        push_data(v);
      } else {
        push_data(std::make_shared<cell>(cur_frame->get_closure(), op_arg));
      }
      break;
    }

    case LOAD_DEREF: {
      auto v = cur_frame->get_closure()->at(op_arg);
      if (v->get_klass() == cell_klass::get_instance()) {
        v = (std::static_pointer_cast<cell>(v))->value();
      }
      push_data(v);
      break;
    }

    case STORE_DEREF:
      cur_frame->get_closure()->set(op_arg, pop_data());
      break;

    case CALL_FUNCTION_KW: {
      auto args = std::make_shared<std::vector<std::shared_ptr<object>>>();
      if (op_arg > 0) {
        auto tpl = pop_data();
        assert(tpl && tpl->get_klass() == tuple_klass::get_instance());
        auto tpl_obj = std::static_pointer_cast<tuple>(tpl);
        const int kw_size = static_cast<int>(tpl_obj->size());
        int i{kw_size};
        auto kwargs = std::make_shared<dict>();
        while (i--) {
          kwargs->insert(tpl_obj->at(i), pop_data());
        }

        i = op_arg - static_cast<int>(kw_size);
        if (i > 0) {
          args->resize(i);
          while (i--) {
            args->at(i) = pop_data();
          }
        }

        args->push_back(kwargs);
      }
      auto func = pop_data();

      build_frame(func, args, op_arg, true);
      break;
    }

    case BUILD_CONST_KEY_MAP: {
      auto keys = pop_data();
      assert(keys && keys->get_klass() == tuple_klass::get_instance());
      auto tpl_obj = std::static_pointer_cast<tuple>(keys);
      int dict_size = static_cast<int>(tpl_obj->get_value().size());
      auto dict_obj = std::make_shared<dict>();
      while (dict_size--) {
        auto v = pop_data();
        dict_obj->insert(tpl_obj->at(dict_size), v);
      }
      push_data(dict_obj);
      break;
    }

    case LOAD_METHOD: {
      auto v = pop_data();
      auto w = cur_frame->get_names()->at(op_arg); // owner
      push_data(v->getattr(w));
      break;
    }

    case CALL_METHOD: {
      std::shared_ptr<std::vector<std::shared_ptr<object>>> args;
      if (op_arg > 0) {
        args = std::make_shared<std::vector<std::shared_ptr<object>>>();
        args->resize(op_arg);
        int i{op_arg};
        while (i--) {
          args->at(i) = pop_data();
        }
      }
      auto func = pop_data();

      build_frame(func, args, op_arg);
      break;
    }

    case LIST_EXTEND: {
      auto tpl = pop_data();
      assert(tpl && tpl->get_klass() == tuple_klass::get_instance());
      auto tpl_obj = std::static_pointer_cast<tuple>(tpl);

      auto lst = pop_data();
      assert(lst && lst->get_klass() == list_klass::get_instance());
      auto lst_obj = std::static_pointer_cast<list>(lst);

      for (const auto e : tpl_obj->get_value()) {
        lst_obj->append(e);
      }
      push_data(lst);
      break;
    }
    default:
      std::println("Error: Unrecognized byte code {:#04x}", op_code);
    }
  }
}

void interpreter::build_frame(
    std::shared_ptr<object> callable,
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args,
    int real_arg_cnt, bool has_kw_arg) {
  if (callable->get_klass() == native_function_klass::get_instance()) {
    auto native_func = std::static_pointer_cast<function>(callable);
    // prepare other data
    if (native_func->get_native_func() == cppython::build_class) {
      auto arg_0 = args->at(0); // func_code
      assert(arg_0 && arg_0->get_klass() == function_klass::get_instance());
      auto func_obj = std::static_pointer_cast<function>(arg_0);
      auto new_frame = std::make_shared<frame>(func_obj, nullptr);
      new_frame->set_entry_frame(true);
      enter_frame(new_frame);
      eval_frame();
      auto locals_dict = cur_frame->get_locals();
      destroy_frame();
      args->push_back(locals_dict);
    }
    push_data(native_func->call(args));

  } else if (callable->get_klass() == method_klass::get_instance()) {
    auto method_obj = std::static_pointer_cast<method>(callable);
    if (!args) {
      args = std::make_shared<std::vector<std::shared_ptr<object>>>();
    }
    args->insert(args->begin(), method_obj->get_owner());
    build_frame(method_obj->get_func(), args, real_arg_cnt + 1, has_kw_arg);
  } else if (callable->get_klass() == function_klass::get_instance()) {
    auto func = std::static_pointer_cast<function>(callable);
    auto new_frame =
        std::make_shared<frame>(func, args, real_arg_cnt, has_kw_arg);
    enter_frame(new_frame);
  } else if (callable->get_klass() == type_klass::get_instance()) {
    auto obj_type = std::static_pointer_cast<type>(callable);
    auto obj = obj_type->get_own_klass()->allocate_instance(callable, args);
    push_data(obj);
  } else {
    auto m = callable->get_klass_attr(string_table::get_instance()->call_str);
    if (m != static_value::none_value) {
      build_frame(m, args, real_arg_cnt, has_kw_arg);
    } else {
      std::println("Error : can not call a normal object <{}>.",
                   callable->to_string());
    }
  }
}

void interpreter::enter_frame(std::shared_ptr<frame> new_frame) {
  new_frame->set_caller(cur_frame);
  cur_frame = new_frame;
}

void interpreter::destroy_frame() { cur_frame = cur_frame->get_caller(); }

void interpreter::leave_frame() {
  destroy_frame();
  push_data(ret_value);
}

std::shared_ptr<object> interpreter::call_virtual(
    std::shared_ptr<object> func,
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  if (func->get_klass() == native_function_klass::get_instance()) {
    // we do not create a virtual frame, but native frame.
    return std::static_pointer_cast<function>(func)->call(args);
  } else if (func->get_klass() == method_klass::get_instance()) {
    auto method_obj = std::static_pointer_cast<method>(func);
    if (!args) {
      args = std::make_shared<std::vector<std::shared_ptr<object>>>();
    }
    args->insert(args->begin(), method_obj->get_owner());
    return call_virtual(method_obj->get_func(), args);
  } else if (method::is_function(func)) {
    auto func_obj = std::static_pointer_cast<function>(func);
    auto new_frame =
        std::make_shared<frame>(func_obj, args, static_cast<int>(args->size()));
    new_frame->set_entry_frame(true);
    enter_frame(new_frame);
    eval_frame();
    destroy_frame();
    return ret_value;
  }
  return static_value::none_value;
}

std::shared_ptr<dict>
interpreter::run_module(std::shared_ptr<code_object> codes,
                        std::shared_ptr<string> module_name) {
  auto module_frame = std::make_shared<frame>(codes);
  module_frame->set_entry_frame(true);
  module_frame->get_locals()->insert(string_table::get_instance()->name_str,
                                     module_name);

  enter_frame(module_frame);
  eval_frame();
  auto result = module_frame->get_locals();
  destroy_frame();
  return result;
}
