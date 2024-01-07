#pragma once

#include "object/klass.hpp"
#include "object/object.hpp"

#include <memory>
#include <vector>

namespace cppython {

class function;
class frame;

class generator_klass : public klass, public singleton<generator_klass> {
public:
  std::shared_ptr<object> next(std::shared_ptr<object> obj) override;
  std::shared_ptr<object> iter(std::shared_ptr<object> obj) override;
};

class Generator : public object {
public:
  Generator(std::shared_ptr<function> func,
            std::shared_ptr<std::vector<std::shared_ptr<object>>> args,
            int arg_cnt);

  std::shared_ptr<frame> get_frame() const { return frm; }
  void set_frame(std::shared_ptr<frame> x) { frm = x; }

private:
  std::shared_ptr<frame> frm;
};

} // namespace cppython
