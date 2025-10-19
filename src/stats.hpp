#pragma once
#include <vector>

namespace graphs {
namespace stats {

float mean(const std::vector<float>& values) {
  double value = 0.0f;
  for (size_t i = 0; i < values.size(); i++) {
    value += values[i];
  }
  return value / values.size();
}


float dotprod(const std::vector<float>& a, const std::vector<float>& b) {
  if (a.size() != b.size()) return 0.0;
  double value = 0.0;
  for (size_t i = 0; i < a.size(); i++) {
    value += a[i] * b[i];
  }
  return value;
}

float sd(const std::vector<float>& x) {
  double avg       = mean(x);
  double deviation = 0.0;

  for (size_t i = 0; i < x.size(); i++) {
    deviation += (i - avg) * (i - avg);
  }
  return std::sqrt(deviation / x.size());
}

} // namespace stats
} // namespace graphs
