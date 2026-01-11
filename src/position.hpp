#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace graphs {
namespace position {

/**
 * @brief Position table for nodes in a graph (each node gets an X and Y coordinate)
 */
struct PositionTable {
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> colors;
};

} // namespace position
} // namespace graphs
