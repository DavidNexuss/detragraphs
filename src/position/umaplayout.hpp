#include <umappp/umappp.hpp>
#include "position.hpp"

namespace graphs {
namespace position {
struct UmapCreateInfo {
  int   n_neighbors  = 15;   // local vs global balance (like perplexity)
  float min_dist     = 0.1f; // how tightly packed clusters become
  int   n_epochs     = 500;  // optimization steps
  int   n_components = 3;    // 2 or 3 for your 3D hunger
};

template <typename GraphT, typename RandomSource>
PositionTable umap_layout(const GraphT& graph, const UmapCreateInfo& info = {}, RandomSource source = {}) {
  const size_t n = graph.getVertexCount();
  if (n < 2) return {};

  auto spectral = spectral_layout_3d(graph, {50.0f}); // pretend you extended to 50D

  std::vector<float> data(n * 50);
  for (size_t i = 0; i < n; ++i) {
    // fill data[i*50 + j] = spectral.positions[i][j % 3] or proper high-D
  }

  // 2. Run umappp
  umappp::Status     status;
  std::vector<float> embedding(n * info.n_components);

  umappp::Options opt;
  opt.num_neighbors  = info.n_neighbors;
  opt.min_dist       = info.min_dist;
  opt.num_epochs     = info.n_epochs;
  opt.num_dimensions = info.n_components;
  // opt.metric = ... (Euclidean default)

  status = umappp::initialize(/* your knn or raw data */, info.n_components, embedding.data(), opt);
  status = umappp::run(status);

  // 3. Map to your PositionTable (3D)
  PositionTable result;
  result.positions.resize(n);
  for (size_t i = 0; i < n; ++i) {
    result.positions[i] = glm::vec3(
      embedding[i * info.n_components + 0],
      embedding[i * info.n_components + 1],
      info.n_components >= 3 ? embedding[i * info.n_components + 2] : 0.0f);
  }

  return result;
}

} // namespace position
} // namespace graphs
