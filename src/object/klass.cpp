#include "object/klass.hpp"
#include "memory/oop_closure.hpp"
#include "object/dict.hpp"
#include "object/integer.hpp"
#include "object/list.hpp"
#include "runtime/function.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/static_value.hpp"
#include "runtime/string_table.hpp"

#include <cassert>
#include <format>
#include <print>

using namespace cppython;

klass::klass() { static_value::klasses->push_back(this); }

void klass::add_super(klass *x) {
  if (super == nullptr) {
    super = new list{};
  }
  super->append(x->get_type_object());
}

type *klass::get_super() {
  if (super == nullptr || super->empty()) {
    return nullptr;
  }
  return super->at(0)->as<type>();
}

void klass::order_supers() {
  if (super == nullptr) {
    return;
  }

  if (mro == nullptr) {
    mro = new list{};
  }

  int cur = -1;
  auto &&super_lst = super->get_value();
  for (size_t i{0}; i < super_lst->size(); i++) {
    auto type_obj = super_lst->at(i)->as<type>();
    auto k = type_obj->get_own_klass();
    mro->append(type_obj);
    if (k->get_mro() == nullptr) {
      continue;
    }

    auto &&mro_lst = k->get_mro()->get_value();
    for (size_t j{0}; j < mro_lst->size(); j++) {
      auto type_obj = mro_lst->at(j)->as<type>();
      auto index = mro->find(type_obj, std::equal_to{});
      auto idx = index.transform([](size_t i) { return static_cast<int>(i); })
                     .value_or(-1);
      if (idx < cur) {
        std::println("Error: method resolution order conflicts.");
        assert(false);
      }
      cur = idx;
      if (idx >= 0) {
        mro_lst->erase(idx);
      }
      mro->append(type_obj);
    }
  }

  if (mro == nullptr) {
    return;
  }

  // std::print("{}'s mro is ", name);
  // auto &&mro_lst = mro->get_value();
  // for (size_t i{0}; i < mro_lst->size(); i++) {
  //   auto type_obj = mro_lst->at(i)->as<type>();
  //   auto k = type_obj->get_own_klass();
  //   std::print("{}, ", k->get_name());
  // }
  // std::print("\n");
}

void cppython::klass::set_name(std::string_view x) { name = new string{x}; }

std::weak_ordering klass::compare(klass *x, klass *y) {
  if (x == y) {
    return std::weak_ordering::equivalent;
  }

  if (x == integer_klass::get_instance()) {
    return std::weak_ordering::less;
  } else if (y == integer_klass::get_instance()) {
    return std::weak_ordering::greater;
  }

  return x->get_name() <=> y->get_name();
}

std::string klass::to_string(object *obj) {

  auto repr_method =
      get_klass_attr(obj, string_table::get_instance()->repr_str);
  if (repr_method != static_value::none_value) {
    return interpreter::get_instance()
        ->call_virtual(repr_method, nullptr)
        ->to_string();
  }

  return std::format("<{} object at {:p}>",
                     obj->get_klass()->get_type_object()->get_type_name(),
                     static_cast<void *>(obj));
}

object *klass::add(object *x, object *y) {
  auto args = new vector<object *>{};
  args->push_back(y);
  return find_and_call(x, args, string_table::get_instance()->add_str);
}

object *klass::subscr(object *x, object *y) {
  auto args = new vector<object *>{};
  args->push_back(y);
  return find_and_call(x, args, string_table::get_instance()->getitem_str);
}
void klass::store_subscr(object *x, object *y, object *z) {
  auto args = new vector<object *>{};
  args->push_back(y);
  args->push_back(z);
  find_and_call(x, args, string_table::get_instance()->setitem_str);
}

object *klass::getattr(object *x, object *y) {
  auto func = find_in_parents(x, string_table::get_instance()->getattr_str);
  if (func->is<function>()) {
    func = new method{func->as<function>(), x};
    auto args = new vector<object *>{};
    args->push_back(y);
    return interpreter::get_instance()->call_virtual(func, args);
  }

  if (x->get_obj_dict()) {
    auto result = x->get_obj_dict()->at(y);
    if (result != static_value::none_value) {
      return result;
    }
  }

