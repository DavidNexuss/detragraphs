#pragma once
#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/Eigenvalues>
#include "flatmatrix.hpp"
#include "../util/vector_utils.hpp"
#include "../stats.hpp"

namespace detra {
namespace math {

inline std::vector<std::complex<float>> eigenvalues(const FlatMatrix<float>& matrix) {
  size_t N = matrix.getRowCount();
  size_t M = matrix.getColumnCount();

  if (N != M) {
    return {};
  }

  using MatrixType = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

  Eigen::Map<const MatrixType> eigen_matrix(matrix.data(), N, M);

  Eigen::EigenSolver<MatrixType> solver(eigen_matrix);

  if (solver.info() != Eigen::Success) {
    return {};
  }

  const Eigen::VectorXcf& eigen_values_eigen = solver.eigenvalues();

  std::vector<std::complex<float>> result_eigenvalues;
  result_eigenvalues.reserve(eigen_values_eigen.size());

  for (int i = 0; i < eigen_values_eigen.size(); ++i) {
    result_eigenvalues.push_back(eigen_values_eigen(i));
  }

  return result_eigenvalues;
}

inline float spectral_radius(const FlatMatrix<float>& matrix) {
  std::vector<std::complex<float>> evals = eigenvalues(matrix);

  if (evals.empty()) {
    return 0.0f;
  }

  float max_modulus = 0.0f;

  for (const auto& lambda : evals) {
    float modulus = std::abs(lambda);

    if (modulus > max_modulus) {
      max_modulus = modulus;
    }
  }

  return max_modulus;
}

inline void row_normalize(FlatMatrix<float>& matrix) {
  int   N              = matrix.getRowCount();
  int   M              = matrix.getColumnCount();
  float damping_factor = 1.0f;

  for (size_t i = 0; i < N; i++) {
    float total = 0.0f;
    for (size_t j = 0; j < M; j++) {
      total += matrix[i][j];
    }
    if (total == 0.0f) {
      for (size_t j = 0; j < M; j++) {
        matrix[i][j] = 1.0f / N;
      }
    } else {
      for (size_t j = 0; j < M; j++) {
        matrix[i][j] = damping_factor * (matrix[i][j] / total) + (1.0f - damping_factor) / N;
      }
    }
  }
}

inline std::vector<float> jacobi(FlatMatrix<float>& matrix) {
  if (matrix.N != matrix.M) return {};

  std::vector<float> result[2] = {
    std::vector<float>(matrix.N),
    std::vector<float>(matrix.N),
  };

  for (size_t i = 0; i < matrix.N; i++) {
    result[0][i] = 1.0 / float(matrix.N);
    result[1][i] = 0.0;
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
  } while (difference > 1e-7 && iteration < maxiterations);

  std::cerr << "Iterations: " << iteration << std::endl;
  std::cerr << "Difference: " << difference << std::endl;

  return result[0];
}
} // namespace math
} // namespace detra
