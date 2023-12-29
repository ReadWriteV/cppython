#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

#include <string>
#include <string_view>

namespace cppython {

class oop_closure;

template <typename T>
class vector;

class string_klass : public klass, public singleton<string_klass> {
public:
  void initialize();

  std::string to_string(object *obj) override;

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

  [[nodiscard]] std::size_t size() const { return length; }
  [[nodiscard]] char at(const std::size_t pos) const { return data_ptr[pos]; }

  [[nodiscard]] char *begin() const { return data_ptr; }
  [[nodiscard]] char *end() const { return data_ptr + length; }

  [[nodiscard]] char *data() const { return data_ptr; }
  char *&data_address() { return data_ptr; };

  static object *string_upper(vector<object *> *args);

private:
  char *data_ptr;
  std::size_t length;
};

} // namespace cppython
