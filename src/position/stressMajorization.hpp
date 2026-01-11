#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <cmath>
#include <limits>
#include <detrarandom/random_sources.hpp>
#include <algorithms.hpp>
#include "../position.hpp"

namespace graphs {
namespace position {

struct StressMajorizationCreateInfo {
  float L0                          = 1.0f;
  float weight_power                = 2.0f;
  int   max_iterations              = 200;
  float epsilon                     = 1e-4f;
  enum class Init { kCircle,
                    kRandom,
                    kFromOld } init = Init::kCircle;
  float init_circle_radius          = 1.0f;
  float init_random_radius          = 1.0f;
  bool  use_apsp_cache              = true;
};

template <typename GraphT, typename RandomSource = random_sources::Standard>
void stress_majorization_step(PositionTable& positions, const GraphT& graph, const StressMajorizationCreateInfo& info = {}, RandomSource source = {}) {
  using glm::vec3;
  using std::size_t;
  const size_t n = graph.getVertexCount();

  if (n < 2) return;

  if (positions.positions.size() != n) {
    positions.positions.resize(n, vec3(0.0f));
    const float two_pi = M_PI * 2.0f;
    if (info.init == StressMajorizationCreateInfo::Init::kRandom) {
      for (size_t i = 0; i < n; ++i) {
        float r                = info.init_random_radius * source.randf();
        float a                = two_pi * source.randf();
        positions.positions[i] = vec3(std::cos(a) * r, std::sin(a) * r, 0.0f);
      }
    } else if (info.init == StressMajorizationCreateInfo::Init::kCircle) {
      for (size_t i = 0; i < n; ++i) {
        float a                = (float(i) / float(n)) * two_pi;
        positions.positions[i] = vec3(std::cos(a), std::sin(a), 0.0f) * info.init_circle_radius;
      }
    }
  }


  const float                     INF  = std::numeric_limits<float>::infinity();
  std::vector<std::vector<float>> dist = algorithms::computeDistanceMatrix(graph);

  float max_d = 0.0f;
  for (size_t i = 0; i < n; ++i)
    for (size_t j = 0; j < n; ++j)
      if (dist[i][j] < INF && dist[i][j] > max_d)
        max_d = dist[i][j];
  if (max_d <= 0.0f) max_d = 1.0f;

  auto L = [&dist, &info, max_d](size_t i, size_t j) -> float {
    const float INF = std::numeric_limits<float>::infinity();
    if (dist[i][j] >= INF) return 0.0f;
    return info.L0 * (dist[i][j] / max_d);
  };

  auto w = [&dist, &info](size_t i, size_t j) -> float {
    const float INF = std::numeric_limits<float>::infinity();
    if (dist[i][j] >= INF || dist[i][j] <= 0.0f) return 0.0f;
    return 1.0f / std::pow(dist[i][j], info.weight_power);
  };

  std::vector<vec3> new_pos = positions.positions;

  float prev_stress = INF;

  for (int iter = 0; iter < info.max_iterations; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      vec3  num(0.0f);
      float denom = 0.0f;

      for (size_t j = 0; j < n; ++j) {
        if (i == j) continue;
        float wij = w(i, j);
        if (wij <= 0.0f) continue;

        float lij   = L(i, j);
        vec3  delta = positions.positions[j] - positions.positions[i];
        float dij   = glm::length(delta);
        if (dij < 1e-8f) dij = 1e-8f;

        num += wij * lij * (positions.positions[j]);
        denom += wij * lij / dij;
      }

      if (denom > 1e-8f) {
        new_pos[i] = num / denom;
      }
    }

    float stress = 0.0f;
    for (size_t i = 0; i < n; ++i) {
      for (size_t j = i + 1; j < n; ++j) {
        float wij = w(i, j);
        if (wij <= 0.0f) continue;
        float lij = L(i, j);
        float dij = glm::length(positions.positions[i] - positions.positions[j]);
        if (dij < 1e-8f) dij = 1e-8f;
        stress += wij * (dij - lij) * (dij - lij);
      }
    }

    positions.positions.swap(new_pos);

    if (std::abs(prev_stress - stress) < info.epsilon * (stress + 1.0f)) {
      break;
    }
    prev_stress = stress;
  }
}

} // namespace position
} // namespace graphs
