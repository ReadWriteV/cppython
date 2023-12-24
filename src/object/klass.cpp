#include "object/klass.hpp"
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

void klass::add_super(klass *x) {
  if (super == nullptr) {
    super = std::make_shared<list>();
  }
  super->append(x->get_type_object());
}

std::shared_ptr<type> klass::get_super() {
  if (super == nullptr) {
    return nullptr;
  }

  if (super->empty()) {
    return nullptr;
  }

  return std::static_pointer_cast<type>(super->at(0));
}

void klass::order_supers() {
  if (super == nullptr) {
    return;
  }

  if (mro == nullptr) {
    mro = std::make_shared<list>();
  }

  int cur = -1;
  for (const auto &e : super->get_value()) {
    auto type_obj = std::static_pointer_cast<type>(e);
    auto k = type_obj->get_own_klass();
    mro->append(type_obj);
    if (k->get_mro() == nullptr) {
      continue;
    }

    for (const auto &ele : k->get_mro()->get_value()) {
      auto type_obj = std::static_pointer_cast<type>(ele);
      int index = mro->find(type_obj, std::equal_to{});
      if (index < cur) {
        std::println("Error: method resolution order conflicts.");
        assert(false);
      }
      cur = index;
      if (index >= 0) {
        mro->get_value().erase(mro->begin() + index);
      }
      mro->append(type_obj);
    }
  }

  if (mro == nullptr) {
    return;
  }

  // std::print("{}'s mro is ", name);
  // for (const auto &e : mro->get_value()) {
  //   auto type_obj = std::static_pointer_cast<type>(e);
  //   auto k = type_obj->get_own_klass();
  //   std::print("{}, ", k->get_name());
  // }
  // std::print("\n");
}

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

std::string klass::to_string(std::shared_ptr<object> obj) {
  return std::format("<{} object at {:p}>",
                     obj->get_klass()->get_type_object()->get_type_name(),
                     static_cast<void *>(obj.get()));
}

std::shared_ptr<object> klass::add(std::shared_ptr<object> x,
                                   std::shared_ptr<object> y) {
  auto args = std::make_shared<std::vector<std::shared_ptr<object>>>();
  args->push_back(y);
  return find_and_call(x, args, string_table::get_instance()->add_str);
}

std::shared_ptr<object> klass::subscr(std::shared_ptr<object> x,
                                      std::shared_ptr<object> y) {
  auto args = std::make_shared<std::vector<std::shared_ptr<object>>>();
  args->push_back(y);
  return find_and_call(x, args, string_table::get_instance()->getitem_str);
}
void klass::store_subscr(std::shared_ptr<object> x, std::shared_ptr<object> y,
                         std::shared_ptr<object> z) {
  auto args = std::make_shared<std::vector<std::shared_ptr<object>>>();
  args->push_back(y);
  args->push_back(z);
  find_and_call(x, args, string_table::get_instance()->setitem_str);
}

std::shared_ptr<object> klass::getattr(std::shared_ptr<object> x,
                                       std::shared_ptr<object> y) {

  auto func = find_in_parents(x, string_table::get_instance()->getattr_str);
  if (func->get_klass() == function_klass::get_instance()) {
    func =
        std::make_shared<method>(std::static_pointer_cast<function>(func), x);
    auto args = std::make_shared<std::vector<std::shared_ptr<object>>>();
    args->push_back(y);
    return interpreter::get_instance()->call_virtual(func, args);
  }

  if (x->get_obj_dict()) {
    auto result = x->get_obj_dict()->at(y);
    if (result != static_value::none_value) {
      return result;
    }
  }

  auto result = find_in_parents(x, y);

  if (method::is_function(result)) {
    result =
        std::make_shared<method>(std::static_pointer_cast<function>(result), x);
  }
  return result;
}

std::shared_ptr<object> klass::setattr(std::shared_ptr<object> x,
                                       std::shared_ptr<object> y,
                                       std::shared_ptr<object> z) {
  auto func =
      x->get_klass()->get_dict()->at(string_table::get_instance()->setattr_str);
  if (func->get_klass() == function_klass::get_instance()) {
    func =
        std::make_shared<method>(std::static_pointer_cast<function>(func), x);
    auto args = std::make_shared<std::vector<std::shared_ptr<object>>>();
    args->push_back(y);
    args->push_back(z);
    return interpreter::get_instance()->call_virtual(func, args);
  }

  if (x->get_klass() == type_klass::get_instance()) {
    auto type_obj = std::static_pointer_cast<type>(x);
    type_obj->get_own_klass()->get_dict()->insert(y, z);
    return static_value::none_value;
  }

  if (!x->get_obj_dict()) {
    x->set_obj_dict(std::make_shared<dict>());
  }

  x->get_obj_dict()->insert(y, z);
  return static_value::none_value;
}

std::shared_ptr<object> klass::len(std::shared_ptr<object> x) {
  return find_and_call(x, nullptr, string_table::get_instance()->len_str);
}

std::shared_ptr<object> klass::allocate_instance(
    std::shared_ptr<object> obj_type,
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  std::shared_ptr<object> inst;
  if (mro->has_pointer(integer_klass::get_instance()->get_type_object())) {
    inst = std::make_shared<integer>(0);
  } else if (mro->has_pointer(
                 string_klass::get_instance()->get_type_object())) {
    inst = std::make_shared<string>("");
  } else if (mro->has_pointer(list_klass::get_instance()->get_type_object())) {
    inst = std::make_shared<list>();
  } else if (mro->has_pointer(dict_klass::get_instance()->get_type_object())) {
    inst = std::make_shared<dict>();
  } else {
    inst = std::make_shared<object>();
  }

  assert(obj_type && obj_type->get_klass() == type_klass::get_instance());
  auto type_obj = std::static_pointer_cast<type>(obj_type);

  inst->set_klass(type_obj->get_own_klass());

  auto constructor = inst->getattr(string_table::get_instance()->init_str);
  if (constructor != static_value::none_value) {
    interpreter::get_instance()->call_virtual(constructor, args);
  }
  return inst;
}

std::shared_ptr<object>
klass::find_and_call(std::shared_ptr<object> x,
                     std::shared_ptr<std::vector<std::shared_ptr<object>>> args,
                     std::shared_ptr<string> func_name) {
  auto func = x->getattr(func_name);
  if (func != static_value::none_value) {
    return interpreter::get_instance()->call_virtual(func, args);
  }

  std::println("class {} Error : unsupport operation for class",
               x->get_klass()->get_name());
  assert(false);
  return static_value::none_value;
}

std::shared_ptr<object> klass::find_in_parents(std::shared_ptr<object> x,
                                               std::shared_ptr<object> y) {

  auto result = x->get_klass()->get_dict()->at(y);
  if (result != static_value::none_value) {
    return result;
  }

  // find attribute in all parents.
  if (x->get_klass()->get_mro() == nullptr) {
    return result;
  }

  for (const auto &e : x->get_klass()->get_mro()->get_value()) {
    auto type_obj = std::static_pointer_cast<type>(e);
    result = type_obj->get_own_klass()->get_dict()->at(y);
    if (result != static_value::none_value)
      break;
  }

  return result;
}