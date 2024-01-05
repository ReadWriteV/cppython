#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"

#include <string>
#include <string_view>

namespace cppython {

class oop_closure;

template <typename T>
class vector;

class string_klass : public klass {

public:
  static string_klass *get_instance();

private:
  string_klass() = default;
  ~string_klass() = default;

public:
  string_klass(const string_klass &) = delete;
  string_klass(string_klass &&) = delete;
  string_klass &operator=(const string_klass &) = delete;
  string_klass &operator=(string_klass &&) = delete;

public:
  void initialize();

  std::string to_string(object *obj) override;

  object *add(object *x, object *y) override;

  object *equal(object *x, object *y) override;
  object *less(object *x, object *y) override;

  object *subscr(object *x, object *y) override;
  object *len(object *x) override;
  object *allocate_instance(object *obj_type, vector<object *> *args) override;

  void oops_do(oop_closure *closure, object *obj) override;
  size_t size() const override;
};

class string : public object {
public:
  using klass_type = string_klass;

public:
  string(std::string_view sv);
  string(const std::size_t cnt, const char ch);
  string(const std::size_t cnt);

  [[nodiscard]] std::size_t size() const { return length; }
  [[nodiscard]] char &at(const std::size_t pos) { return data_ptr[pos]; }

  [[nodiscard]] char *begin() const { return data_ptr; }
  [[nodiscard]] char *end() const { return data_ptr + length; }

  [[nodiscard]] char *data() const { return data_ptr; }
  char *&data_address() { return data_ptr; };

  string *join(object *iterable);

  static object *string_upper(vector<object *> *args);
  static object *string_join(vector<object *> *args);

private:
  char *data_ptr;
  std::size_t length;
};

} // namespace cppython
