#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <algorithm>
#include <numeric>
#include "../position.hpp"

namespace graphs {
namespace position {

struct DegreeGrowthCreateInfo {
  float base_radius     = 1.0f; // Initial spacing scale
  float degree_exponent = 1.0f; // Controls geometric decay
  float angular_jitter  = 0.3f; // Branch separation strength

  enum class Init {
    kOrigin,
    kCircle,
    kFromOld
  } init = Init::kOrigin;

  float init_circle_radius = 5.0f;

  float min_radius = 1e-4f;
};

template <typename GraphT, typename RandomSource = random_sources::Standard>
PositionTable degree_growth(PositionTable& old, const GraphT& graph, const DegreeGrowthCreateInfo& info = DegreeGrowthCreateInfo{}, RandomSource source = {}) {
  using glm::vec3;
  using std::size_t;

  size_t        n = graph.getVertexCount();
  PositionTable result;
  result.positions.resize(n, vec3(0.0f));

  if (n == 0) return result;

  std::vector<size_t> order(n);
  std::iota(order.begin(), order.end(), 0);

  std::vector<uint32_t> degree(n);
  for (size_t i = 0; i < n; ++i)
    degree[i] = graph.getEdgeCount(i);

  std::sort(order.begin(), order.end(),
            [&](size_t a, size_t b) {
              return degree[a] > degree[b];
            });

  uint32_t max_degree = degree[order[0]];

  std::vector<char> placed(n, 0);

  if (info.init == DegreeGrowthCreateInfo::Init::kFromOld &&
      old.positions.size() == n) {
    result = old;
    for (size_t i = 0; i < n; ++i)
      placed[i] = 1;
    return result;
  }

  if (info.init == DegreeGrowthCreateInfo::Init::kCircle) {
    float step = glm::two_pi<float>() / float(n);
    for (size_t i = 0; i < n; ++i) {
      float a = step * i;
      result.positions[i] =
        vec3(std::cos(a), 0.0f, std::sin(a)) * info.init_circle_radius;
    }
  }

  result.positions[order[0]] = vec3(0.0f);
  placed[order[0]]           = 1;

  for (size_t k = 1; k < n; ++k) {
    size_t v = order[k];

    vec3 centroid(0.0f);
    int  count = 0;

    for (uint64_t u : graph.getEdges(v)) {
      if (placed[u]) {
        centroid = centroid + result.positions[u];
        count++;
      }
    }

    if (count > 0)
      centroid /= float(count);

    float r =
      info.base_radius *
      std::pow(
        (float(degree[v]) + 1.0f) /
          (float(max_degree) + 1.0f),
        info.degree_exponent);

    r = std::max(r, info.min_radius);

    // Random direction
    float theta = source.randf() * glm::two_pi<float>();
    float z     = 2.0f * source.randf() - 1.0f;
    float s     = std::sqrt(1.0f - z * z);

    vec3 dir(
      s * std::cos(theta),
      z,
      s * std::sin(theta));

    // Angular jitter to prevent crossings
    dir += info.angular_jitter * vec3(source.randf() - 0.5f, source.randf() - 0.5f, source.randf() - 0.5f);

    dir = glm::normalize(dir);

    result.positions[v] = centroid + dir * r;
    placed[v]           = 1;
  }

  return result;
}
} // namespace position
} // namespace graphs
