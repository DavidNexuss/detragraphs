#pragma once

#include <vector>
#include <cmath>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <algorithms.hpp>
#include <detrarandom/random_sources.hpp>

#include "../position.hpp"

namespace graphs {
namespace position {

struct KamadaKawaiCreateInfo {
  int   max_iterations = 1000;
  float epsilon        = 1e-3f;
  bool  use_newton     = true;

  float L0              = 1.0f;
  float k_constant_base = 1.0f;
  float distance_power  = 2.0f;

  enum class Init {
    kCircle,
    kRandom,
    kFromOld
  } init = Init::kCircle;

  float init_circle_radius = 1.0f;
  float init_random_radius = 1.0f;
  float min_distance       = 1e-6f;
};

template <typename GraphT, typename RandomSource = random_sources::Standard>
PositionTable kamada_kawai(
  PositionTable&               old,
  const GraphT&                graph,
  const KamadaKawaiCreateInfo& info   = KamadaKawaiCreateInfo{},
  RandomSource                 source = {}) {
  using glm::vec2;
  using glm::vec3;
  using std::size_t;

  const size_t n = static_cast<size_t>(graph.getVertexCount());

  PositionTable result;
  result.positions.resize(n);

  if (n == 0) {
    return result;
  }

  if (info.init == KamadaKawaiCreateInfo::Init::kFromOld &&
      old.positions.size() == n) {

    result.positions = old.positions;

  } else if (info.init == KamadaKawaiCreateInfo::Init::kRandom) {

    const float two_pi = glm::two_pi<float>();

    for (size_t i = 0; i < n; ++i) {
      const float r = info.init_random_radius * std::sqrt(source.randf());
      const float a = two_pi * source.randf();
      result.positions[i] =
        vec3(std::cos(a) * r, std::sin(a) * r, 0.0f);
    }

  } else {

    const float two_pi = glm::two_pi<float>();

    for (size_t i = 0; i < n; ++i) {
      const float angle =
        (static_cast<float>(i) / static_cast<float>(n)) * two_pi;
      result.positions[i] =
        vec3(std::cos(angle), std::sin(angle), 0.0f) *
        info.init_circle_radius;
    }
  }

  constexpr float INF = std::numeric_limits<float>::infinity();

  std::vector<std::vector<float>> dist =
    algorithms::computeDistanceMatrix(graph);

  std::vector<std::vector<float>> L(n, std::vector<float>(n, 0.0f));
  std::vector<std::vector<float>> K(n, std::vector<float>(n, 0.0f));

  float maxd = 0.0f;
  for (size_t i = 0; i < n; ++i)
    for (size_t j = 0; j < n; ++j)
      if (dist[i][j] < INF && dist[i][j] > maxd)
        maxd = dist[i][j];

  if (maxd <= 0.0f) {
    maxd = 1.0f;
  }

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      if (i == j) continue;

      if (dist[i][j] < INF) {
        const float dij = std::max(dist[i][j], info.min_distance);
        L[i][j]         = info.L0 * (dij / maxd);
        K[i][j]         = info.k_constant_base /
          std::max(std::pow(dij, info.distance_power),
                   info.min_distance);
      } else {
        L[i][j] = info.L0;
        K[i][j] = 0.0f;
      }
    }
  }

  for (int iteration = 0; iteration < info.max_iterations; ++iteration) {

    float max_move = 0.0f;

    for (size_t m = 0; m < n; ++m) {

      float gx = 0.0f, gy = 0.0f;
      float gxx = 0.0f, gyy = 0.0f, gxy = 0.0f;

      const vec2 pos_m = vec2(result.positions[m]);

      for (size_t i = 0; i < n; ++i) {
        if (i == m || K[m][i] == 0.0f) continue;

        const vec2 pos_i = vec2(result.positions[i]);
        const vec2 delta = pos_m - pos_i;

        float dij = glm::length(delta);
        dij       = std::max(dij, info.min_distance);

        const float kij = K[m][i];
        const float lij = L[m][i];

        const float inv_d  = 1.0f / dij;
        const float common = kij * (1.0f - lij * inv_d);

        gx += common * delta.x;
        gy += common * delta.y;

        const float dij3   = dij * dij * dij;
        const float inv_d3 = 1.0f / std::max(dij3, 1e-12f);

        gxx += kij * (1.0f - lij * delta.y * delta.y * inv_d3);
        gyy += kij * (1.0f - lij * delta.x * delta.x * inv_d3);
        gxy += kij * lij * delta.x * delta.y * inv_d3;
      }

      float dx = 0.0f, dy = 0.0f;

      if (info.use_newton) {

        const float det = gxx * gyy - gxy * gxy;
        if (std::abs(det) < 1e-12f) continue;

        dx = (-gx * gyy + gy * gxy) / det;
        dy = (-gy * gxx + gx * gxy) / det;

      } else {

        constexpr float step = 0.1f;
        dx                   = -step * gx;
        dy                   = -step * gy;
      }

      result.positions[m].x += dx;
      result.positions[m].y += dy;

      const float move = std::sqrt(dx * dx + dy * dy);
      if (move > max_move) {
        max_move = move;
      }
    }

    if (max_move < info.epsilon) {
      break;
    }
  }

  return result;
}

} // namespace position
} // namespace graphs
