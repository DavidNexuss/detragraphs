#pragma once
#include <vector>
#include <cmath>

namespace detra {

namespace math {

struct SparseVector {
  int                                M;
  std::vector<std::pair<int, float>> data;

  SparseVector() {}
  SparseVector(int _M) :
    M(_M) {}
};

SparseVector toSparseVector(const std::vector<float>& x, float tol = 1e-6f) {
  SparseVector sx;
  sx.M = static_cast<int>(x.size());
  for (int i = 0; i < sx.M; ++i)
    if (std::fabs(x[i]) > tol)
      sx.data.emplace_back(i, x[i]);
  return sx;
}

} // namespace math
} // namespace detra
