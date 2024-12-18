#include "object/dict.hpp"
#include "object/integer.hpp"
#include "object/list.hpp"
#include "object/string.hpp"
#include "object/tuple.hpp"
#include "runtime/function.hpp"
#include "runtime/static_value.hpp"
#include "runtime/string_table.hpp"

#include <cassert>

using namespace cppython;

void dict_klass::initialize() {
  auto map = std::make_shared<dict>();
  map->insert(std::make_shared<string>("setdefault"),
              std::make_shared<function>(dict::dict_set_default));
  map->insert(std::make_shared<string>("pop"),
              std::make_shared<function>(dict::dict_pop));
  map->insert(std::make_shared<string>("keys"),
              std::make_shared<function>(dict::dict_keys));
  map->insert(std::make_shared<string>("values"),
              std::make_shared<function>(dict::dict_values));
  map->insert(std::make_shared<string>("items"),
              std::make_shared<function>(dict::dict_items));
  set_dict(map);
  set_name("dict");
  std::make_shared<type>()->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::shared_ptr<string> dict_klass::repr(std::shared_ptr<object> obj) {
  assert(obj && obj->get_klass() == this);
  auto dict_obj = std::static_pointer_cast<cppython::dict>(obj);

  auto fmt_str = [](const std::shared_ptr<object> &v) {
    if (v->get_klass() == string_klass::get_instance()) {
      return "'" + v->str()->get_value() + "'";
    } else {
      return v->str()->get_value();
    }
  };

  std::string result;

  result += "{";

  auto iter = dict_obj->get_value().begin();

  if (iter != dict_obj->get_value().end()) {
    result += fmt_str(iter->first);
    result += ": ";
    result += fmt_str(iter->second);

    while (++iter != dict_obj->get_value().end()) {
      result += ", ";
      result += fmt_str(iter->first);
      result += ": ";
      result += fmt_str(iter->second);
    }
  }

  result += "}";
  return std::make_shared<string>(std::move(result));
}

std::shared_ptr<object> dict_klass::subscr(std::shared_ptr<object> x,
                                           std::shared_ptr<object> y) {
  assert(x && x->get_klass() == this);
  auto map_obj = std::static_pointer_cast<dict>(x);

  return map_obj->at(y);
}

void dict_klass::store_subscr(std::shared_ptr<object> x,
                              std::shared_ptr<object> y,
                              std::shared_ptr<object> z) {
  assert(x && x->get_klass() == this);
  auto map_obj = std::static_pointer_cast<dict>(x);

  map_obj->insert(y, z);
}

std::shared_ptr<object> dict_klass::iter(std::shared_ptr<object> x) {
  auto obj = std::make_shared<dict_iterator>(std::static_pointer_cast<dict>(x));
  return obj;
}

void dict_klass::del_subscr(std::shared_ptr<object> x,
                            std::shared_ptr<object> y) {
  assert(x && x->get_klass() == this);
  auto map_obj = std::static_pointer_cast<dict>(x);
  map_obj->remove(y);
}

std::shared_ptr<object> dict_klass::getattr(std::shared_ptr<object> obj,
                                            std::shared_ptr<string> name) {
  assert(obj && obj->get_klass() == this);
  return get_dict()->at(name);
}

std::shared_ptr<object> dict_klass::allocate_instance(
    std::shared_ptr<object> obj_type,
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  if (!args || args->size() == 0) {
    return std::make_shared<dict>();
  } else {
    return nullptr;
  }
}

bool dict::has_key(std::shared_ptr<object> k) {
  auto iter = std::find_if(value.begin(), value.end(), [&k](auto &&x) {
    return x.first->equal(k) == static_value::true_value;
  });
  return iter != value.end();
}

std::shared_ptr<object> dict::at(std::shared_ptr<object> k) {
  return get(k, value_equal{}).value_or(static_value::none_value);
}

std::shared_ptr<object> dict::remove(std::shared_ptr<object> k) {
  auto iter = std::find_if(value.begin(), value.end(),
                           [&](auto &&e) { return value_equal{}(e.first, k); });
  auto tmp = iter->second;
  value.erase(iter);
  return tmp;
}

std::shared_ptr<object> dict::dict_set_default(
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {

  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == dict_klass::get_instance());
  auto dict_obj = std::static_pointer_cast<dict>(arg_0);

  auto key = args->at(1);
  auto value = args->at(2);

  if (!dict_obj->has_key(key)) {
    dict_obj->insert(key, value);
  }

  return static_value::none_value;
}

std::shared_ptr<object>
dict::dict_pop(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == dict_klass::get_instance());
  auto dict_obj = std::static_pointer_cast<dict>(arg_0);

  auto key = args->at(1);

  dict_obj->remove(key);

  return static_value::none_value;
}

std::shared_ptr<object>
dict::dict_keys(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == dict_klass::get_instance());
  auto dict_obj = std::static_pointer_cast<dict>(arg_0);

  auto keys = std::make_shared<list>();

  for (const auto &[k, v] : dict_obj->get_value()) {
    keys->append(k);
  }

  return keys;
}

std::shared_ptr<object>
dict::dict_values(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == dict_klass::get_instance());
  auto dict_obj = std::static_pointer_cast<dict>(arg_0);

  auto values = std::make_shared<list>();

  for (const auto &[k, v] : dict_obj->get_value()) {
    values->append(v);
  }

  return values;
}

std::shared_ptr<object>
dict::dict_items(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == dict_klass::get_instance());
  auto dict_obj = std::static_pointer_cast<dict>(arg_0);

  auto items = std::make_shared<list>();

  for (const auto &[k, v] : dict_obj->get_value()) {
    auto k_v = std::make_shared<tuple>();
    k_v->append(k);
    k_v->append(v);
    items->append(k_v);
  }

  return items;
}

dict_iterator_klass::dict_iterator_klass() {
  set_name("dict_keyiterator");
  set_dict(std::make_shared<dict>());
}

std::shared_ptr<object> dict_iterator_klass::next(std::shared_ptr<object> x) {
  assert(x->get_klass() == dict_iterator_klass::get_instance());
  auto dict_iter_obj = std::static_pointer_cast<dict_iterator>(x);

  auto dic = dict_iter_obj->get_owner();
  int iter_cnt = dict_iter_obj->get_iter_cnt();

  if (iter_cnt < dic->size()) {
    auto iter = dic->get_value().begin();
    iter = std::next(iter, iter_cnt);
    dict_iter_obj->inc_cnt();
    return iter->first;
  } else {
    // TODO : we need traceback here to mark iteration end
    return nullptr;
  }
}

dict_iterator::dict_iterator(std::shared_ptr<dict> owner) : dic{owner} {
  set_klass(dict_iterator_klass::get_instance());
}
