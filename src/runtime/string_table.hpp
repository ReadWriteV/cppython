#pragma once

#include "object/string.hpp"

namespace cppython {

class string_table {
  string_table();

public:
  static string_table *get_instance();

  std::shared_ptr<string> next_str;
  std::shared_ptr<string> mod_str;
  std::shared_ptr<string> init_str;
  std::shared_ptr<string> add_str;
  std::shared_ptr<string> len_str;
  std::shared_ptr<string> call_str;
  std::shared_ptr<string> name_str;
  std::shared_ptr<string> iter_str;
  std::shared_ptr<string> repr_str;

  std::shared_ptr<string> getitem_str;
  std::shared_ptr<string> setitem_str;
  std::shared_ptr<string> getattr_str;
  std::shared_ptr<string> setattr_str;
};

} // namespace cppython
