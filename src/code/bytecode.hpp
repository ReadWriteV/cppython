#pragma once

namespace cppython {

class bytecode {
public:
  static constexpr inline unsigned char NOP = 0;
  static constexpr inline unsigned char POP_TOP = 1;
  static constexpr inline unsigned char ROT_TWO = 2;
  static constexpr inline unsigned char ROT_THREE = 3;
  static constexpr inline unsigned char DUP_TOP = 4;
  static constexpr inline unsigned char DUP_TOP_TWO = 0x05;

  static constexpr inline unsigned char UNARY_NEGATIVE = 11;
  static constexpr inline unsigned char BINARY_MULTIPLY = 20;
  static constexpr inline unsigned char BINARY_MODULO = 22;
  static constexpr inline unsigned char BINARY_SUBSCR = 25;
  static constexpr inline unsigned char BINARY_DIVIDE = 21;
  static constexpr inline unsigned char BINARY_ADD = 23;
  static constexpr inline unsigned char BINARY_SUBTRACT = 24;

  static constexpr inline unsigned char INPLACE_ADD = 55;
  static constexpr inline unsigned char STORE_MAP = 54;
  static constexpr inline unsigned char INPLACE_SUBSTRACT = 56;
  static constexpr inline unsigned char INPLACE_MULTIPLY = 57;
  static constexpr inline unsigned char INPLACE_DIVIDE = 58;
  static constexpr inline unsigned char INPLACE_MODULO = 59;
  static constexpr inline unsigned char STORE_SUBSCR = 60;
  static constexpr inline unsigned char DELETE_SUBSCR = 61;

  static constexpr inline unsigned char GET_ITER = 68;

  static constexpr inline unsigned char LOAD_BUILD_CLASS = 0x47;

  static constexpr inline unsigned char PRINT_NEWLINE = 72;

  static constexpr inline unsigned char BREAK_LOOP = 80;
  static constexpr inline unsigned char LOAD_LOCALS = 82;
  static constexpr inline unsigned char RETURN_VALUE = 83;
  static constexpr inline unsigned char YIELD_VALUE = 86;
  static constexpr inline unsigned char POP_BLOCK = 87;
  static constexpr inline unsigned char END_FINALLY = 88;
  static constexpr inline unsigned char BUILD_CLASS = 89;

  // TODO: This is a separator
  /* Opcodes from here have an argument: */
  static constexpr inline unsigned char HAVE_ARGUMENT = 90;

  static constexpr inline unsigned char STORE_NAME = 90;
  static constexpr inline unsigned char UNPACK_SEQUENCE = 92;
  static constexpr inline unsigned char FOR_ITER = 93;
  static constexpr inline unsigned char STORE_ATTR =
      95; /* Index in name list */
  static constexpr inline unsigned char STORE_GLOBAL = 97;
  static constexpr inline unsigned char DUP_TOPX =
      99; /* number of items to duplicate */
  static constexpr inline unsigned char LOAD_CONST =
      100; /* Index in constexpr list */
  static constexpr inline unsigned char LOAD_NAME =
      101; /* Index in name list */
  static constexpr inline unsigned char BUILD_TUPLE = 102;
  static constexpr inline unsigned char BUILD_LIST = 103;
  static constexpr inline unsigned char BUILD_MAP = 105;
  static constexpr inline unsigned char LOAD_ATTR =
      106; /* Index in name list */
  static constexpr inline unsigned char COMPARE_OP =
      107; /* Comparison operator */
  static constexpr inline unsigned char IMPORT_NAME =
      108; /* Index in name list */
  static constexpr inline unsigned char IMPORT_FROM =
      109; /* Index in name list */
  static constexpr inline unsigned char JUMP_FORWARD =
      110; /* Number of bytes to skip */
  static constexpr inline unsigned char JUMP_IF_FALSE_OR_POP = 111; /* Target
                                  byte offset from beginning of code */

  static constexpr inline unsigned char CONTAINS_OP = 0x76;

  static constexpr inline unsigned char JUMP_ABSOLUTE = 113;
  static constexpr inline unsigned char POP_JUMP_IF_FALSE = 114;
  static constexpr inline unsigned char POP_JUMP_IF_TRUE = 115;
  static constexpr inline unsigned char LOAD_GLOBAL =
      116; /* Index in name list */

  static constexpr inline unsigned char IS_OP = 117;

  static constexpr inline unsigned char CONTINUE_LOOP =
      119; /* Start of loop (absolute) */
  static constexpr inline unsigned char SETUP_LOOP =
      120; /* Target address (relative) */
  static constexpr inline unsigned char SETUP_EXCEPT = 121;  /* "" */
  static constexpr inline unsigned char SETUP_FINALLY = 122; /* "" */

  static constexpr inline unsigned char LOAD_FAST =
      124; /* Local variable number */
  static constexpr inline unsigned char STORE_FAST =
      125; /* Local variable number */

  static constexpr inline unsigned char RAISE_VARARGS = 130;
  static constexpr inline unsigned char CALL_FUNCTION = 131;
  static constexpr inline unsigned char MAKE_FUNCTION = 0x84;

  static constexpr inline unsigned char MAKE_CLOSURE = 134; /* #free vars */
  static constexpr inline unsigned char LOAD_CLOSURE =
      135; /* Load free variable from closure */
  static constexpr inline unsigned char LOAD_DEREF =
      136; /* Load and dereference from closure cell */
  static constexpr inline unsigned char STORE_DEREF = 137; /* Store into cell */

  static constexpr inline unsigned char CALL_FUNCTION_VAR = 140;
  static constexpr inline unsigned char CALL_FUNCTION_KW = 0x8d;

  static constexpr inline unsigned char BUILD_CONST_KEY_MAP = 0x9c;

  static constexpr inline unsigned char LOAD_METHOD = 160;
  static constexpr inline unsigned char CALL_METHOD = 161;

  static constexpr inline unsigned char LIST_EXTEND = 0xa2;

  enum compare {
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
};

} // namespace cppython
