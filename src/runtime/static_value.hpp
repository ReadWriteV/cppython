#pragma once

#include <memory>
#include <vector>

namespace cppython {

class klass;
class object;

class static_value {
public:
  static void create();
  static void destroy();
  static std::shared_ptr<object> get_bool_value(bool v);

  static std::shared_ptr<object> true_value;
  static std::shared_ptr<object> false_value;
  static std::shared_ptr<object> none_value;

  static std::shared_ptr<std::vector<klass *>> klasses;
};

struct value_equal {
  [[nodiscard]] bool operator()(const std::shared_ptr<object> &lhs,
                                const std::shared_ptr<object> &rhs) const;
};

struct value_less {
  [[nodiscard]] bool operator()(const std::shared_ptr<object> &lhs,
                                const std::shared_ptr<object> &rhs) const;
};
} // namespace cppython
