#pragma once

#include "object/string.hpp"

namespace cppython {

class oop_closure;

class string_table {
public:
  static string_table *get_instance();

public:
  string_table(const string_table &) = delete;
  string_table(string_table &&) = delete;
  string_table &operator=(const string_table &) = delete;
  string_table &operator=(string_table &&) = delete;

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
  string *name_str{nullptr};
  string *iter_str{nullptr};
  string *repr_str{nullptr};

  string *getitem_str{nullptr};
  string *setitem_str{nullptr};
  string *getattr_str{nullptr};
  string *setattr_str{nullptr};

  string *empty_str{nullptr};
  string *dll_suf_str{nullptr};
  string *pyc_suf_str{nullptr};
  string *libdir_pre_str{nullptr};
};

} // namespace cppython
