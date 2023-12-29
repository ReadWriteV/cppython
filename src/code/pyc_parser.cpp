#include "code/pyc_parser.hpp"
#include "code/code_object.hpp"
#include "object/integer.hpp"
#include "object/string.hpp"
#include "object/tuple.hpp"
#include "runtime/static_value.hpp"
#include "utils/vector.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <string>

using namespace cppython;

constexpr int flag_ref = 0x80;

code_object *pyc_parser::parse() {
  const auto magic_number = reader.read<int>();

  const auto bit_field = reader.read<int>();

  const time_t mod_date = reader.read<int>();
  const auto time = std::chrono::system_clock::from_time_t(mod_date);
  const std::chrono::zoned_time cur_time{std::chrono::current_zone(), time};

  const auto file_size = reader.read<int>();

  auto r = parse_object();

  return r->as<code_object>();
}

object *pyc_parser::parse_object() {
  int object_type = reader.read<unsigned char>();
  const bool ref_flag = object_type & flag_ref;
  object_type = object_type & ~flag_ref;

  switch (object_type) {
  case 'N': // None
    return static_value::none_value;
    break;
  case 'F': // False
    return static_value::false_value;
    break;
  case 'T': // True
    return static_value::true_value;
    break;
  case 'i': // integer
    return get_integer(ref_flag);
    break;
  case 'r':
  case 'R': // ref integer
    return ref_table.at(reader.read<int>());
    break;
  case 's': // string
  case 't': // ref string
    return get_string(ref_flag);
    break;
  case 'c': // code
    return get_code_object(ref_flag);
    break;
  case '(': // tuple
  case ')': // small tuple
    return get_tuple(ref_flag);
    break;
  case 'z': // short ascii
  case 'Z': // short ascii interned
    return get_short_ascii(ref_flag);
    break;
  default:
    assert(false);
    break;
  }
  return nullptr;
}

code_object *pyc_parser::get_code_object(bool ref_flag) {
  size_t pos = -1;
  if (ref_flag) {
    ref_table.push_back(nullptr);
    pos = ref_table.size() - 1;
  }

  int argcount = reader.read<int>();

  int posonlyargcount = reader.read<int>();

  int kwonlyargcount = reader.read<int>();

  int nlocals = reader.read<int>();

  int stacksize = reader.read<int>();

  int flags = reader.read<int>();

  auto code = parse_byte_codes();

  auto consts = parse_tuple();

  auto names = parse_tuple();

  auto varnames = parse_object();

  auto freevars = parse_object();

  auto cellvars = parse_object();

  auto filename = parse_object();

  auto name = parse_object();

  int firstlineno = reader.read<int>();

  auto lnotab = parse_object();

  auto tmp = new code_object{argcount,       posonlyargcount,
                             kwonlyargcount, nlocals,
                             stacksize,      flags,
                             code,           consts,
                             names,          varnames,
                             freevars,       cellvars,
                             filename,       name,
                             firstlineno,    lnotab};
  if (ref_flag) {
    ref_table.at(pos) = tmp;
  }
  return tmp;
}

string *pyc_parser::parse_byte_codes() {
  assert(reader.read<char>() == 's');
  return get_string(false);
}

string *pyc_parser::get_string(bool ref_flag) {
  int length = reader.read<int>();
  auto tmp = new string{static_cast<size_t>(length), '\0'};

  std::generate_n(tmp->begin(), length, [this] { return reader.read<char>(); });

  if (ref_flag) {
    ref_table.push_back(tmp);
  }
  return tmp;
}

integer *pyc_parser::get_integer(bool ref_flag) {
  auto tmp = new integer{reader.read<int>()};
  if (ref_flag) {
    ref_table.push_back(tmp);
  }
  return tmp;
}

string *pyc_parser::get_short_ascii(bool ref_flag) {
  auto length = reader.read<unsigned char>();
  auto tmp = new string{static_cast<size_t>(length), '\0'};

  std::generate_n(tmp->begin(), length, [this] { return reader.read<char>(); });

  if (ref_flag) {
    ref_table.push_back(tmp);
  }
  return tmp;
}

tuple *pyc_parser::parse_tuple() {
  auto r = parse_object();
  return r->as<tuple>();
}

tuple *pyc_parser::get_tuple(bool ref_flag) {
  size_t pos = -1;
  if (ref_flag) {
    ref_table.push_back(nullptr);
    pos = ref_table.size() - 1;
  }
  int length = reader.read<char>();
  auto lst = new vector<object *>{};

  for (int i{0}; i < length; i++) {
    lst->push_back(parse_object());
  }
  auto tmp = new tuple{lst};
  if (ref_flag) {
    ref_table.at(pos) = tmp;
  }
  return tmp;
}
