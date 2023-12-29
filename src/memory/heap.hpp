#pragma once

#include "utils/singleton.hpp"

namespace cppython {

class object;

class space {
public:
  space(const size_t size);
  ~space();

  bool can_alloc(const size_t size) const;
  void *allocate(size_t size);
  bool has_obj(char *obj) const;
  void clear(bool reset_mem = true);

  size_t get_capacity() const { return capacity; }

private:
  char *base;
  char *top;
  char *end;
  size_t size;
  size_t capacity;
};

class heap : public singleton<heap> {
  friend class singleton<heap>;

private:
  heap(size_t size = MAX_CAP);

public:
  static inline constexpr size_t MAX_CAP{2 * 1024 * 1024};

  ~heap();

  void *allocate(size_t size);
  void *allocate_meta(size_t size);
  void copy_live_objects();

  void gc();

private:
  space *eden;
  space *survivor;
  space *metaspace;
};

} // namespace cppython
