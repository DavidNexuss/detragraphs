#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace graphs {
namespace position {

/**
 * @brief Position table for nodes in a graph (each node gets an X and Y coordinate)
 */
struct PositionTable {
  std::vector<glm::vec2> positions;
};

} // namespace position
} // namespace graphs
