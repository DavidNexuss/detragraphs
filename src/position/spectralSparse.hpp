#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/Eigenvalues>
#include <Spectra/SymEigsSolver.h>
#include <Spectra/MatOp/SparseSymMatProd.h>
#include <Spectra/SelectionRule.h>
#include "../position.hpp"
#include "../graph.hpp"


namespace graphs {
namespace position {

template <typename Backend>
PositionTable spectral_layout_2d_sparse(const graphs::Graph<Backend>& graph, const SpectralCreateInfo& info = {}) {
  using Scalar  = double;
  using SpMat   = Eigen::SparseMatrix<Scalar>;
  using Index   = typename SpMat::Index;
  using Triplet = Eigen::Triplet<Scalar>;

  const size_t n = graph.getVertexCount();
  if (n == 0) return {};

  std::vector<Triplet> triplets;
  triplets.reserve(2 * graph.getTotalEdgeCount() * 2);

  for (size_t u = 0; u < n; ++u) {
    auto   edges = graph.getEdges(u);
    size_t deg   = edges.size();

    triplets.emplace_back(u, u, static_cast<Scalar>(deg));

    for (uint64_t vv : edges) {
      Index v = static_cast<Index>(vv);
      if (u == v) continue; // safety

      triplets.emplace_back(u, v, Scalar(-1));
    }
  }

  SpMat L(n, n);
  L.setFromTriplets(triplets.begin(), triplets.end());
  L.makeCompressed();

  constexpr int k_needed = 3;
  constexpr int ncv      = 2 * k_needed + 8;

  Spectra::SparseSymMatProd<Scalar> op(L);

  Spectra::SymEigsSolver<Spectra::SparseSymMatProd<Scalar>> eigs(op, k_needed, ncv);

  eigs.init();
  int nconv = eigs.compute(Spectra::SMALLEST_ALGEBRAIC, 300, 1e-9);

  if (nconv < k_needed)
    return {};

  Eigen::VectorXd evals = eigs.eigenvalues();
  Eigen::MatrixXd evecs = eigs.eigenvectors();

  int first_nonzero = 0;
  while (first_nonzero < evals.size() && std::abs(evals(first_nonzero)) < 1e-7)
    ++first_nonzero;

  if (first_nonzero + 1 >= evals.size())
    return {};

  PositionTable result;
  result.positions.resize(n);

  for (size_t i = 0; i < n; ++i) {
    result.positions[i] = glm::vec3(
      static_cast<float>(evecs(i, first_nonzero) * info.scale),
      static_cast<float>(evecs(i, first_nonzero + 1) * info.scale),
      0.0f);
  }

  if (info.center) {
    glm::vec2 c(0.0f);
    for (const auto& p : result.positions) c += glm::vec2(p);
    c /= static_cast<float>(n);

    for (auto& p : result.positions)
      p -= glm::vec3(c, 0.0f);
  }

  return result;
}

} // namespace position
} // namespace graphs
