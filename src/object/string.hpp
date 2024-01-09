#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

#include <memory>
#include <string>

namespace cppython {
class string_klass : public klass, public singleton<string_klass> {
public:
  void initialize();

  std::string to_string(std::shared_ptr<object> obj) override;

  std::shared_ptr<object> add(std::shared_ptr<object> x,
                              std::shared_ptr<object> y) override;

  std::shared_ptr<object> equal(std::shared_ptr<object> x,
                                std::shared_ptr<object> y) override;
  std::shared_ptr<object> less(std::shared_ptr<object> x,
                               std::shared_ptr<object> y) override;

  std::shared_ptr<object> subscr(std::shared_ptr<object> x,
                                 std::shared_ptr<object> y) override;
  std::shared_ptr<object> len(std::shared_ptr<object> x) override;
  std::shared_ptr<object> allocate_instance(
      std::shared_ptr<object> obj_type,
      std::shared_ptr<std::vector<std::shared_ptr<object>>> args) override;
};

class string : public object {
public:
  template <typename... Args>
  string(Args &&...args) : value(std::forward<Args>(args)...) {
    set_klass(string_klass::get_instance());
  }

  auto size() const { return value.size(); }
  auto at(size_t pos) const { return value.at(pos); }

  const std::string &get_value() const { return value; }

  std::shared_ptr<string> join(std::shared_ptr<object> iterable);

  static std::shared_ptr<object>
  string_upper(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  static std::shared_ptr<object>
  string_join(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

  static std::shared_ptr<object>
  string_repr(std::shared_ptr<std::vector<std::shared_ptr<object>>> args);

private:
  std::string value;
};

} // namespace cppython
