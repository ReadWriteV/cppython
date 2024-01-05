#pragma once

#include <compare>
#include <string_view>

namespace cppython {

class object;
class string;
class dict;
class type;
class list;
class oop_closure;

template <typename T>
class vector;

// TODO: implement other overload functions
class klass {
public:
  klass();

  void add_super(klass *x);
  type *get_super();

  void order_supers();

  void set_super_list(list *x) { super = x; }
  auto get_mro() { return mro; }

  void set_type_object(type *x) { type_object = x; }
  type *get_type_object() { return type_object; }

  void set_name(std::string_view x);
  auto get_name() const { return name; }

  void set_dict(dict *x) { map = x; }
  dict *get_dict() { return map; }

  static std::weak_ordering compare(klass *x, klass *y);

  virtual std::string to_string(object *obj);

  virtual object *greater(object *x, object *y) { return nullptr; }
  virtual object *less(object *x, object *y) { return nullptr; }
  virtual object *equal(object *x, object *y) { return nullptr; }
  virtual object *not_equal(object *x, object *y) { return nullptr; }
  virtual object *ge(object *x, object *y) { return nullptr; }
  virtual object *le(object *x, object *y) { return nullptr; }

  virtual object *add(object *x, object *y);
  virtual object *sub(object *x, object *y) { return nullptr; }
  virtual object *mul(object *x, object *y) { return nullptr; }
  virtual object *div(object *x, object *y) { return nullptr; }
  virtual object *mod(object *x, object *y) { return nullptr; }
  virtual object *subscr(object *x, object *y);
  virtual void store_subscr(object *x, object *y, object *z);
  virtual void del_subscr(object *x, object *y) { return; }

  virtual object *getattr(object *x, object *y);
  virtual object *setattr(object *x, object *y, object *z);

  virtual object *get_klass_attr(object *x, object *y);

  virtual object *contains(object *x, object *y) { return nullptr; }
  virtual object *iter(object *x);
  virtual object *next(object *x);
  virtual object *len(object *x);
  virtual object *call(vector<object *> *args) { return nullptr; }
  virtual object *allocate_instance(object *obj_type, vector<object *> *args);

  void *operator new(size_t size);

  // gc interfaces
  // this is for objects of this type.
  virtual void oops_do(oop_closure *closure, object *obj);
  // for klass itself only.
  virtual void oops_do(oop_closure *closure);
  virtual size_t size() const;

private:
  object *find_and_call(object *x, vector<object *> *args, string *func_name);
  object *find_in_parents(object *x, object *y);

private:
  list *super{nullptr};
  list *mro{nullptr};

  type *type_object{nullptr};

  string *name{nullptr};
  dict *map{nullptr};
};

} // namespace cppython