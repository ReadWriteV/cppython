#pragma once

namespace cppython {

enum class bytecode : unsigned char {
  NOP = 0,
  POP_TOP = 1,
  ROT_TWO = 2,
  ROT_THREE = 3,
  DUP_TOP = 4,
  DUP_TOP_TWO = 0x05,

  UNARY_NEGATIVE = 11,
  BINARY_MULTIPLY = 20,
  BINARY_MODULO = 22,
  BINARY_SUBSCR = 25,
  BINARY_DIVIDE = 21,
  BINARY_ADD = 23,
  BINARY_SUBTRACT = 24,

  INPLACE_ADD = 55,
  STORE_MAP = 54,
  INPLACE_SUBSTRACT = 56,
  INPLACE_MULTIPLY = 57,
  INPLACE_DIVIDE = 58,
  INPLACE_MODULO = 59,
  STORE_SUBSCR = 60,
  DELETE_SUBSCR = 61,

  GET_ITER = 68,

  LOAD_BUILD_CLASS = 0x47,

  PRINT_NEWLINE = 72,

  BREAK_LOOP = 80,
  LOAD_LOCALS = 82,
  RETURN_VALUE = 83,
  YIELD_VALUE = 86,
  POP_BLOCK = 87,
  END_FINALLY = 88,
  BUILD_CLASS = 89,

  // TODO: This is a separator
  /* Opcodes from here have an argument: */
  HAVE_ARGUMENT = 90,

  STORE_NAME = 90,
  UNPACK_SEQUENCE = 92,
  FOR_ITER = 93,
  STORE_ATTR = 95, /* Index in name list */
  STORE_GLOBAL = 97,
  DUP_TOPX = 99,    /* number of items to duplicate */
  LOAD_CONST = 100, /* Index in constexpr list */
  LOAD_NAME = 101,  /* Index in name list */
  BUILD_TUPLE = 102,
  BUILD_LIST = 103,
  BUILD_MAP = 105,
  LOAD_ATTR = 106,            /* Index in name list */
  COMPARE_OP = 107,           /* Comparison operator */
  IMPORT_NAME = 108,          /* Index in name list */
  IMPORT_FROM = 109,          /* Index in name list */
  JUMP_FORWARD = 110,         /* Number of bytes to skip */
  JUMP_IF_FALSE_OR_POP = 111, /* Target
                                  byte offset from beginning of code */

  CONTAINS_OP = 0x76,

  JUMP_ABSOLUTE = 113,
  POP_JUMP_IF_FALSE = 114,
  POP_JUMP_IF_TRUE = 115,
  LOAD_GLOBAL = 116, /* Index in name list */

  IS_OP = 117,

  CONTINUE_LOOP = 119, /* Start of loop (absolute) */
  SETUP_LOOP = 120,    /* Target address (relative) */
  SETUP_EXCEPT = 121,  /* "" */
  SETUP_FINALLY = 122, /* "" */

  LOAD_FAST = 124,  /* Local variable number */
  STORE_FAST = 125, /* Local variable number */

  RAISE_VARARGS = 130,
  CALL_FUNCTION = 131,
  MAKE_FUNCTION = 0x84,

  MAKE_CLOSURE = 134, /* #free vars */
  LOAD_CLOSURE = 135, /* Load free variable from closure */
  LOAD_DEREF = 136,   /* Load and dereference from closure cell */
  STORE_DEREF = 137,  /* Store into cell */

  CALL_FUNCTION_VAR = 140,
  CALL_FUNCTION_KW = 0x8d,

  BUILD_CONST_KEY_MAP = 0x9c,

  LOAD_METHOD = 160,
  CALL_METHOD = 161,

  LIST_EXTEND = 0xa2,

};

enum class compare : unsigned char {
  less = 0,
  less_equal,
  equal,
  not_equal,
  greater,
  greater_equal,
  in,
  not_in,
  is,
  is_not,
  exc_match
};

} // namespace cppython
