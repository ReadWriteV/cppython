#pragma once

#include <fstream>
#include <iostream>
#include <string_view>

namespace cppython {
class pyc_reader {
public:
  pyc_reader(std::string_view filename) {
    fp.open(filename.data(), std::ios::binary);
    if (!fp.is_open()) {
      std::cerr << "failed to open " << filename << '\n';
    }
  };
  template <typename T>
  T read() {
    T data;
    fp.read(reinterpret_cast<char *>(&data), sizeof(T));
    return data;
  }

private:
  std::ifstream fp;
};
} // namespace cppython
