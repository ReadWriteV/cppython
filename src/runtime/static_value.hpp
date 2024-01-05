#pragma once

namespace cppython {

class klass;
class object;
class code_object;
class oop_closure;

template <typename T>
class vector;

class static_value {
public:
  static void create();
  static void destroy();

  [[nodiscard]] static object *get_bool_value(bool v);
  static void add_klass(klass *k);

  static void gc();
  [[nodiscard]] static void *allocate(size_t size);
  [[nodiscard]] static void *allocate_meta(size_t size);
  static void oops_do(oop_closure *closure);

  static inline object *true_value{nullptr};
  static inline object *false_value{nullptr};
  static inline object *none_value{nullptr};

  static inline vector<klass *> *klasses{nullptr};

  static inline code_object *main_code{nullptr};

  static inline object *stop_iteration{nullptr};
};

struct value_equal {
  [[nodiscard]] bool operator()(object *lhs, object *rhs) const;
};

struct value_less {
  [[nodiscard]] bool operator()(object *lhs, object *rhs) const;
};

} // namespace cppython
