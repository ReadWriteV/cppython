#pragma once

#include <concepts>
#include <cstddef>
#include <optional>
#include <utility>

namespace cppython {

class klass;
class object;
class oop_closure;

/// @brief a std::vector like container
/// @tparam T type of element
template <typename T>
class vector {
public:
  /// @brief construct a vector with n capacity
  /// @param n initial capacity
  vector(std::size_t n = 8);

  /// @brief add element to the back, expend when capacity isn't enough
  /// @param v element to add
  void push_back(T v) {
    if (length >= capacity) {
      expand();
    }
    data_ptr[length++] = v;
  }

  /// @brief decrease length by 1, and return last element
  /// @return last element
  T pop_back() { return data_ptr[--length]; }

  /// @brief insert element at pos, expend when capacity isn't enough
  /// @param pos position to insert
  /// @param v element to insert
  void insert(std::size_t pos, T v) {
    push_back({});
    for (auto i = length - 1; i > pos; i--) {
      data_ptr[i] = data_ptr[i - 1];
    }
    data_ptr[pos] = v;
  }

  /// @brief return element at pos, no bound check
  /// @param pos position to get
  /// @return element at pos
  [[nodiscard]] T at(std::size_t pos) { return data_ptr[pos]; }

  /// @brief return element at pos, no bound check
  /// @return element at pos
  [[nodiscard]] T back() { return data_ptr[length - 1]; }

  /// @brief assign or insert element at pos
  /// @param pos position of element
  /// @param v element
  void set(std::size_t pos, T v) {
    if (length <= pos) {
      length = pos + 1;
      while (length > capacity) {
        expand();
      }
    }
    data_ptr[pos] = v;
  }

  template <typename PredicateOperation>
    requires std::predicate<PredicateOperation, object *, object *>
  [[nodiscard]] std::optional<std::size_t> index(object *v,
                                                 PredicateOperation pred) {
    for (size_t i{0}; i < length; i++) {
      if (pred(data_ptr[i], v)) {
        return i;
      }
    }
    return std::nullopt;
  }

  /// @brief get size of container
  /// @return size of container
  [[nodiscard]] std::size_t size() const { return length; }

  /// @brief
  /// @return
  [[nodiscard]] bool empty() const { return length == 0; }

  /// @brief
  /// @return
  [[nodiscard]] T *data() const { return data_ptr; }

  /// @brief remove element at pos
  /// @param pos pos to remove
  void erase(std::size_t pos) {
    for (auto i = pos; i + 1 < length; i++) {
      data_ptr[i] = data_ptr[i + 1];
    }
    length--;
  }

  T *begin() { return data_ptr; }
  T *end() { return data_ptr + length; }

  void *operator new(size_t size);

  void oops_do(oop_closure *closure)
    requires !std::same_as<T, klass *> && !std::same_as<T, object *>;

  void oops_do(oop_closure *closure)
    requires std::same_as<T, klass *> || std::same_as<T, object *>;

private:
  void expand();

private:
  T *data_ptr;
  std::size_t length;
  std::size_t capacity;
};

} // namespace cppython
