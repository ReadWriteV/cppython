#include "object/list.hpp"
#include "memory/oop_closure.hpp"
#include "object/dict.hpp"
#include "object/integer.hpp"
#include "object/string.hpp"
#include "runtime/function.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/static_value.hpp"
#include "runtime/string_table.hpp"

#include <algorithm>

using namespace cppython;

list_klass *list_klass::get_instance() {
  static list_klass instance;
  return &instance;
}

void list_klass::initialize() {
  auto map = new dict{};
  map->insert(new string{"append"}, new native_function{list::list_append});
  map->insert(new string{"index"}, new native_function{list::list_index});
  map->insert(new string{"pop"}, new native_function{list::list_pop});
  map->insert(new string{"remove"}, new native_function{list::list_remove});
  map->insert(new string{"reverse"}, new native_function{list::list_reverse});
  map->insert(new string{"sort"}, new native_function{list::list_sort});
  map->insert(new string{"extend"}, new native_function{list::list_extend});
  map->insert(new string{"__getitem__"},
              new native_function{list::list_getitem});

  set_dict(map);

  set_name("list");
  (new type{})->set_own_klass(this);

  add_super(object_klass::get_instance());
}

std::string list_klass::to_string(object *x) {
  auto list_obj = x->as<list>();

  std::string result;

  result += "[";

  auto fmt_str = [&list_obj](size_t idx) {
    if (list_obj->at(idx)->is<string>()) {
      return "'" + list_obj->at(idx)->to_string() + "'";
    } else {
      return list_obj->at(idx)->to_string();
    }
  };

  if (!list_obj->empty()) {
    result += fmt_str(0);
  }

  for (size_t i{1}; i < list_obj->size(); ++i) {
    result += ", ";
    result += fmt_str(i);
  }

  result += "]";
  return result;
}

object *list_klass::less(object *x, object *y) {
  auto p = x->as<list>();

  // check y is list or not
  if (!y->is<list>()) {
    return static_value::get_bool_value(
        std::is_lt(klass::compare(x->get_klass(), y->get_klass())));
  }

  auto q = y->as<list>();

  auto &&lst1 = p->get_value();
  auto &&lst2 = q->get_value();

  auto r = std::lexicographical_compare(
      lst1->data(), lst1->data() + lst1->size(), lst2->data(),
      lst2->data() + lst2->size(), value_less{});

  return static_value::get_bool_value(r);
}

object *list_klass::add(object *x, object *y) {
  auto list_obj_x = x->as<list>();
  auto list_obj_y = y->as<list>();

  auto &&lst_x = list_obj_x->get_value();
  auto &&lst_y = list_obj_y->get_value();

  auto lst = new list{};
  for (size_t i{0}; i < lst_x->size(); i++) {
    lst->get_value()->push_back(lst_x->at(i));
  }
  for (size_t i{0}; i < lst_y->size(); i++) {
    lst->get_value()->push_back(lst_y->at(i));
  }
  return lst;
}

object *list_klass::mul(object *x, object *y) {

  auto list_obj_x = x->as<list>();
  auto int_obj_y = y->as<integer>();

  auto &&lst_x = list_obj_x->get_value();
  auto &&n = int_obj_y->get_value();

  auto lst = new list{};
  for (size_t i{0}; i < n; i++) {
    for (size_t j{0}; j < lst_x->size(); j++) {
      lst->get_value()->push_back(lst_x->at(j));
    }
  }
  return lst;
}

object *list_klass::subscr(object *x, object *y) {
  auto list_obj = x->as<list>();
  auto index_obj = y->as<integer>();
  return list_obj->at(index_obj->get_value());
}

void list_klass::store_subscr(object *x, object *y, object *z) {
  auto list_obj = x->as<list>();
  auto index_obj = y->as<integer>();
  list_obj->set_at(index_obj->get_value(), z);
}

void list_klass::del_subscr(object *x, object *y) {
  auto list_obj = x->as<list>();
  auto index_obj = y->as<integer>();

  list_obj->get_value()->erase(index_obj->get_value());
}

