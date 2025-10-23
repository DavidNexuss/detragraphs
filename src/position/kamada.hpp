#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <queue>
#include <limits>
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

  enum class Init { kCircle,
                    kRandom,
                    kFromOld } init = Init::kCircle;

  float    init_circle_radius       = 1.0f;
  float    init_random_radius       = 1.0f;
  uint64_t floyd_warshall_threshold = 0;
  float    min_distance             = 1e-6f;
};

template <typename GraphT, typename RandomSource>
PositionTable kamada_kawai(PositionTable& old, const GraphT& graph, const KamadaKawaiCreateInfo& info = KamadaKawaiCreateInfo{}, RandomSource source = {}) {
  using glm::vec2;
  using std::size_t;

  const size_t  n = static_cast<size_t>(graph.getVertexCount());
  PositionTable result;
  result.positions.resize(n);

  if (n == 0) return result;

  if (info.init == KamadaKawaiCreateInfo::Init::kFromOld && old.positions.size() == n) {
    result.positions = old.positions;
  } else if (info.init == KamadaKawaiCreateInfo::Init::kRandom) {
    const float two_pi = glm::two_pi<float>();
    for (size_t i = 0; i < n; ++i) {
      float r             = info.init_random_radius * source.randf();
      float a             = two_pi * source.randf();
      result.positions[i] = glm::vec2(std::cos(a) * r, std::sin(a) * r);
    }
  } else {
    const float two_pi = glm::two_pi<float>();
    for (size_t i = 0; i < n; ++i) {
      float angle         = (static_cast<float>(i) / static_cast<float>(n)) * two_pi;
      result.positions[i] = glm::vec2(std::cos(angle), std::sin(angle)) * info.init_circle_radius;
    }
  }

  const float INF = std::numeric_limits<float>::infinity();

  std::vector<std::vector<float>> dist(n, std::vector<float>(n, INF));

  for (size_t s = 0; s < n; ++s) {
    std::queue<size_t> q;
    dist[s][s] = 0.0f;
    q.push(s);
    while (!q.empty()) {
      size_t u = q.front();
      q.pop();
      for (size_t v = 0; v < n; ++v) {
        if (dist[s][v] == INF && graph.isConnectedUndirected(u, v)) {
          dist[s][v] = dist[s][u] + 1.0f;
          q.push(v);
        }
      }
    }
  }

  std::vector<std::vector<float>> L(n, std::vector<float>(n, 0.0f));
  std::vector<std::vector<float>> K(n, std::vector<float>(n, 0.0f));

  float maxd = 0.0f;
  for (size_t i = 0; i < n; ++i)
    for (size_t j = 0; j < n; ++j)
      if (dist[i][j] < INF && dist[i][j] > maxd) maxd = dist[i][j];

  if (maxd <= 0.0f) maxd = 1.0f;

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      if (i == j) continue;
      if (dist[i][j] < INF) {
        L[i][j]     = info.L0 * (dist[i][j] / maxd);
        float denom = 1.0f;
        if (dist[i][j] > 0.0f)
          denom = std::pow(dist[i][j], info.distance_power);
        K[i][j] = info.k_constant_base / denom;
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

      const vec2 pos_m = result.positions[m];

      for (size_t i = 0; i < n; ++i) {
        if (i == m || K[m][i] == 0.0f) continue;

        const vec2& pos_i = result.positions[i];
        vec2        delta = pos_m - pos_i;
        float       dij   = glm::length(delta);
        if (dij < info.min_distance) dij = info.min_distance;

        float kij = K[m][i];
        float lij = L[m][i];

        float common = kij * (1.0f - lij / dij);
        gx += common * delta.x;
        gy += common * delta.y;

        float dij3       = dij * dij * dij;
        float term_denom = dij3 > 0.0f ? dij3 : 1e-12f;

        gxx += kij * (1.0f - lij * delta.y * delta.y / term_denom);
        gyy += kij * (1.0f - lij * delta.x * delta.x / term_denom);
        gxy += (kij * lij * delta.x * delta.y) / term_denom;
      }

      if (!info.use_newton) {
        constexpr float step = 0.1f;
        float           dx   = -step * gx;
        float           dy   = -step * gy;
        result.positions[m].x += dx;
        result.positions[m].y += dy;
        float move = std::sqrt(dx * dx + dy * dy);
        if (move > max_move) max_move = move;
        continue;
      }

      float denom = gxx * gyy - gxy * gxy;
      if (std::abs(denom) < 1e-12f) continue;

      float dx = (-gx * gyy - (-gy) * gxy) / denom;
      float dy = (gxx * (-gy) - gxy * (-gx)) / denom;

      result.positions[m].x += dx;
      result.positions[m].y += dy;

      float move = std::sqrt(dx * dx + dy * dy);
      if (move > max_move) max_move = move;
    }

    if (max_move < info.epsilon) break;
  }

  return result;
}

} // namespace position
} // namespace graphs
