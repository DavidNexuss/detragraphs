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

  //This was an attempt of using spectra for speeding up the computation on the spectral algorithm for sparse networks, which I have failed to implement correctly and I have
  //removed it from the code, I would have liked to actually implement it to test the graph plotting mechanism with high degree counts for the WS generator. I have considered the option still.
}

} // namespace position
} // namespace graphs