object *list_klass::contains(object *x, object *y) {
  auto list_obj = x->as<list>();
  auto &&lst = list_obj->get_value();
  return static_value::get_bool_value(lst->index(y, value_equal{}).has_value());
}

object *list_klass::iter(object *x) {
  auto list_obj = x->as<list>();
  return new list_iterator{list_obj};
}

object *list_klass::len(object *x) {
  auto list_obj = x->as<list>();
  return new integer{static_cast<int>(list_obj->size())};
}

object *list_klass::allocate_instance(object *obj_type,
                                      vector<object *> *args) {
  if (!args || args->size() == 0) {
    return new list{};
  } else {
    return nullptr;
  }
}

void list_klass::oops_do(oop_closure *closure, object *obj) {
  auto list_obj = obj->as<list>();
  closure->do_vector(list_obj->data_address());
}

size_t list_klass::size() const { return sizeof(list); }

object *list::list_append(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto list_obj = arg_0->as<list>();
  auto arg_1 = args->at(1);
  list_obj->append(arg_1);
  return static_value::none_value;
}

object *list::list_index(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto list_obj = arg_0->as<list>();
  auto target = args->at(1);
  auto &&lst = list_obj->get_value();

  return lst->index(target, value_equal{})
      .transform(
          [](size_t i) -> object * { return new integer{static_cast<int>(i)}; })
      .value_or(static_value::none_value);
}

object *list::list_pop(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto list_obj = arg_0->as<list>();
  return list_obj->pop();
}

object *list::list_remove(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto list_obj = arg_0->as<list>();
  auto target = args->at(1);

  auto &&lst = list_obj->get_value();

  if (auto r = lst->index(target, value_equal{}); r) {
    lst->erase(r.value());
  }
  return static_value::none_value;
}

object *list::list_reverse(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto list_obj = arg_0->as<list>();
  auto &&lst = list_obj->get_value();
  std::reverse(lst->data(), lst->data() + lst->size());
  return static_value::none_value;
}

object *list::list_sort(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto list_obj = arg_0->as<list>();
  auto &&lst = list_obj->get_value();
  std::sort(lst->data(), lst->data() + lst->size(), value_less{});
  return static_value::none_value;
}

object *list::list_extend(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto list_obj = arg_0->as<list>();
  auto obj = args->at(1);

  auto next_func = obj->iter()->getattr(string_table::get_instance()->next_str);
  assert(next_func != static_value::none_value);

  object *v;
  while ((v = interpreter::get_instance()->call_virtual(next_func, nullptr)) !=
         nullptr) {
    list_obj->append(v);
  }
  return static_value::none_value;
}

object *list::list_getitem(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto list_obj = arg_0->as<list>();
  auto arg_1 = args->at(1);
  auto int_obj = arg_1->as<integer>();
  return list_obj->at(int_obj->get_value());
}

list_iterator_klass *list_iterator_klass::get_instance() {
  static list_iterator_klass instance;
  return &instance;
}

list_iterator_klass::list_iterator_klass() {
  auto map = new dict{};

  map->insert(new string{"next"},
              new native_function{list_iterator::list_iterator_next});

  set_dict(map);
  set_name("listiterator");
}

object *list_iterator_klass::next(object *x) {
  auto args = new vector<object *>{};
  args->push_back(x);
  return list_iterator::list_iterator_next(args);
}

list_iterator::list_iterator(list *lst) : lst{lst} {
  set_klass(list_iterator_klass::get_instance());
}

object *list_iterator::list_iterator_next(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto list_iter_obj = arg_0->as<list_iterator>();

  auto lst = list_iter_obj->get_list();
  int iter_cnt = list_iter_obj->get_iter_cnt();
  if (iter_cnt < lst->size()) {
    auto obj = lst->at(iter_cnt);
    list_iter_obj->inc_cnt();
    return obj;
  } else { // TODO : we need traceback here to mark iteration end
    return nullptr;
  }
}
