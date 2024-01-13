#include "runtime/traceback.hpp"
#include "object/dict.hpp"
#include "object/string.hpp"
#include "runtime/frame.hpp"

#include <cassert>
#include <format>
#include <ranges>

using namespace cppython;

traceback_klass::traceback_klass() {
  set_name("traceback");
  set_dict(std::make_shared<dict>());
  std::make_shared<type>()->set_own_klass(this);
  add_super(object_klass::get_instance());
}

std::shared_ptr<string> traceback_klass::repr(std::shared_ptr<object> x) {
  assert(x && x->get_klass() == this);
  auto tbx = std::static_pointer_cast<traceback>(x);
  std::string r{"Traceback (most recent call last):\n"};
  for (auto &e : tbx->get_stack_elements() | std::views::reverse) {
    r += std::format("  File \"{}\", line {}, in {}\n",
                     e.file_name->str()->get_value(), e.line_no,
                     e.func_name->str()->get_value());
  }
  return std::make_shared<string>(std::move(r));
}

traceback::traceback() { set_klass(traceback_klass::get_instance()); }

void traceback::record_frame(std::shared_ptr<frame> frm) {
  stack_elements.push_back(
      {frm->get_file_name(), frm->get_func_name(), frm->get_source_lineno()});
}
