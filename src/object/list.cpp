#include "object/list.hpp"
#include "object/dict.hpp"
#include "object/integer.hpp"
#include "object/string.hpp"
#include "runtime/function.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/string_table.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <unordered_map>

using namespace cppython;

void list_klass::initialize() {
  auto map = std::make_shared<dict>();
  map->insert(std::make_shared<string>("append"),
              std::make_shared<function>(list::list_append));
  map->insert(std::make_shared<string>("index"),
              std::make_shared<function>(list::list_index));
  map->insert(std::make_shared<string>("pop"),
              std::make_shared<function>(list::list_pop));
  map->insert(std::make_shared<string>("remove"),
              std::make_shared<function>(list::list_remove));
  map->insert(std::make_shared<string>("reverse"),
              std::make_shared<function>(list::list_reverse));
  map->insert(std::make_shared<string>("sort"),
              std::make_shared<function>(list::list_sort));
  map->insert(std::make_shared<string>("extend"),
              std::make_shared<function>(list::list_extend));
  map->insert(string_table::get_instance()->getitem_str,
              std::make_shared<function>(list::list_getitem));
  map->insert(string_table::get_instance()->repr_str,
              std::make_shared<function>(list::list_repr));
  set_dict(map);

  set_name("list");
  std::make_shared<type>()->set_own_klass(this);

  add_super(object_klass::get_instance());
}

std::shared_ptr<string> list_klass::repr(std::shared_ptr<object> x) {
  auto list_obj = std::static_pointer_cast<list>(x);

  std::string result;

  result += "[";

  auto fmt_str = [&list_obj](size_t idx) {
    if (list_obj->at(idx)->get_klass() == string_klass::get_instance()) {
      return "'" + list_obj->at(idx)->str()->get_value() + "'";
    } else {
      return list_obj->at(idx)->str()->get_value();
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
  return std::make_shared<string>(std::move(result));
}

std::shared_ptr<object> list_klass::less(std::shared_ptr<object> x,
                                         std::shared_ptr<object> y) {
  auto p = std::static_pointer_cast<list>(x);
  assert(p && (p->get_klass() == this));

  // check y is list or not
  if (y->get_klass() != this) {
    return static_value::get_bool_value(
        std::is_lt(klass::compare(x->get_klass(), y->get_klass())));
  }

  auto q = std::static_pointer_cast<list>(y);
  assert(q && (q->get_klass() == this));

  auto &&lst1 = p->get_value();
  auto &&lst2 = q->get_value();

  auto r = std::ranges::lexicographical_compare(
      lst1, lst2,
      [](const std::shared_ptr<object> &l, const std::shared_ptr<object> &r) {
        return l->less(r) == static_value::true_value;
      });

  return static_value::get_bool_value(r);
}

std::shared_ptr<object> list_klass::add(std::shared_ptr<object> x,
                                        std::shared_ptr<object> y) {
  assert(x && x->get_klass() == this);
  auto list_obj_x = std::static_pointer_cast<list>(x);
  assert(y && y->get_klass() == this);
  auto list_obj_y = std::static_pointer_cast<list>(y);

  auto &&lst_x = list_obj_x->get_value();
  auto &&lst_y = list_obj_y->get_value();

  const auto lst_x_y = {lst_x, lst_y};

  auto result = lst_x_y | std::views::join;

  return std::make_shared<list>(std::from_range, result);
}

std::shared_ptr<object> list_klass::mul(std::shared_ptr<object> x,
                                        std::shared_ptr<object> y) {

  assert(x && x->get_klass() == this);
  auto list_obj_x = std::static_pointer_cast<list>(x);
  assert(y && y->get_klass() == integer_klass::get_instance());
  auto int_obj_y = std::static_pointer_cast<integer>(y);

  auto &&lst = list_obj_x->get_value();
  auto &&n = int_obj_y->get_value();

  auto lst_n = std::views::repeat(lst, n) | std::views::join;

  return std::make_shared<list>(std::from_range, lst_n);
}

std::shared_ptr<object> list_klass::subscr(std::shared_ptr<object> x,
                                           std::shared_ptr<object> y) {
  assert(x->get_klass() == this);
  assert(y->get_klass() == integer_klass::get_instance());

  auto list_obj = std::static_pointer_cast<list>(x);
  auto index_obj = std::static_pointer_cast<integer>(y);

  return list_obj->at(index_obj->get_value());
}

void list_klass::store_subscr(std::shared_ptr<object> x,
                              std::shared_ptr<object> y,
                              std::shared_ptr<object> z) {
  assert(x->get_klass() == this);
  assert(y->get_klass() == integer_klass::get_instance());

  auto list_obj = std::static_pointer_cast<list>(x);
  auto index_obj = std::static_pointer_cast<integer>(y);

  list_obj->at(index_obj->get_value()) = z;
}

void list_klass::del_subscr(std::shared_ptr<object> x,
                            std::shared_ptr<object> y) {
  assert(x->get_klass() == this);
  assert(y->get_klass() == integer_klass::get_instance());

  auto list_obj = std::static_pointer_cast<list>(x);
  auto index_obj = std::static_pointer_cast<integer>(y);

  list_obj->get_value().erase(list_obj->get_value().begin() +
                              index_obj->get_value());
}

std::shared_ptr<object> list_klass::contains(std::shared_ptr<object> x,
                                             std::shared_ptr<object> y) {
  assert(x->get_klass() == this);
  auto list_obj = std::static_pointer_cast<list>(x);

  auto &&lst = list_obj->get_value();
  return static_value::get_bool_value(std::any_of(
      lst.begin(), lst.end(), [&y](const std::shared_ptr<object> &v) {
        return v->equal(y) == static_value::true_value;
      }));
}

std::shared_ptr<object> list_klass::iter(std::shared_ptr<object> x) {
  assert(x && x->get_klass() == this);
  auto list_obj = std::static_pointer_cast<list>(x);
  return std::make_shared<list_iterator>(list_obj);
}

std::shared_ptr<object> list_klass::len(std::shared_ptr<object> x) {
  assert(x->get_klass() == this);
  auto list_obj = std::static_pointer_cast<list>(x);
  return std::make_shared<integer>(static_cast<int>(list_obj->size()));
}

std::shared_ptr<object> list_klass::allocate_instance(
    std::shared_ptr<object> obj_type,
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  if (!args || args->size() == 0) {
    return std::make_shared<list>();
  } else {
    return nullptr;
  }
}

std::shared_ptr<object>
list::list_append(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->isinstance(list_klass::get_instance()->get_type_object()));

  auto list_obj = std::static_pointer_cast<list>(arg_0);

  auto arg_1 = args->at(1);

  list_obj->append(arg_1);
  return static_value::none_value;
}

std::shared_ptr<object>
list::list_index(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->isinstance(list_klass::get_instance()->get_type_object()));

  auto list_obj = std::static_pointer_cast<list>(arg_0);

  auto target = args->at(1);

  auto &&lst = list_obj->get_value();

  auto iter =
      std::ranges::find_if(lst, [&target](const std::shared_ptr<object> &v) {
        return v->equal(target) == static_value::true_value;
      });

  return iter == lst.end() ? nullptr
                           : std::make_shared<integer>(static_cast<int>(
                                 std::distance(lst.begin(), iter)));
}

