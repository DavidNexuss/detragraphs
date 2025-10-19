#pragma once
#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/Eigenvalues>
#include "flatmatrix.hpp"

namespace detra {
namespace math {

std::vector<std::complex<float>> eigenvalues(FlatMatrix<float>& matrix) {
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
} // namespace math
} // namespace detra
