#pragma once
#include <vector>
#include <cmath>
#include <limits>

namespace graphs {
namespace stats {

inline float mean(const std::vector<float>& values) {
  double value = 0.0f;
  for (size_t i = 0; i < values.size(); i++) {
    value += values[i];
  }
  return value / values.size();
}


inline float dotprod(const std::vector<float>& a, const std::vector<float>& b) {
  if (a.size() != b.size()) return 0.0;
  double value = 0.0;
  for (size_t i = 0; i < a.size(); i++) {
    value += a[i] * b[i];
  }
  return value;
}

inline float sd(const std::vector<float>& x) {
  double avg       = mean(x);
  double deviation = 0.0;

  for (size_t i = 0; i < x.size(); i++) {
    deviation += (i - avg) * (i - avg);
  }
  return std::sqrt(deviation / x.size());
}

inline float square_difference(const std::vector<float>& a, const std::vector<float>& b) {
  if (a.size() != b.size()) return std::numeric_limits<float>::max();
  double accum = 0.0;
  for (size_t i = 0; i < a.size(); i++) {
    accum += std::pow(a[i] - b[i], 2.0);
  }
  return accum;
}

} // namespace stats
} // namespace graphs
