#pragma once

#include "memory/oop_closure.hpp"
#include "runtime/static_value.hpp"

#include <concepts>
#include <cstddef>
#include <optional>

namespace cppython {

template <typename K, typename V>
class map {
  struct map_entry {
    void *operator new[](size_t size) { return static_value::allocate(size); }
    K key;
    V value;
  };

public:
  map(std::size_t n = 8) : data{new map_entry[n]}, length{0}, capacity{n} {}

  [[nodiscard]] std::size_t size() const { return length; }

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, K, K>
  void insert(K k, V v, PredicateOperation pred) {
    for (size_t i{0}; i < length; i++) {
      if (pred(data[i].key, k)) {
        data[i].value = v;
        return;
      }
    }
    expand();
    data[length++] = {k, v};
  }

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, K, K>
  std::optional<V> get(K k, PredicateOperation pred) {
    auto i = index(k, pred);
    return i.transform([this](std::size_t pos) { return data[pos].value; });
  }

  K get_key(std::size_t pos) { return data[pos].key; }
  V get_value(std::size_t pos) { return data[pos].value; }

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, K, K>
  bool has_key(K k, PredicateOperation pred) {
    return index(k, pred).has_value();
  }

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, K, K>
  V remove(K k, PredicateOperation pred) {
    auto i = index(k, pred);
    if (!i) {
      return {};
    }

    auto v = data[i.value()].value;
    data[i.value()] = data[--length];
    return v;
  }

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, K, K>
  std::optional<std::size_t> index(K k, PredicateOperation pred) {
    for (size_t i{0}; i < length; i++) {
      if (pred(data[i].key, k)) {
        return i;
      }
    }
    return std::nullopt;
  }

  map_entry *begin() { return data; }
  map_entry *end() { return data + length; }

  void *operator new(size_t size) { return static_value::allocate(size); }
  void oops_do(oop_closure *closure)
    requires std::same_as<K, object *> && std::same_as<V, object *>
  {
    closure->do_raw_mem(reinterpret_cast<char *&>(data),
                        capacity * sizeof(map_entry));
    for (size_t i{0}; i < length; i++) {
      closure->do_oop(data[i].key);
      closure->do_oop(data[i].value);
    }
  }

private:
  void expand() {
    if (length >= capacity) {
      auto new_data = new map_entry[capacity << 1];
      std::copy_n(data, length, new_data);
      data = new_data;
      capacity <<= 1;
    }
  }

private:
  map_entry *data;
  std::size_t length;
  std::size_t capacity;
};
} // namespace cppython
