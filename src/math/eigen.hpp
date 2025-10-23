#pragma once
#include <vector>
#include "flatmatrix.hpp"
#include "../stats.hpp"

namespace detra {
namespace math {

/*  
inline std::vector<std::complex<float>> eigenvalues(const FlatMatrix<float>& matrix) {
  const size_t N = matrix.getRowCount();
  const size_t M = matrix.getColumnCount();

  if (N != M) {
    return {};
  }

  using MatrixType = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

  Eigen::Map<const MatrixType> eigen_matrix(matrix.data(), N, M);

  Eigen::EigenSolver<MatrixType> solver(eigen_matrix, false);

  if (solver.info() != Eigen::Success) {
    return {};
  }

  const auto& eigen_values = solver.eigenvalues();

  std::vector<std::complex<float>> result;
  result.reserve(eigen_values.size());

  for (Eigen::Index i = 0; i < eigen_values.size(); ++i) {
    result.emplace_back(eigen_values[i]);
  }

  return result;
}
*/

} // namespace math
} // namespace detra
