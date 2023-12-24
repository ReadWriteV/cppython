#pragma once

namespace cppython {

template <typename T>
class singleton {
public:
  static T *get_instance() {
    static T instance;
    return &instance;
  }

protected:
  singleton() = default;
  ~singleton() = default;

public:
  singleton(const singleton &) = delete;
  singleton(singleton &&) = delete;
  singleton &operator=(const singleton &) = delete;
  singleton &operator=(singleton &&) = delete;
};

} // namespace cppython