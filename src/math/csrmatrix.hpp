#pragma once
#include <vector>

namespace detra {
namespace math {

/**
* @brief Compressed Sparse Row (CSR) format for matrix A.
* This is the standard row-major format, good for y = A * x.
*/
struct CSRMatrix {
  int                N, M;
  std::vector<int>   row_ptr;
  std::vector<int>   col_idx;
  std::vector<float> val;

  using Vector = std::vector<float>;

  inline void apply_inplace(const std::vector<float>& x, std::vector<float>& y) {
    y.assign(N, 0.0f);
    for (int i = 0; i < N; ++i)
      for (int k = row_ptr[i]; k < row_ptr[i + 1]; ++k)
        y[i] += val[k] * x[col_idx[k]];
  }
};
} // namespace math
} // namespace detra
