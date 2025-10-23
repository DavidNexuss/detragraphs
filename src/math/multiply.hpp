#pragma once
#include "../stats.hpp"
#include <iostream>

namespace detra {
namespace math {

template <typename Matrix>
inline typename Matrix::Vector jacobi(Matrix& matrix) {
  using Vector = typename Matrix::Vector;

  if (matrix.N != matrix.M) return {};

  Vector result[2] = {
    Vector(matrix.N),
    Vector(matrix.N),
  };

  for (size_t i = 0; i < matrix.N; i++) {
    result[0][i] = 1.0f / float(matrix.N);
    result[1][i] = 0.0f;
  }

  float difference    = 0.0f;
  int   maxiterations = 1000;
  int   iteration     = 0;

  do {
    matrix.apply_inplace(result[0], result[1]);
    difference = graphs::stats::square_difference(result[0], result[1]);
    std::cerr << iteration << " " << difference << std::endl;
    std::swap(result[0], result[1]);
    iteration++;
  } while (difference > 1e-7f && iteration < maxiterations);

  std::cerr << "Iterations: " << iteration << std::endl;
  std::cerr << "Difference: " << difference << std::endl;

  return result[0];
}


} // namespace math
} // namespace detra