  return get_klass_attr(x, y);
}

object *klass::setattr(object *x, object *y, object *z) {
  auto func =
      x->get_klass()->get_dict()->at(string_table::get_instance()->setattr_str);
  if (func->is<function>()) {
    func = new method{func->as<function>(), x};
    auto args = new vector<object *>{};
    args->push_back(y);
    args->push_back(z);
    return interpreter::get_instance()->call_virtual(func, args);
  }

  if (x->is<type>()) {
    auto type_obj = x->as<type>();
    type_obj->get_own_klass()->get_dict()->insert(y, z);
    return static_value::none_value;
  }

  if (!x->get_obj_dict()) {
    x->set_obj_dict(new dict{});
  }

  x->get_obj_dict()->insert(y, z);
  return static_value::none_value;
}

object *klass::get_klass_attr(object *x, object *y) {
  auto result = find_in_parents(x, y);
  if (method::is_function(result)) {
    // TODO: result maybe a native function, we will check it again, and cast it
    // to native_function when invoke this method in build_frame
    result = new method{static_cast<function *>(result), x};
  }
  return result;
}

object *klass::iter(object *x) {
  return find_and_call(x, nullptr, string_table::get_instance()->iter_str);
}

object *klass::next(object *x) {
  return find_and_call(x, nullptr, string_table::get_instance()->next_str);
}

object *klass::len(object *x) {
  return find_and_call(x, nullptr, string_table::get_instance()->len_str);
}

object *klass::allocate_instance(object *obj_type, vector<object *> *args) {
  object *inst;
  if (mro->find(integer_klass::get_instance()->get_type_object(),
                std::equal_to{})) {
    inst = new integer{0};
  } else if (mro->find(string_klass::get_instance()->get_type_object(),
                       std::equal_to{})) {
    inst = new string{""};
  } else if (mro->find(list_klass::get_instance()->get_type_object(),
                       std::equal_to{})) {
    inst = new list{};
  } else if (mro->find(dict_klass::get_instance()->get_type_object(),
                       std::equal_to{})) {
    inst = new dict{};
  } else {
    inst = new object{};
  }

  auto type_obj = obj_type->as<type>();
  inst->set_klass(type_obj->get_own_klass());

  auto constructor =
      inst->get_klass_attr(string_table::get_instance()->init_str);
  if (constructor != static_value::none_value) {
    interpreter::get_instance()->call_virtual(constructor, args);
  }
  return inst;
}

void *klass::operator new(size_t size) {
  return static_value::allocate_meta(size);
}

void klass::oops_do(oop_closure *closure, object *obj) {
  std::println("warning: klass oops_do for {}", name->to_string());
}

void klass::oops_do(oop_closure *closure) {
  closure->do_oop(reinterpret_cast<object *&>(super));
  closure->do_oop(reinterpret_cast<object *&>(mro));
  closure->do_oop(reinterpret_cast<object *&>(name));
  closure->do_oop(reinterpret_cast<object *&>(map));
  closure->do_oop(reinterpret_cast<object *&>(type_object));
}

size_t klass::size() const { return sizeof(object); }

object *klass::find_and_call(object *x, vector<object *> *args,
                             string *func_name) {
  auto func = x->get_klass_attr(func_name);
  if (func != static_value::none_value) {
    return interpreter::get_instance()->call_virtual(func, args);
  }

  std::println("class {} Error : unsupport operation for class",
               x->get_klass()->get_name()->to_string());
  assert(false);
  return static_value::none_value;
}

object *klass::find_in_parents(object *x, object *y) {

  auto result = x->get_klass()->get_dict()->at(y);
  if (result != static_value::none_value) {
    return result;
  }

  // find attribute in all parents.
  if (x->get_klass()->get_mro() == nullptr) {
    return result;
  }

  auto &&mro_lst = x->get_klass()->get_mro()->get_value();
  for (size_t i{0}; i < mro_lst->size(); i++) {
    auto type_obj = mro_lst->at(i)->as<type>();
    result = type_obj->get_own_klass()->get_dict()->at(y);
    if (result != static_value::none_value)
      break;
  }

  return result;
}