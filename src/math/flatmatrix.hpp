#pragma once
#include <vector>

namespace detra {
namespace math {

template <typename T>
struct FlatMatrix {
  size_t         N;
  size_t         M;
  std::vector<T> p_data;

  FlatMatrix(size_t _N, size_t _M) :
    N(_N), M(_M), p_data(_N * _M) {}

  FlatMatrix(size_t _N, size_t _M, const std::vector<T>& _data) :
    N(_N), M(_M), p_data(_data) {}


  struct FlatMatrixProxy {
    size_t i;
    size_t M;

    std::vector<T>& data;

    FlatMatrixProxy(size_t _i, size_t _M, std::vector<T>& _data) :
      i(_i), M(_M), data(_data) {}

    T& operator[](size_t j) { return data[i * M + j]; }
  };


  FlatMatrixProxy& operator[](size_t i) { return FlatMatrixProxy(i, M, p_data); }


  size_t getColumnCount() { return M; }
  size_t getRowCount() { return N; }

  T*       data() { return p_data.data(); }
  const T* data() const { return p_data.data(); }
};

} // namespace math
} // namespace detra
