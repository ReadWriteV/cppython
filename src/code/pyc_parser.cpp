#include "code/pyc_parser.hpp"
#include "code/code_object.hpp"
#include "object/integer.hpp"
#include "object/string.hpp"
#include "object/tuple.hpp"
#include "runtime/static_value.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <print>

using namespace cppython;

constexpr int flag_ref = 0x80;

std::shared_ptr<code_object> pyc_parser::parse() {
  auto magic_number = reader.read<int>();
  // std::println("magic number is {:#x}", magic_number);

  auto bit_field = reader.read<int>();
  // std::println("bit field is {}", bit_field);

  time_t mod_date = reader.read<int>();
  auto time = std::chrono::system_clock::from_time_t(mod_date);
  const std::chrono::zoned_time cur_time{std::chrono::current_zone(), time};
  // std::println("last modified date is {}", cur_time);

  auto file_size = reader.read<int>();
  // std::println("file size is {} byte", file_size);

  auto r = parse_object();
  assert(r->get_klass() == code_klass::get_instance());
  return std::static_pointer_cast<code_object>(r);
}

std::shared_ptr<object> pyc_parser::parse_object() {
  int object_type = reader.read<unsigned char>();
  bool ref_flag = object_type & flag_ref;
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
    if (auto idx = reader.read<int>(); idx < ref_table.size()) {
      return ref_table.at(idx);
    } else {
      return static_value::none_value;
    }
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
  return std::shared_ptr<object>();
}

std::shared_ptr<code_object> pyc_parser::get_code_object(bool ref_flag) {
  size_t pos = -1;
  if (ref_flag) {
    ref_table.push_back(static_value::none_value);
    pos = ref_table.size() - 1;
  }

  int argcount = reader.read<int>();
  // std::println("argcount {}", argcount);

  int posonlyargcount = reader.read<int>();
  // std::println("posonlyargcount {}", posonlyargcount);

  int kwonlyargcount = reader.read<int>();
  // std::println("kwonlyargcount {}", kwonlyargcount);

  int nlocals = reader.read<int>();
  // std::println("nlocals {}", nlocals);

  int stacksize = reader.read<int>();
  // std::println("stacksize {}", stacksize);

  int flags = reader.read<int>();
  // std::println("flags {:04x}", flags);

  auto code = parse_byte_codes();
  // std::print("code ");
  // for (const auto ch : code->to_string()) {
  //   std::print("{:02x}", static_cast<unsigned char>(ch));
  // }
  // std::print("\n");

  auto consts = parse_consts();
  // std::println("consts {}", consts->to_string());

  auto names = parse_names();
  // std::println("names {}", names->to_string());

  auto varnames = parse_object();
  // std::println("varnames {}", varnames->to_string());

  auto freevars = parse_object();
  // std::println("freevars {}", freevars->to_string());

  auto cellvars = parse_object();
  // std::println("cellvars {}", cellvars->to_string());

  auto filename = parse_object();
  // std::println("filename {}", filename->to_string());

  auto name = parse_object();
  // std::println("name {}", name->to_string());

  int firstlineno = reader.read<int>();
  // std::println("line_no {}", firstlineno);

  auto lnotab = parse_object();
  // for (const auto ch : lnotab->to_string()) {
  //   std::print("{:02x}", static_cast<unsigned char>(ch));
  // }
  // std::print("\n");

  auto tmp = std::make_shared<code_object>(
      argcount, posonlyargcount, kwonlyargcount, nlocals, stacksize, flags,
      std::move(code), std::move(consts), std::move(names), std::move(varnames),
      std::move(freevars), std::move(cellvars), std::move(filename),
      std::move(name), firstlineno, std::move(lnotab));
  if (ref_flag) {
    ref_table.at(pos) = tmp;
  }
  return tmp;
}

std::shared_ptr<string> pyc_parser::parse_byte_codes() {
  assert(reader.read<char>() == 's');
  return get_string(false);
}

std::shared_ptr<string> pyc_parser::get_string(bool ref_flag) {
  int length = reader.read<int>();
  std::string str;
  str.resize(length);

  std::generate_n(str.begin(), length,
                  [this]() { return reader.read<char>(); });

  auto tmp = std::make_shared<string>(std::move(str));
  if (ref_flag) {
    ref_table.push_back(tmp);
  }
  return tmp;
}

std::shared_ptr<integer> pyc_parser::get_integer(bool ref_flag) {
  auto tmp = std::make_shared<integer>(reader.read<int>());
  if (ref_flag) {
    ref_table.push_back(tmp);
  }
  return tmp;
}

std::shared_ptr<string> pyc_parser::get_short_ascii(bool ref_flag) {
  auto length = reader.read<unsigned char>();
  std::string str;
  str.resize(length);

  std::generate_n(str.begin(), length,
                  [this]() { return reader.read<char>(); });

  auto tmp = std::make_shared<string>(std::move(str));
  if (ref_flag) {
    ref_table.push_back(tmp);
  }

  return tmp;
}

std::shared_ptr<tuple> pyc_parser::parse_consts() { return parse_tuple(); }

std::shared_ptr<tuple> pyc_parser::parse_names() { return parse_tuple(); }

std::shared_ptr<tuple> pyc_parser::parse_tuple() {
  auto r = parse_object();
  assert(r->get_klass() == tuple_klass::get_instance());
  return std::static_pointer_cast<tuple>(r);
}

std::shared_ptr<tuple> pyc_parser::get_tuple(bool ref_flag) {
  size_t pos = -1;
  if (ref_flag) {
    ref_table.push_back(static_value::none_value);
    pos = ref_table.size() - 1;
  }
  int length = reader.read<char>();
  std::vector<std::shared_ptr<object>> list;
  for (int i{0}; i < length; i++) {
    list.push_back(parse_object());
  }
  auto tmp = std::make_shared<tuple>(std::move(list));
  if (ref_flag) {
    ref_table.at(pos) = tmp;
  }
  return tmp;
}