std::shared_ptr<object>
list::list_pop(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->isinstance(list_klass::get_instance()->get_type_object()));

  auto list_obj = std::static_pointer_cast<list>(arg_0);

  return list_obj->pop();
}

std::shared_ptr<object>
list::list_remove(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {

  auto arg_0 = args->at(0);
  assert(arg_0->isinstance(list_klass::get_instance()->get_type_object()));
  auto list_obj = std::static_pointer_cast<list>(arg_0);

  auto target = args->at(1);

  auto &&lst = list_obj->get_value();

  auto iter = std::find_if(
      lst.begin(), lst.end(), [&target](const std::shared_ptr<object> &v) {
        return v->equal(target) == static_value::true_value;
      });

  if (iter != lst.end()) {
    lst.erase(iter);
  }

  return static_value::none_value;
}

std::shared_ptr<object>
list::list_reverse(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {

  auto arg_0 = args->at(0);
  assert(arg_0->isinstance(list_klass::get_instance()->get_type_object()));
  auto list_obj = std::static_pointer_cast<list>(arg_0);

  auto &&lst = list_obj->get_value();
  std::ranges::reverse(lst);

  return static_value::none_value;
}

std::shared_ptr<object>
list::list_sort(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {

  auto arg_0 = args->at(0);
  assert(arg_0->isinstance(list_klass::get_instance()->get_type_object()));
  auto list_obj = std::static_pointer_cast<list>(arg_0);

  auto &&lst = list_obj->get_value();

  std::ranges::sort(lst, [](const std::shared_ptr<object> &l,
                            const std::shared_ptr<object> &r) {
    return l->less(r) == static_value::true_value;
  });

  return static_value::none_value;
}

std::shared_ptr<object>
list::list_extend(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {

  auto arg_0 = args->at(0);
  assert(arg_0->isinstance(list_klass::get_instance()->get_type_object()));
  auto list_obj = std::static_pointer_cast<list>(arg_0);

  auto obj = args->at(1);

  auto next_func = obj->iter()->getattr(string_table::get_instance()->next_str);
  assert(next_func != static_value::none_value);

  std::shared_ptr<object> v;
  while ((v = interpreter::get_instance()->call_virtual(next_func, nullptr)) !=
         nullptr) {
    list_obj->append(v);
  }
  return static_value::none_value;
}

std::shared_ptr<object>
list::list_getitem(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->isinstance(list_klass::get_instance()->get_type_object()));
  auto list_obj = std::static_pointer_cast<list>(arg_0);

  auto arg_1 = args->at(1);
  assert(arg_1->get_klass() == integer_klass::get_instance());
  auto int_obj = std::static_pointer_cast<integer>(arg_1);

  return list_obj->at(int_obj->get_value());
}

std::shared_ptr<object>
list::list_repr(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->isinstance(list_klass::get_instance()->get_type_object()));
  auto list_obj = std::static_pointer_cast<list>(arg_0);

  return list_klass::get_instance()->repr(list_obj);
}

list_iterator_klass::list_iterator_klass() {
  auto map = std::make_shared<dict>();

  map->insert(std::make_shared<string>("next"),
              std::make_shared<function>(list_iterator::list_iterator_next));

  set_dict(map);
  set_name("listiterator");
}

std::shared_ptr<object> list_iterator_klass::next(std::shared_ptr<object> x) {
  auto args = std::make_shared<std::vector<std::shared_ptr<object>>>();
  args->push_back(x);
  return list_iterator::list_iterator_next(args);
}

list_iterator::list_iterator(std::shared_ptr<list> lst) : lst{lst} {
  set_klass(list_iterator_klass::get_instance());
}

std::shared_ptr<object> list_iterator::list_iterator_next(
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {

  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == list_iterator_klass::get_instance());
  auto list_iter_obj = std::static_pointer_cast<list_iterator>(arg_0);

  auto lst = list_iter_obj->get_list();
  int iter_cnt = list_iter_obj->get_iter_cnt();
  if (iter_cnt < lst->size()) {
    auto obj = lst->at(iter_cnt);
    list_iter_obj->inc_cnt();
    return obj;
  } else {
    // TODO : we need traceback here to mark iteration end
    return nullptr;
  }
}
