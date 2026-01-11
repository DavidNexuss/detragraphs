#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <detrarandom/random_sources.hpp>
#include <Octree.hpp>
#include <glm/glm.hpp>
#include "../position.hpp"

namespace graphs {
namespace position {


struct SpringLayoutCreateInfo {
  float L0           = 1.0f;
  float k_spring     = 0.1f;
  float k_repulse    = 0.01f;
  float speed        = 0.1f;
  float min_distance = 1e-6f;
};

template <typename GraphT, typename RandomSource = random_sources::XORand>
void initializePositions(PositionTable& positions, const GraphT& graph, RandomSource source = {}) {

  const size_t n = graph.getVertexCount();
  if (positions.positions.size() != n) {
    positions.positions.resize(n);
    for (size_t i = 0; i < n; ++i) {
      positions.positions[i] = glm::vec3(
        source.randf() - 0.5f,
        source.randf() - 0.5f,
        source.randf() - 0.5f);
    }
  }
}
template <typename GraphT, typename RandomSource = random_sources::Standard>
void spring_layout_step(PositionTable& positions, const GraphT& graph, const SpringLayoutCreateInfo& ci, RandomSource source = {}) {

  using glm::vec3;
  const size_t n = graph.getVertexCount();
  if (n == 0) return;

  initializePositions(positions, graph, random_sources::XORand());

  std::vector<vec3> disp(n, vec3(0.0f));

  for (size_t v = 0; v < n; ++v) {
    for (size_t u = v + 1; u < n; ++u) {
      vec3  delta = positions.positions[v] - positions.positions[u];
      float dist  = glm::length(delta);
      if (dist < 1e-6f) dist = 1e-6f;
      vec3 force = delta / dist * (ci.k_repulse / (dist * dist));
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
      vec3 force = -ci.k_spring * (dist - ci.L0) * (delta / dist);
      disp[v] += force;
      disp[u] -= force;
    }
  }

  for (size_t i = 0; i < n; ++i) {
    positions.positions[i] += disp[i] * ci.speed;
  }
}

struct SpringLayoutOctreeCreateInfo {
  float L0           = 1.0f;
  float k_spring     = 0.1f;
  float k_repulse    = 0.01f;
  float speed        = 0.1f;
  int   maxK         = 10;
  float maxDistance  = 10.0f;
  float min_distance = 1e-6f;
};

/**
 * Single-step 3D spring layout using Octree for efficient repulsion
 * Positions are stored in PositionTable and updated incrementally.
 */
template <typename GraphT, typename RandomSource = random_sources::Standard>
void spring_layout_step_octree(PositionTable& positions, const GraphT& graph, const SpringLayoutOctreeCreateInfo& ci, RandomSource source = {}) {

  using namespace unibn;

  using glm::vec3;
  const size_t n = graph.getVertexCount();
  if (n == 0) return;

  initializePositions(positions, graph);

  Octree<glm::vec3> tree;
  tree.initialize(positions.positions);

  std::vector<vec3> disp(n, vec3(0.0f));

  for (size_t v = 0; v < n; ++v) {
    std::vector<uint32_t> neighbors;
    tree.radiusNeighbors<L2Distance<glm::vec3>>(positions.positions[v], ci.maxDistance, neighbors);
    if (neighbors.size() > ci.maxK) neighbors.resize(ci.maxK);

    for (const auto& index : neighbors) {
      glm::vec3 other = positions.positions[index];

      vec3  delta = positions.positions[v] - other;
      float dist  = glm::length(delta);
      if (dist < 1e-6f) dist = 1e-6f;
      disp[v] += delta / dist * (ci.k_repulse / (dist * dist));
    }
  }

  for (size_t v = 0; v < n; ++v) {
    const auto& neighbors = graph.getEdges(v);
    for (auto u : neighbors) {
      if (u >= n) continue;
      vec3  delta = positions.positions[v] - positions.positions[u];
      float dist  = glm::length(delta);
      if (dist < 1e-6f) dist = 1e-6f;
      vec3 force = -ci.k_spring * (dist - ci.L0) * (delta / dist);
      disp[v] += force;
      disp[u] -= force;
    }
  }

  for (size_t i = 0; i < n; ++i) {
    positions.positions[i] += disp[i] * ci.speed;
  }
}

} // namespace position
} // namespace graphs
