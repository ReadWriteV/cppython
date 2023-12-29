#pragma once

#include "object/string.hpp"
#include "utils/singleton.hpp"

namespace cppython {

class oop_closure;

class string_table : public singleton<string_table> {
  friend class singleton<string_table>;

private:
  string_table();

public:
  void oops_do(oop_closure *f);

public:
  string *next_str{nullptr};
  string *mod_str{nullptr};
  string *init_str{nullptr};
  string *add_str{nullptr};
  string *len_str{nullptr};
  string *call_str{nullptr};
  string *getitem_str{nullptr};
  string *setitem_str{nullptr};
  string *getattr_str{nullptr};
  string *setattr_str{nullptr};
};

} // namespace cppython
