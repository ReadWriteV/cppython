#pragma once

#include "code/pyc_reader.hpp"

#include <string_view>
#include <vector>

namespace cppython {

class code_object;
class object;
class integer;
class string;
class tuple;

class pyc_parser {
public:
  pyc_parser(std::string_view filename) : reader{filename} {};

  code_object *parse();

private:
  object *parse_object();
  string *parse_byte_codes();
  tuple *parse_tuple();

  code_object *get_code_object(bool ref_flag);
  string *get_string(bool ref_flag);
  integer *get_integer(bool ref_flag);
  string *get_short_ascii(bool ref_flag);
  tuple *get_tuple(bool ref_flag);

private:
  pyc_reader reader;
  std::vector<object *> ref_table;
};
} // namespace cppython
