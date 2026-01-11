#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <unordered_map>
#include <algorithm>
#include <random>
#include "../position.hpp"
#include "../metrics.hpp"
#include <detrarandom/random_sources.hpp>

namespace graphs {
namespace position {

struct RadialPlacementCreateInfo {
  float hub_degree_threshold_percentile = 90.0f; // top X% by degree become gravitational centers
  float peer_attraction_strength        = 0.8f;  // strength of pull toward dominant hubs
  float hub_repulsion_strength          = 3.0f;  // fierce mutual repulsion among the sovereigns
  float hub_spring_length               = 6.0f;  // ideal radial distance between connected hubs
  int   hub_force_iterations            = 400;   // how long the gods wrestle before settling
  float min_distance                    = 0.02f;
  float color_fade_power                = 1.4f; // how quickly lesser connections dim
  bool  randomize_peer_initial_offset   = true;
  float peer_cloud_spread               = 1.8f; // how widely the devotees may orbit their lords
};

template <typename GraphT, typename RandomSource = random_sources::Standard>
PositionTable radialPlacement(PositionTable& old, const GraphT& graph, const RadialPlacementCreateInfo& info = RadialPlacementCreateInfo{}, RandomSource source = {}) {

  using vec3     = glm::vec3;
  const size_t n = graph.getVertexCount();

  if (n == 0) return {};

  auto degrees = metrics::degree_sequence(graph);

  std::vector<size_t> order(n);
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(), [&](size_t a, size_t b) {
    return degrees[a] > degrees[b];
  });

  size_t hub_threshold_idx = static_cast<size_t>(n * (100.f - info.hub_degree_threshold_percentile) / 100.f);

  std::vector<bool>   is_hub(n, false);
  std::vector<size_t> hubs, peers;

  for (size_t i = 0; i < n; ++i) {
    size_t v = order[i];
    if (i < hub_threshold_idx) {
      is_hub[v] = true;
      hubs.push_back(v);
    } else {
      peers.push_back(v);
    }
  }

  const size_t n_hubs = hubs.size();

  std::vector<vec3> hub_pos(n_hubs, vec3(0.f));

  for (size_t i = 0; i < n_hubs; ++i) {
    float theta = source.randf(0.0f, glm::pi<float>());
    float phi   = std::acos(1.f - 2.f * source.randf(0.1f, 1.0f));
    float r     = std::cbrt(source.randf(0.1f, 1.0f)) * 5.f;
    hub_pos[i]  = vec3(
      r * std::sin(phi) * std::cos(theta),
      r * std::sin(phi) * std::sin(theta),
      r * std::cos(phi));
  }

  const float hub_k = info.hub_spring_length;
  for (int iter = 0; iter < info.hub_force_iterations; ++iter) {
    std::vector<vec3> forces(n_hubs, vec3(0.f));

    for (size_t i = 0; i < n_hubs; ++i) {
      size_t u  = hubs[i];
      vec3   pu = hub_pos[i];

      for (size_t j = 0; j < n_hubs; ++j) {
        if (i == j) continue;
        vec3  delta = pu - hub_pos[j];
        float d2    = glm::dot(delta, delta);
        if (d2 < info.min_distance * info.min_distance) d2 = info.min_distance * info.min_distance;
        float d = std::sqrt(d2);
        forces[i] += (delta / d) * (info.hub_repulsion_strength / d2);
      }

      auto neighbors = graph.getEdges(u);
      for (uint64_t v_ : neighbors) {
        size_t v = static_cast<size_t>(v_);
        if (!is_hub[v]) continue;
        auto it = std::find(hubs.begin(), hubs.end(), v);
        if (it == hubs.end()) continue;
        size_t j = std::distance(hubs.begin(), it);

        vec3  delta = hub_pos[j] - pu;
        float d     = glm::length(delta);
        if (d < info.min_distance) d = info.min_distance;
        float displacement = d - hub_k;
        forces[i] += (delta / d) * displacement * 0.5f;
      }
    }

    float max_force = 0.f;
    for (size_t i = 0; i < n_hubs; ++i) {
      hub_pos[i] += forces[i] * 0.075f;
      float len = glm::length(forces[i]);
      if (len > max_force) max_force = len;
    }

    if (max_force < 0.015f) break;
  }

  std::vector<vec3> final_pos(n);

  std::unordered_map<size_t, vec3> hub_map;
  for (size_t i = 0; i < n_hubs; ++i) {
    hub_map[hubs[i]]   = hub_pos[i];
    final_pos[hubs[i]] = hub_pos[i];
  }

  std::vector<float> max_hub_degree(n_hubs);
  for (size_t i = 0; i < n_hubs; ++i)
    max_hub_degree[i] = static_cast<float>(degrees[hubs[i]]);

  float global_max_hub_deg = *std::max_element(max_hub_degree.begin(), max_hub_degree.end());

  for (size_t p : peers) {
    vec3  center(0.f);
    float total_weight = 0.f;

    auto neighbors = graph.getEdges(p);
    for (uint64_t v_ : neighbors) {
      size_t v = static_cast<size_t>(v_);
      if (!is_hub[v]) continue;

      float w = static_cast<float>(degrees[v]) / global_max_hub_deg;
      w       = std::pow(w, info.color_fade_power);
      center += hub_map[v] * w;
      total_weight += w;
    }

    if (total_weight > 0.f)
      center /= total_weight;
    else
      center = vec3(0.f);

    float spread = info.peer_cloud_spread * (1.f + source.randf() * 0.7f);
    vec3  offset(
      (source.randf() - 0.5f) * spread,
      (source.randf() - 0.5f) * spread,
      (source.randf() - 0.5f) * spread);

    final_pos[p] = center + offset;
  }

  PositionTable result;
  result.positions.swap(final_pos);

  return result;
}

} // namespace position
} // namespace graphs
