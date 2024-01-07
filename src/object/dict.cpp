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
  map->insert(std::make_shared<string>("remove"),
              std::make_shared<function>(dict::dict_remove));
  map->insert(std::make_shared<string>("keys"),
              std::make_shared<function>(dict::dict_keys));
  map->insert(std::make_shared<string>("values"),
              std::make_shared<function>(dict::dict_values));
  map->insert(std::make_shared<string>("items"),
              std::make_shared<function>(dict::dict_items));
  map->insert(std::make_shared<string>("iterkeys"),
              std::make_shared<function>(dict::dict_iterkeys));
  map->insert(std::make_shared<string>("itervalues"),
              std::make_shared<function>(dict::dict_itervalues));
  map->insert(std::make_shared<string>("iteritems"),
              std::make_shared<function>(dict::dict_iteritems));
  set_dict(map);
  set_name("dict");
  std::make_shared<type>()->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::string dict_klass::to_string(std::shared_ptr<object> obj) {
  assert(obj && obj->get_klass() == this);
  auto dict_obj = std::static_pointer_cast<cppython::dict>(obj);

  auto fmt_str = [](const std::shared_ptr<object> &v) {
    if (v->get_klass() == string_klass::get_instance()) {
      return "'" + v->to_string() + "'";
    } else {
      return v->to_string();
    }
  };

  std::string result;

  result += "{";

  auto iter = dict_obj->get_value().begin();

  if (iter != dict_obj->get_value().end()) {
    result += fmt_str(iter->first);
    result += ": ";
    result += fmt_str(iter->second);
  }

  while (iter != dict_obj->get_value().end()) {
    ++iter;
    result += ", ";
    result += fmt_str(iter->first);
    result += ": ";
    result += fmt_str(iter->second);
  }

  result += "}";
  return result;
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
  obj->set_klass(dict_iterator_klass<iter_key>::get_instance());

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
  auto iter = std::find_if(value.begin(), value.end(), [&k](auto &&x) {
    return x.first->equal(k) == static_value::true_value;
  });
  return iter == value.end() ? static_value::none_value : iter->second;
}

std::shared_ptr<object> dict::remove(std::shared_ptr<object> k) {
  auto iter = std::find_if(value.begin(), value.end(), [&k](const auto &item) {
    return item.first->equal(k) == static_value::true_value;
  });
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
dict::dict_remove(std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
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

std::shared_ptr<object> dict::dict_iterkeys(
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == dict_klass::get_instance());
  auto dict_obj = std::static_pointer_cast<dict>(arg_0);
  auto iter = std::make_shared<dict_iterator>(dict_obj);
  iter->set_klass(dict_iterator_klass<iter_key>::get_instance());
  return iter;
}

std::shared_ptr<object> dict::dict_itervalues(
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == dict_klass::get_instance());
  auto dict_obj = std::static_pointer_cast<dict>(arg_0);
  auto iter = std::make_shared<dict_iterator>(dict_obj);
  iter->set_klass(dict_iterator_klass<iter_value>::get_instance());
  return iter;
}

std::shared_ptr<object> dict::dict_iteritems(
    std::shared_ptr<std::vector<std::shared_ptr<object>>> args) {
  auto arg_0 = args->at(0);
  assert(arg_0->get_klass() == dict_klass::get_instance());
  auto dict_obj = std::static_pointer_cast<dict>(arg_0);
  auto iter = std::make_shared<dict_iterator>(dict_obj);
  iter->set_klass(dict_iterator_klass<iter_item>::get_instance());
  return iter;
}

template <iter_type n>
dict_iterator_klass<n>::dict_iterator_klass() {

  std::string_view klass_names[] = {
      "dictionary-keyiterator",
      "dictionary-valueiterator",
      "dictionary-itemiterator",
  };
  set_dict(std::make_shared<dict>());
  set_name(klass_names[n]);
}

template <iter_type n>
std::shared_ptr<object>
dict_iterator_klass<n>::next(std::shared_ptr<object> x) {
  assert(x->get_klass() == dict_iterator_klass<n>::get_instance());
  auto dict_iter_obj = std::static_pointer_cast<dict_iterator>(x);

  auto dic = dict_iter_obj->get_owner();
  int iter_cnt = dict_iter_obj->get_iter_cnt();

  if (iter_cnt < dic->size()) {
    auto iter = dic->get_value().begin();
    iter = std::next(iter, iter_cnt);
    std::shared_ptr<object> obj;
    if constexpr (n == iter_key) {
      obj = iter->first;
    } else if constexpr (n == iter_value) {
      obj = iter->second;
    } else if constexpr (n == iter_item) {
      auto item = std::make_shared<tuple>();
      item->append(iter->first);
      item->append(iter->second);
    }
    dict_iter_obj->inc_cnt();
    return obj;
  } else // TODO : we need traceback here to mark iteration end
  {
    return nullptr;
  }
}

dict_iterator::dict_iterator(std::shared_ptr<dict> owner) : dic{owner} {}
