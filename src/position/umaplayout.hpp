#include "position.hpp"

namespace graphs {
namespace position {
struct UmapCreateInfo {
};

template <typename GraphT, typename RandomSource>
PositionTable umap_layout(const GraphT& graph, const UmapCreateInfo& info = {}, RandomSource source = {}) {
  // Experimental placeholder for a UMAP-based layout.
  // Intended to re-embed positions from another layout to emphasize
  // higher-level structure. Not implemented.
}

} // namespace position
} // namespace graphs
