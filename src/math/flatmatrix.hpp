#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <iostream>

namespace detra {
namespace math {

template <typename T>
struct FlatMatrix {
  size_t         N;
  size_t         M;
  std::vector<T> p_data;

  FlatMatrix()                        = default;
  FlatMatrix(const FlatMatrix& other) = default;

  FlatMatrix(size_t _N, size_t _M) :
    N(_N), M(_M), p_data(_N * _M) {}

  FlatMatrix(size_t _N, size_t _M, const std::vector<T>& _data) :
    N(_N), M(_M), p_data(_data) {}

  FlatMatrix(size_t _N, size_t _M, std::vector<T>&& _data) :
    N(_N), M(_M), p_data(std::move(_data)) {}


  using Vector = std::vector<T>;
  struct FlatMatrixProxy {
    size_t i;
    size_t M;

    std::vector<T>& data;

    FlatMatrixProxy(size_t _i, size_t _M, std::vector<T>& _data) :
      i(_i), M(_M), data(_data) {}

    T& operator[](size_t j) { return data[i * M + j]; }
  };

  FlatMatrixProxy operator[](size_t i) { return FlatMatrixProxy(i, M, p_data); }


  struct ConstFlatMatrixProxy {
    size_t i;
    size_t M;

    const std::vector<T>& data;

    ConstFlatMatrixProxy(size_t _i, size_t _M, const std::vector<T>& _data) :
      i(_i), M(_M), data(_data) {}

    const T& operator[](size_t j) const { return data[i * M + j]; }
  };

  ConstFlatMatrixProxy operator[](size_t i) const { return ConstFlatMatrixProxy(i, M, p_data); }

  size_t getColumnCount() const { return M; }

  size_t getRowCount() const { return N; }

  inline T& get(size_t i, size_t j) { return p_data[i * M + j]; }

  /**
  * Performs the following operation to vector output from vector input
  * O = output;
  * I = input;
  * A = *this;
  * Oj = Ii * sum j Aij
  */
  void apply_inplace(const std::vector<float>& input, std::vector<float>& output) {
    std::fill(output.begin(), output.end(), 0.0);
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        output[j] += input[i] * p_data[i * M + j];
  }

  const T* data() const { return p_data.data(); }

  void row_normalize(float damping_factor = 1.0f) {
    int N = getRowCount();
    int M = getColumnCount();

    for (size_t i = 0; i < N; i++) {
      float total = 0.0f;
      for (size_t j = 0; j < M; j++) {
        total += get(i, j);
      }
      if (total == 0.0f) {
        for (size_t j = 0; j < M; j++) {
          get(i, j) = 1.0f / N;
        }
      } else {
        for (size_t j = 0; j < M; j++) {
          get(i, j) = damping_factor * (get(i, j) / total) + (1.0f - damping_factor) / N;
        }
      }
    }
  }

  void print() const {
    std::cout << N << "x" << M << " Matrix:\n";
    for (size_t i = 0; i < N; ++i) {
      std::cout << "[";
      for (size_t j = 0; j < M; ++j) {
        if constexpr (std::is_floating_point_v<T>) {
          std::cout << std::fixed << std::setprecision(4) << std::setw(8) << p_data[i * M + j];
        } else {
          std::cout << std::setw(4) << p_data[i * M + j];
        }
        if (j < M - 1) {
          std::cout << ", ";
        }
      }
      std::cout << " ]\n";
    }
  }
};

} // namespace math
} // namespace detra
