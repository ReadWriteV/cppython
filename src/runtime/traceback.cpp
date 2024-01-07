#include "runtime/traceback.hpp"
#include "object/string.hpp"
#include "runtime/frame.hpp"

#include <cassert>
#include <format>
#include <ranges>

using namespace cppython;

std::string traceback_klass::to_string(std::shared_ptr<object> x) {
  assert(x && x->get_klass() == this);
  auto tbx = std::static_pointer_cast<traceback>(x);
  std::string r{"Traceback (most recent call last):\n"};
  for (auto &e : tbx->get_stack_elements() | std::views::reverse) {
    r +=
        std::format("  File \"{}\", line {}, in {}\n", e.file_name->to_string(),
                    e.line_no, e.func_name->to_string());
  }
  return r;
}

traceback::traceback() { set_klass(traceback_klass::get_instance()); }

void traceback::record_frame(std::shared_ptr<frame> frm) {
  stack_elements.push_back(
      {frm->get_file_name(), frm->get_func_name(), frm->get_source_lineno()});
}
