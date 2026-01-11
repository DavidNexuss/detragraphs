#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Eigen/Dense>
#include "../position.hpp"
#include "../graph.hpp"

namespace graphs {
namespace position {

struct SpectralCreateInfo {
  float scale  = 1.0f;
  bool  center = true;
};

/*
 * This is the spectral graph layout in 2D using the unnormalized Laplacian.
 * This method embeds vertices by projecting them onto the lowest non-zero eigenmodes of the graph Laplacian.
 * The resulting layout captures clusters, symmetries, and long-range relationships deterministically.
 * @param graph The graph
 * @param info The parameters of the algorithm
 */

template <typename Backend>
PositionTable spectral_layout_2d(const graphs::Graph<Backend>& graph, const SpectralCreateInfo& info = {}) {
  using std::size_t;
  const size_t n = static_cast<size_t>(graph.getVertexCount());

  PositionTable result;
  result.positions.resize(n);

  if (n == 0)
    return result;

  Eigen::MatrixXd L = Eigen::MatrixXd::Zero(n, n);

  for (size_t u = 0; u < n; ++u) {
    const auto edges = graph.getEdges(u);
    for (uint64_t v : edges) {
      if (u == v) continue;

      L(u, v) -= 1.0;
      L(v, u) -= 1.0;
      L(u, u) += 1.0;
      L(v, v) += 1.0;
    }
  }

  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(L);
  if (solver.info() != Eigen::Success)
    return result;

  const Eigen::VectorXd& eigenvalues  = solver.eigenvalues();
  const Eigen::MatrixXd& eigenvectors = solver.eigenvectors();

  int e1 = -1;
  int e2 = -1;

  for (int i = 0; i < eigenvalues.size(); ++i) {
    if (eigenvalues[i] > 1e-8) {
      if (e1 < 0)
        e1 = i;
      else {
        e2 = i;
        break;
      }
    }
  }

  if (e1 < 0 || e2 < 0)
    return result;

  for (size_t i = 0; i < n; ++i) {
    result.positions[i] = glm::vec3(
      static_cast<float>(eigenvectors(i, e1)) * info.scale,
      static_cast<float>(eigenvectors(i, e2)) * info.scale,
      0.0f);
  }

  if (info.center) {
    glm::vec2 c(0.0f);
    for (const auto& p : result.positions)
      c += glm::vec2(p);
    c /= static_cast<float>(n);

    for (auto& p : result.positions) {
      p.x -= c.x;
      p.y -= c.y;
    }
  }

  return result;
}



/*
 * This is the spectral graph layout in 3D.
 * @param graph The graph
 * @param info The parameters of the algorithm
 */
template <typename Backend>
PositionTable spectral_layout_3d(const graphs::Graph<Backend>& graph, const SpectralCreateInfo& info = {}) {
  using std::size_t;

  const size_t  n = static_cast<size_t>(graph.getVertexCount());
  PositionTable result;
  result.positions.resize(n);

  if (n == 0)
    return result;

  Eigen::MatrixXd L = Eigen::MatrixXd::Zero(n, n);

  for (size_t u = 0; u < n; ++u) {
    const auto edges = graph.getEdges(u);
    for (uint64_t v : edges) {
      if (u == v) continue;

      L(u, v) -= 1.0;
      L(v, u) -= 1.0;
      L(u, u) += 1.0;
      L(v, v) += 1.0;
    }
  }

  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(L);
  if (solver.info() != Eigen::Success)
    return result;

  const Eigen::VectorXd& eigenvalues  = solver.eigenvalues();
  const Eigen::MatrixXd& eigenvectors = solver.eigenvectors();

  int e1 = -1;
  int e2 = -1;
  int e3 = -1;

  for (int i = 0; i < eigenvalues.size(); ++i) {
    if (eigenvalues[i] > 1e-8) {
      if (e1 < 0) e1 = i;
      else if (e2 < 0)
        e2 = i;
      else {
        e3 = i;
        break;
      }
    }
  }

  if (e1 < 0 || e2 < 0 || e3 < 0)
    return result;

  for (size_t i = 0; i < n; ++i) {
    result.positions[i] = glm::vec3(
      static_cast<float>(eigenvectors(i, e1)) * info.scale,
      static_cast<float>(eigenvectors(i, e2)) * info.scale,
      static_cast<float>(eigenvectors(i, e3)) * info.scale);
  }

  if (info.center) {
    glm::vec3 c(0.0f);
    for (const auto& p : result.positions)
      c += p;
    c /= static_cast<float>(n);

    for (auto& p : result.positions)
      p -= c;
  }

  return result;
}

} // namespace position
} // namespace graphs
