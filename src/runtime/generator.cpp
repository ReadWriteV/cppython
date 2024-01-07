#include "runtime/generator.hpp"
#include "runtime/frame.hpp"
#include "runtime/interpreter.hpp"

#include <cassert>

using namespace cppython;

std::shared_ptr<object> generator_klass::iter(std::shared_ptr<object> obj) {
  return obj;
}

std::shared_ptr<object> generator_klass::next(std::shared_ptr<object> obj) {
  assert(obj && obj->get_klass() == this);
  auto g = std::static_pointer_cast<Generator>(obj);
  return interpreter::get_instance()->eval_generator(g);
}

Generator::Generator(std::shared_ptr<function> func,
                     std::shared_ptr<std::vector<std::shared_ptr<object>>> args,
                     int arg_cnt) {
  frm = std::make_shared<frame>(func, args, arg_cnt);
  set_klass(generator_klass::get_instance());
}
