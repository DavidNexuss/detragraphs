#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../position.hpp"
#include "../algorithms.hpp"
#include <detrarandom/random_sources.hpp>

namespace graphs {
namespace position {

struct Shell3DCreateInfo {
  uint64_t source       = 0;
  float    layerSpacing = 1.0f;
  float    jitter       = 0.0f;
};

template <typename GraphT, typename RandomSource = random_sources::Standard>
PositionTable shell3d_layout_bfs(const GraphT& graph, const Shell3DCreateInfo& info = {}, RandomSource sourceRandom = {}) {

  using namespace graphs::algorithms;

  using glm::vec3;
  const size_t  n = static_cast<size_t>(graph.getVertexCount());
  PositionTable result;
  result.positions.resize(n, vec3(0.0f));

  if (n == 0 || info.source >= n) return result;

  std::vector<WalkResult> bfsResult = walk_bfs(graph, info.source);

  int maxLayer = 0;
  for (const auto& w : bfsResult)
    if (w.distanceFromSource > maxLayer) maxLayer = w.distanceFromSource;

  std::vector<int> nodesPerLayer(maxLayer + 1, 0);
  for (const auto& w : bfsResult)
    nodesPerLayer[w.distanceFromSource]++;

  std::vector<int> layerIndex(maxLayer + 1, 0);

  for (const auto& w : bfsResult) {
    int layer        = w.distanceFromSource;
    int idxInLayer   = layerIndex[layer]++;
    int countInLayer = nodesPerLayer[layer];

    float radius = info.layerSpacing * float(layer);

    float phi   = acos(-1.0f + 2.0f * (idxInLayer + 0.5f) / countInLayer);
    float theta = 2.0f * M_PI * (idxInLayer + 0.5f) / countInLayer;

    vec3 pos;
    pos.x = radius * sin(phi) * cos(theta);
    pos.y = radius * sin(phi) * sin(theta);
    pos.z = radius * cos(phi);

    // optional jitter
    if (info.jitter > 0.0f) {
      pos.x += (sourceRandom.randf() - 0.5f) * info.jitter;
      pos.y += (sourceRandom.randf() - 0.5f) * info.jitter;
      pos.z += (sourceRandom.randf() - 0.5f) * info.jitter;
    }

    result.positions[w.vertexId] = pos;
  }

  return result;
}

} // namespace position
} // namespace graphs
