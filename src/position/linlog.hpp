#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <cmath>
#include <detrarandom/random_sources.hpp>
#include "../position.hpp"
#include <Octree.hpp>

namespace graphs {
namespace position {
struct LinLogCreateInfo {
  float k_attract    = 1.0f;
  float k_repulse    = 1.0f;
  float step         = 0.1f;
  int   maxNeighbors = 10;
  float maxDistance  = 10.0f;
};
template <typename GraphT, typename RandomSource = random_sources::Standard>
void linlog_step(PositionTable& positions, const GraphT& graph, const LinLogCreateInfo& info = {}, RandomSource source = {}) {
  using namespace unibn;

  using glm::vec3;
  const size_t n = graph.getVertexCount();
  if (n == 0) return;
  if (positions.positions.size() != n) {
    positions.positions.resize(n);
    for (size_t i = 0; i < n; ++i) {
      positions.positions[i] = vec3(
        source.randf() - 0.5f,
        source.randf() - 0.5f,
        source.randf() - 0.5f);
    }
  }
  Octree<glm::vec3> tree;
  tree.initialize(positions.positions);

  std::vector<vec3> disp(n, vec3(0.0f));
  for (size_t i = 0; i < n; ++i) {
    std::vector<uint32_t> indices;

    tree.radiusNeighbors<L2Distance<glm::vec3>>(positions.positions[i], info.maxDistance, indices);

    if (indices.size() > info.maxDistance)
      indices.resize(info.maxDistance);

    for (const auto& p2index : indices) {
      auto& p2 = positions.positions[p2index];

      vec3  delta = positions.positions[i] - p2;
      float dist  = glm::length(delta);
      if (dist < 1e-6f) dist = 1e-6f;
      disp[i] += delta / dist * (info.k_repulse / (dist * dist));
    }
  }

  for (size_t u = 0; u < n; ++u) {
    const auto& neighbors = graph.getEdges(u);
    for (uint64_t v : neighbors) {
      if (v >= n) continue;
      vec3  delta = positions.positions[u] - positions.positions[v];
      float dist  = glm::length(delta);
      if (dist < 1e-6f) dist = 1e-6f;
      float forceMag = info.k_attract * std::log(dist + 1.0f);
      vec3  force    = -forceMag * (delta / dist);
      disp[u] += force;
      disp[v] -= force;
    }
  }
  for (size_t i = 0; i < n; ++i) {
    positions.positions[i] += disp[i] * info.step;
  }
}
} // namespace position
} // namespace graphs
