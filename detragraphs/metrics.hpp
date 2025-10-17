#pragma once
#include "graph.hpp"
#include <vector>
namespace graphs {

namespace metrics {

template <typename GraphT>
std::vector<uint32_t> degree_sequence(const GraphT& graph) {
  int N = graph.getVertexCount();

  std::vector<uint32_t> degrees(N);

  for (int i = 0; i < N; i++) {
    degrees[i] = graph.getEdgeCount(i);
  }

  return degrees;
}

} // namespace metrics
} // namespace graphs
