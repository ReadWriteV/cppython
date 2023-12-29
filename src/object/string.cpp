#include "object/string.hpp"
#include "memory/oop_closure.hpp"
#include "object/dict.hpp"
#include "object/integer.hpp"
#include "runtime/function.hpp"
#include "runtime/static_value.hpp"
#include "utils/vector.hpp"

#include <algorithm>

using namespace cppython;

void string_klass::initialize() {

  auto string_dict = new dict{};

  string_dict->insert(new string{"upper"},
                      new native_function{string::string_upper});

  set_dict(string_dict);

  set_name("str");
  (new type{})->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::string string_klass::to_string(object *obj) {
  auto p = obj->as<string>();
  return std::string{p->data(), p->size()};
}

object *string_klass::equal(object *x, object *y) {
  if (x->get_klass() != y->get_klass()) {
    return static_value::false_value;
  }

  auto p = x->as<string>();
  auto q = y->as<string>();

  if (p->size() != q->size()) {
    return static_value::false_value;
  }

  auto r = std::ranges::equal(*p, *q);
  return static_value::get_bool_value(r);
}

object *string_klass::less(object *x, object *y) {
  auto p = x->as<string>();

  // check y is string or not
  if (!y->is<string>()) {
    return static_value::get_bool_value(
        std::is_lt(klass::compare(x->get_klass(), y->get_klass())));
  }

  auto q = y->as<string>();

  auto r = std::ranges::lexicographical_compare(*p, *q);

  return static_value::get_bool_value(r);
}

object *string_klass::subscr(object *x, object *y) {
  auto str_obj = x->as<string>();
  auto int_obj = y->as<integer>();

  auto ch = str_obj->at(int_obj->get_value());

  return new string{1, ch};
}

object *string_klass::len(object *x) {
  auto str_obj = x->as<string>();
  return new integer{static_cast<int>(str_obj->size())};
}

object *string_klass::allocate_instance(object *obj_type,
                                        vector<object *> *args) {
  if (args == nullptr || args->size() == 0) {
    return new string{""};
  } else {
    return nullptr;
  }
}

void string_klass::oops_do(oop_closure *closure, object *obj) {
  auto str_obj = obj->as<string>();
  closure->do_raw_mem(str_obj->data_address(), str_obj->size());
}

size_t string_klass::size() const { return sizeof(string); }

string::string(std::string_view sv) {
  length = sv.size();
  data_ptr = static_cast<char *>(static_value::allocate(length));
  std::copy(sv.begin(), sv.end(), data_ptr);
  set_klass(string_klass::get_instance());
}

string::string(const std::size_t cnt, const char ch) {
  length = cnt;
  data_ptr = static_cast<char *>(static_value::allocate(length));
  std::fill_n(data_ptr, length, ch);
  set_klass(string_klass::get_instance());
}

object *string::string_upper(vector<object *> *args) {
  auto arg_0 = args->at(0);
  auto str_obj = arg_0->as<string>();

  auto length = str_obj->size();
  if (length == 0) {
    return static_value::none_value;
  }

  auto upper_str = new string{length, '\0'};

  std::transform(str_obj->begin(), str_obj->end(), upper_str->begin(),
                 ::toupper);

  return upper_str;
}