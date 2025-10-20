#pragma once
#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/Eigenvalues>
#include "flatmatrix.hpp"
#include "../stats.hpp"

namespace detra {
namespace math {

std::vector<std::complex<float>> eigenvalues(const FlatMatrix<float>& matrix) {
  size_t N = matrix.getRowCount();
  size_t M = matrix.getColumnCount();

  if (N != M) {
    return {};
  }

  using MatrixType = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

  Eigen::Map<MatrixType> eigen_matrix(matrix.data(), N, M);

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

float spectral_radius(const FlatMatrix<float>& matrix) {
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

void row_normalize(FlatMatrix<float>& matrix) {
  for (size_t i = 0; i < matrix.N; i++) {
    matrix[i][i] = 1.0;

    double total = 0.0;

    for (size_t j = 0; j < matrix.M; j++) {
      total += matrix[i][j];
    }

    for (size_t j = 0; j < matrix.M; j++) {
      matrix[i][j] = (matrix[i][j] / total);
    }
  }
}

std::vector<float> jacobi(FlatMatrix<float>& matrix) {
  if (matrix.N != matrix.M) return {};

  std::vector<float> result[2] = {
    std::vector<float>(matrix.N),
    std::vector<float>(matrix.N),
  };

  for (size_t i = 0; i < matrix.N; i++) {
    result[0][i] = 0.5;
    result[1][i] = 0.0;
  }

  float difference    = 0.0f;
  int   maxiterations = 1000;
  int   iteration     = 0;

  do {
    matrix.apply_inplace(result[0], result[1]);
    difference = graphs::stats::square_difference(result[0], result[1]);
    std::cout << iteration << " " << difference << std::endl;
    std::swap(result[0], result[1]);
    iteration++;
  } while (difference > 1e-7 && iteration < maxiterations);


  for (size_t i = 0; i < matrix.N; i++)
    std::cout << result[0][i] << " ";

  std::cout << std::endl;

  return result[0];
}
} // namespace math
} // namespace detra
