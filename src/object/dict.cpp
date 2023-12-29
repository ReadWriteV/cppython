#include "object/dict.hpp"
#include "memory/oop_closure.hpp"
#include "object/integer.hpp"
#include "object/list.hpp"
#include "object/string.hpp"
#include "object/tuple.hpp"
#include "runtime/function.hpp"
#include "runtime/static_value.hpp"
#include "runtime/string_table.hpp"

using namespace cppython;

void dict_klass::initialize() {
  auto map = new dict{};
  map->insert(new string{"setdefault"},
              new native_function{dict::dict_set_default});
  map->insert(new string{"remove"}, new native_function{dict::dict_remove});
  map->insert(new string{"keys"}, new native_function{dict::dict_keys});
  map->insert(new string{"values"}, new native_function{dict::dict_values});
  map->insert(new string{"items"}, new native_function{dict::dict_items});
  map->insert(new string{"iterkeys"}, new native_function{dict::dict_iterkeys});
  map->insert(new string{"itervalues"},
              new native_function{dict::dict_itervalues});
  map->insert(new string{"iteritems"},
              new native_function{dict::dict_iteritems});
  set_dict(map);
  set_name("dict");
  (new type{})->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::string dict_klass::to_string(object *obj) {
  auto dict_obj = obj->as<dict>();

  auto fmt_str = [](object *v) {
    if (v->get_klass() == string_klass::get_instance()) {
      return "'" + v->to_string() + "'";
    } else {
      return v->to_string();
    }
  };

  std::string result;

  result += "{";

  if (dict_obj->size() != 0) {
    result += fmt_str(dict_obj->get_value()->get_key(0));
    result += ": ";
    result += fmt_str(dict_obj->get_value()->get_value(0));
  }

  for (size_t i{1}; i < dict_obj->get_value()->size(); i++) {
    result += ", ";
    result += fmt_str(dict_obj->get_value()->get_key(i));
    result += ": ";
    result += fmt_str(dict_obj->get_value()->get_value(i));
  }

  result += "}";
  return result;
}

object *dict_klass::subscr(object *x, object *y) {
  auto map_obj = x->as<dict>();
  return map_obj->at(y);
}

void dict_klass::store_subscr(object *x, object *y, object *z) {
  auto map_obj = x->as<dict>();
  map_obj->insert(y, z);
}

object *dict_klass::iter(object *x) {
  auto obj = new dict_iterator{x->as<dict>()};
  obj->set_klass(dict_iterator_klass<iter_key>::get_instance());
  return obj;
}

void dict_klass::del_subscr(object *x, object *y) {
  auto map_obj = x->as<dict>();
  map_obj->remove(y);
}

object *dict_klass::getattr(object *obj, string *name) {
  assert(obj && obj->get_klass() == this);
  return get_dict()->at(name);
}

object *dict_klass::allocate_instance(object *obj_type,
                                      vector<object *> *args) {
  if (!args || args->size() == 0) {
    return new dict{};
  } else {
    return nullptr;
  }
}

void dict_klass::oops_do(oop_closure *closure, object *obj) {
  auto dict_obj = obj->as<dict>();
  closure->do_map(dict_obj->data_address());
}

size_t dict_klass::size() const { return sizeof(dict); }

object *dict::at(object *k) {
  auto r = value->index(k, value_equal{});
  return r.transform([this](size_t i) { return value->get_value(i); })
      .value_or(static_value::none_value);
}

object *dict::dict_set_default(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto dict_obj = arg_0->as<dict>();

  auto key = args->at(1);
  auto value = args->at(2);

  if (!dict_obj->has_key(key)) {
    dict_obj->insert(key, value);
  }

  return static_value::none_value;
}

object *dict::dict_remove(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto dict_obj = arg_0->as<dict>();

  auto key = args->at(1);

  dict_obj->remove(key);

  return static_value::none_value;
}

object *dict::dict_keys(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto dict_obj = arg_0->as<dict>();

  auto keys = new list{};
  auto map_obj = dict_obj->get_value();
  for (size_t i{0}; i < map_obj->size(); i++) {
    keys->append(map_obj->get_key(i));
  }
  return keys;
}

object *dict::dict_values(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto dict_obj = arg_0->as<dict>();

  auto values = new list{};
  auto map_obj = dict_obj->get_value();
  for (size_t i{0}; i < map_obj->size(); i++) {
    values->append(map_obj->get_value(i));
  }

  return values;
}

object *dict::dict_items(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto dict_obj = arg_0->as<dict>();

  auto items = new list{};
  auto map_obj = dict_obj->get_value();
  for (size_t i{0}; i < map_obj->size(); i++) {
    auto item = new vector<object *>{};
    item->push_back(map_obj->get_key(i));
    item->push_back(map_obj->get_value(i));
    items->append(new tuple{item});
  }

  return items;
}

object *dict::dict_iterkeys(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto dict_obj = arg_0->as<dict>();
  auto iter = new dict_iterator{dict_obj};
  iter->set_klass(dict_iterator_klass<iter_key>::get_instance());
  return iter;
}

object *dict::dict_itervalues(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto dict_obj = arg_0->as<dict>();
  auto iter = new dict_iterator{dict_obj};
  iter->set_klass(dict_iterator_klass<iter_value>::get_instance());
  return iter;
}

object *dict::dict_iteritems(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto dict_obj = arg_0->as<dict>();
  auto iter = new dict_iterator{dict_obj};
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
  auto dic = new dict{};
  dic->insert(string_table::get_instance()->next_str,
              new native_function{dict_iterator::dict_iterator_next<n>});
  set_dict(dic);
  set_name(klass_names[n]);
}

dict_iterator::dict_iterator(dict *owner) : dic{owner} {}

template <iter_type n>
object *dict_iterator::dict_iterator_next(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto dict_iter_obj = arg_0->as<dict_iterator>();

  auto dic = dict_iter_obj->get_owner();
  int iter_cnt = dict_iter_obj->get_iter_cnt();

  if (iter_cnt < dic->size()) {
    object *obj;
    if constexpr (n == iter_key) {
      obj = dic->get_value()->get_key(iter_cnt);
    } else if constexpr (n == iter_value) {
      obj = dic->get_value()->get_value(iter_cnt);
    } else if constexpr (n == iter_item) {
      auto item = new vector<object *>{};
      item->push_back(dic->get_value()->get_key(iter_cnt));
      item->push_back(dic->get_value()->get_value(iter_cnt));
      obj = new tuple{item};
    }
    dict_iter_obj->inc_cnt();
    return obj;
  } else // TODO : we need Traceback here to mark iteration end
  {
    return nullptr;
  }
}
