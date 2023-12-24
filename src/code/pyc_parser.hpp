#pragma once

#include "code/pyc_reader.hpp"

#include <memory>
#include <string>
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
  pyc_parser(pyc_reader &&reader) : reader{std::move(reader)} {}

  std::shared_ptr<code_object> parse();

private:
  std::shared_ptr<object> parse_object();
  std::shared_ptr<string> parse_byte_codes();
  std::shared_ptr<tuple> parse_tuple();
  std::shared_ptr<tuple> parse_consts();
  std::shared_ptr<tuple> parse_names();

  std::shared_ptr<code_object> get_code_object(bool ref_flag);
  std::shared_ptr<string> get_string(bool ref_flag);
  std::shared_ptr<integer> get_integer(bool ref_flag);
  std::shared_ptr<string> get_short_ascii(bool ref_flag);
  std::shared_ptr<tuple> get_tuple(bool ref_flag);

private:
  pyc_reader reader;
  std::vector<std::shared_ptr<object>> ref_table;
};
} // namespace cppython
