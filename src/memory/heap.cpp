#include "memory/heap.hpp"
#include "memory/oop_closure.hpp"
#include "runtime/static_value.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <print>
#include <utility>

using namespace cppython;

space::space(const size_t _size) : size{_size} {
  auto p = std::malloc(size);
  assert(p != nullptr);
  base = static_cast<char *>(p);
  end = base + size;
  top = reinterpret_cast<char *>(reinterpret_cast<intmax_t>(base + 15) & -16);
  capacity = end - top;
}

space::~space() {
  if (base) {
    free(base);
    base = nullptr;
  }
  top = 0;
  end = 0;
  capacity = 0;
  size = 0;
}

bool space::can_alloc(const size_t _size) const { return capacity > _size; }

void *space::allocate(size_t _size) {
  assert(can_alloc(_size));
  _size = (_size + 7) & -8;
  char *start = top;
  top += _size;
  capacity -= _size;
  return start;
}

void space::clear(bool reset_mem) {
  if (reset_mem) {
    std::memset(base, 0, size);
  }
  top = reinterpret_cast<char *>(reinterpret_cast<intmax_t>(base + 15) & -16);
  capacity = end - top;
}

bool space::has_obj(char *obj) const { return obj >= base && end > obj; }

heap *heap::get_instance() {
  static heap instance;
  return &instance;
}

heap::heap(size_t size) {
  eden = new space{size};
  survivor = new space{size};
  metaspace = new space{size / 16};

  eden->clear();
  survivor->clear();
  metaspace->clear();
}

heap::~heap() {
  delete std::exchange(eden, nullptr);
  delete std::exchange(survivor, nullptr);
  delete std::exchange(metaspace, nullptr);
}

void *heap::allocate(size_t size) {
  if (!eden->can_alloc(size)) {
    gc();
  }
  assert(eden->can_alloc(size));
  return eden->allocate(size);
}

void *heap::allocate_meta(size_t size) {
  if (!metaspace->can_alloc(size)) {
    return nullptr;
  }
  return metaspace->allocate(size);
}

void heap::copy_live_objects() {
  scavenge_oop_closure closure{eden, survivor, metaspace};
  closure.scavenge();
}

void heap::gc() {
  // std::println("gc starting...");
  // std::println("  befroe gc :");
  // std::println("  eden's capacity is {}", eden->get_capacity());

  copy_live_objects();

  std::swap(eden, survivor);

  // std::println("  after gc :");
  // std::println("  eden's capacity is {}", eden->get_capacity());
  // std::println("gc end");
  survivor->clear(
      false); // don't clear memory in survivor, workaround for gc bug
}
