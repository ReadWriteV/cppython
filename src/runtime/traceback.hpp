#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"
#include "utils/singleton.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cppython {

class frame;
class string;

struct stack_element {
  std::shared_ptr<string> file_name;
  std::shared_ptr<string> func_name;
  int line_no;
};

class traceback_klass : public klass, public singleton<traceback_klass> {
  friend class singleton<traceback_klass>;

private:
  traceback_klass();

public:
  std::shared_ptr<string> repr(std::shared_ptr<object> x) override;
};

class traceback : public object {
public:
  traceback();

  void record_frame(std::shared_ptr<frame> frm);

  auto &get_stack_elements() { return stack_elements; }

private:
  std::vector<stack_element> stack_elements;
};

} // namespace cppython
