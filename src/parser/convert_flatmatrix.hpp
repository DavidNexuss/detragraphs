#pragma once
#include "../math/flatmatrix.hpp"
#include "../graph.hpp"
#include "../graphbackend/adjacencymatrix.hpp"

namespace graphs {
namespace parser {

template <typename G>
detra::math::FlatMatrix<float> convert_matrix(const G& graph) {
  size_t                         N = graph.getVertexCount();
  detra::math::FlatMatrix<float> matrix(N, N);
  for (size_t i = 0; i < N; i++) {
    for (size_t j : graph.getEdges(i)) {
      matrix[i][j] = 1.0f;
    }
  }

  return matrix;
}

inline detra::math::FlatMatrix<float> convert_matrix(const Graph<backends::AdjacencyMatrixFlat<float>>& graph) {
  size_t N = graph.getVertexCount();
  return detra::math::FlatMatrix<float>(N, N, graph.data.mat);
}

inline detra::math::FlatMatrix<float> convert_matrix(Graph<backends::AdjacencyMatrixFlat<float>>&& graph) {
  size_t N = graph.getVertexCount();
  return detra::math::FlatMatrix<float>(N, N, std::move(graph.data.mat));
}

} // namespace parser
} // namespace graphs