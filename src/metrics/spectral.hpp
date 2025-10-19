#pragma once
#include <cstddef>
#include "../math/flatmatrix.hpp"
#include "../parser.hpp"
#include "../metrics.hpp"

namespace graphs {

namespace metrics {

template <typename GraphT>

detra::math::FlatMatrix<float> laplacian_matrix(const GraphT& inputmatrix) {
  detra::math::FlatMatrix<float> result = parser::convert_matrix(inputmatrix);

  auto degree_sequence = metrics::degree_sequence<GraphT>(inputmatrix);

  for (size_t i = 0; i < degree_sequence.size(); i++) {
    result[i][i] = degree_sequence[i];
  }

  return result;
}

} // namespace metrics
} // namespace graphs
