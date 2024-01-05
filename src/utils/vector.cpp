#include "utils/vector.hpp"
#include "memory/oop_closure.hpp"
#include "runtime/static_value.hpp"

#include <algorithm>

using namespace cppython;

template <typename T>
vector<T>::vector(std::size_t n) {
  length = 0;
  capacity = n;
  void *p = static_value::allocate(sizeof(T) * capacity);
  data_ptr = new (p) T[capacity];
}

template <typename T>
void *vector<T>::operator new(size_t size) {
  return static_value::allocate(size);
}

template <typename T>
void vector<T>::oops_do(oop_closure *closure)
  requires !std::same_as<T, klass *> && !std::same_as<T, object *>
{
  closure->do_raw_mem(reinterpret_cast<char *&>(data_ptr),
                      capacity * sizeof(T));
}

template <typename T>
void vector<T>::oops_do(oop_closure *closure)
  requires std::same_as<T, klass *> || std::same_as<T, object *>
{
  closure->do_raw_mem(reinterpret_cast<char *&>(data_ptr),
                      capacity * sizeof(T));

  for (size_t i = 0; i < length; i++) {
    if constexpr (std::is_same_v<T, klass *>) {
      closure->do_klass(data_ptr[i]);
    } else {
      closure->do_oop(data_ptr[i]);
    }
  }
}

template <typename T>
void vector<T>::expand() {
  void *p = static_value::allocate(sizeof(T) * (capacity << 1));
  auto new_data = new (p) T[capacity << 1];
  std::copy_n(data_ptr, length, new_data);
  data_ptr = new_data;
  capacity <<= 1;
}

template class vector<object *>;
template class vector<klass *>;

class string;
template class vector<string *>;
