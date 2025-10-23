#pragma once
#include <vector>
#include "sparsevector.hpp"
#include "csrmatrix.hpp"
#include <algorithm>

namespace detra {
namespace math {

struct SparseInverseDirectory {
  int N, M;

  using Vector = SparseVector;

  std::vector<std::vector<std::pair<int, float>>> inv_dir;

  inline void apply_inplace(const SparseVector& x, std::vector<float>& y) {
    y.assign(N, 0.0f);
    for (const auto& [col, xv] : x.data) {
      if (col >= M)
        continue;
      for (const auto& [row, val] : inv_dir[col])
        y[row] += val * xv;
    }
  }
};

SparseInverseDirectory buildInverseDirectory(const CSRMatrix& A) {
  SparseInverseDirectory id;
  id.N = A.N;
  id.M = A.M;
  id.inv_dir.resize(A.M);

  for (int i = 0; i < A.N; ++i)
    for (int k = A.row_ptr[i]; k < A.row_ptr[i + 1]; ++k)
      id.inv_dir[A.col_idx[k]].emplace_back(i, A.val[k]);

  for (auto& col : id.inv_dir) std::sort(col.begin(), col.end(), [](auto& a, auto& b) { return a.first < b.first; });

  return id;
}
} // namespace math
} // namespace detra
