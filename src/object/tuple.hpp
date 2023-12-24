#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

#include <memory>
#include <vector>

namespace cppython {

class tuple_klass : public klass, public singleton<tuple_klass> {
  friend class singleton<tuple_klass>;

private:
  tuple_klass();

public:
  std::string to_string(std::shared_ptr<object> obj) override;

  std::shared_ptr<object> subscr(std::shared_ptr<object> x,
                                 std::shared_ptr<object> y) override;
};

class tuple : public object {
public:
  template <typename... Args>
  tuple(Args &&...args) : value{std::forward<Args>(args)...} {
    set_klass(tuple_klass::get_instance());
  }

  const std::vector<std::shared_ptr<object>> &get_value() const {
    return value;
  }

  auto empty() { return value.empty(); }
  auto size() { return value.size(); }

  auto at(size_t pos) { return value.at(pos); }
  auto append(const std::shared_ptr<object> &v) { return value.push_back(v); }

private:
  std::vector<std::shared_ptr<object>> value;
};

} // namespace cppython
