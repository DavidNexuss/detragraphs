#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <cmath>
#include <detrarandom/random_sources.hpp>
#include "../position.hpp"

namespace graphs {
namespace position {

/*
 * Another attemp at force directed graph rendering, using Coulomb-like force.
 * @param positions The initial positions. This is only one step of the algorithm.
 * @param graph The graph.
 * @param source The source of randomness.
 * @param speed The update speed.
 */
template <typename GraphT, typename RandomSource = random_sources::Standard>
void force_directed_3d_step(PositionTable& positions, const GraphT& graph, RandomSource source = {}, float speed = 0.1f) {
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

  const float k = std::cbrt(1.0f / static_cast<float>(n));

  std::vector<vec3> disp(n, vec3(0.0f));

  for (size_t v = 0; v < n; ++v) {
    for (size_t u = v + 1; u < n; ++u) {
      vec3  delta = positions.positions[v] - positions.positions[u];
      float dist  = glm::length(delta);
      if (dist < 1e-6f) dist = 1e-6f;
      vec3 force = delta / dist * (k * k / dist);
      disp[v] += force;
      disp[u] -= force;
    }
  }

  for (size_t v = 0; v < n; ++v) {
    const auto& neighbors = graph.getEdges(v);
    for (auto u : neighbors) {
      if (u >= n) continue;
      vec3  delta = positions.positions[v] - positions.positions[u];
      float dist  = glm::length(delta);
      if (dist < 1e-6f) dist = 1e-6f;
      vec3 force = delta / dist * (dist * dist / k);
      disp[v] -= force;
      disp[u] += force;
    }
  }

  for (size_t i = 0; i < n; ++i) {
    positions.positions[i] += disp[i] * speed;
  }
}

} // namespace position
} // namespace graphs
