#pragma once
#include "../stats.hpp"
#include <iostream>

namespace detra {
namespace math {

/* 
* Computes the power method for finding the stationary vector of a column stochastic Markov chain.
* This method does not check if the matrix spectral radius is less than 1.0!
* @param matrix The Markov chain in column stochastic (each column must sum 1.0 or less)
* @returns The stationary vector
* */
template <typename Matrix>
inline typename Matrix::Vector jacobi(const Matrix& matrix, float epsilon = 1e-7f, int maxiterations = 1000, typename Matrix::Vector current = {}, typename Matrix::Vector constant = {}) {
  using Vector = typename Matrix::Vector;

  if (matrix.N == 0) return {};
  if (matrix.M == 0) return {};

  if (current.size() == 0) {
    current = typename Matrix::Vector(matrix.N, 1.0f / (float)matrix.N);
  }

  Vector next = current;

  float difference = 0.0f;
  int   iteration  = 0;

  do {
    matrix.apply_inplace(current, next);

    if (constant.size() != 0) {
      for (size_t i = 0; i < constant.size(); i++) next[i] += constant[i];
    }

    difference = graphs::stats::square_difference(current, next);
    std::swap(current, next);
    iteration++;
  } while (difference > epsilon && iteration < maxiterations);

  std::cerr << "[Jacobi] Iterations: " << iteration
            << "  Delta: " << difference << std::endl;

  return current;
}

template <typename Matrix>
inline typename Matrix::Vector gauss(const Matrix& matrix, float epsilon = 1e-7f, int maxiterations = 1000, typename Matrix::Vector current = {}) {
  using Vector = typename Matrix::Vector;

  if (matrix.N == 0) return {};
  if (matrix.M == 0) return {};

  if (current.size() == 0) {
    current = typename Matrix::Vector(matrix.N, 1.0f / (float)matrix.N);
  }

  Vector next = current;

  float difference = 0.0f;
  int   iteration  = 0;

  do {
    matrix.apply_inplace(next);
    difference = graphs::stats::square_difference(current, next);
    current    = next;
    iteration++;
  } while (difference > epsilon && iteration < maxiterations);

  std::cerr << "[Jacobi] Iterations: " << iteration
            << "  Delta: " << difference << std::endl;

  return current;
}


} // namespace math
} // namespace detra
