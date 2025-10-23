#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <math/flatmatrix.hpp>
#include <math/csrmatrix.hpp>
#include <math/multiply.hpp>
#include <util/bench.hpp>
#include <math/sparseinversedirectory.hpp>
#include <math/sparsevector.hpp>

using namespace detra::math;
using namespace detra::util;

FlatMatrix<float> generateDenseMatrix(int N, int M, float sparsity = 0.1f) {
    FlatMatrix<float> matrix(N, M);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist_val(0.1f, 10.0f);
    std::uniform_real_distribution<float> dist_zero(0.0f, 1.0f);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (dist_zero(rng) < sparsity) {
                matrix.get(i, j) = dist_val(rng);
            }
        }
    }
    return matrix;
}

bool compareResults(const std::vector<float> &a, const std::vector<float> &b,
                    float tol = 1e-5f) {
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (std::fabs(a[i] - b[i]) > tol)
            return false;
    return true;
}

int main() {
    int N = 1000; 
    int M = 1000;
    int R = 20;
    float sparsityA = 0.01f;

    std::cout << "Generating sparse matrix (" << N << "x" << M << ") with sparsity " << (sparsityA * 100) << "%...\n";

    FlatMatrix<float> A_dense = generateDenseMatrix(N, M, sparsityA);

    auto t_csr_build_0 = std::chrono::high_resolution_clock::now();
    CSRMatrix A(A_dense);
    auto t_csr_build_1 = std::chrono::high_resolution_clock::now();
    double csr_build_ms = std::chrono::duration<double, std::milli>(t_csr_build_1 - t_csr_build_0).count();


    std::vector<float> x_dense(M, 0.0f);
    {
        std::mt19937 rng(123);
        std::uniform_int_distribution<int> dist_idx(0, M - 1);
        for (int i = 0; i < R; i++) {
            x_dense[dist_idx(rng)] = 1.0f; 
        }
    }

    SparseVector x_sparse(x_dense);


    std::cout << "Building inverse directory...\n";
    auto t_inv_build_0 = std::chrono::high_resolution_clock::now();
    SparseInverseDirectory inv(A); 
    auto t_inv_build_1 = std::chrono::high_resolution_clock::now();
    double inv_build_ms = std::chrono::duration<double, std::milli>(t_inv_build_1 - t_inv_build_0).count();


    std::vector<float> y_csr, y_inv_sparse, y_dense_naive;

    std::cout << "\nBenchmarking...\n";

    int RUNS = 100;
    double t_csr = benchmark([&]() { A.apply_inplace(x_dense, y_csr); }, RUNS);
    double t_inv_sparse = benchmark( [&]() { inv.apply_inplace(x_sparse, y_inv_sparse); }, RUNS);
    double t_dense_naive = benchmark([&]() { A_dense.apply_inplace(x_dense, y_dense_naive); }, RUNS);


    bool ok_inv = compareResults(y_csr, y_inv_sparse);
    bool ok_dense_naive = compareResults(y_csr, y_dense_naive);

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "\n--- RESULTS ---\n";
    std::cout << "CSR vs InverseSparse: " << (ok_inv ? "PASSED ✅" : "FAILED ❌") << "\n";
    std::cout << "CSR vs Naive Dense:   " << (ok_dense_naive ? "PASSED ✅" : "FAILED ❌") << "\n";

    std::cout << "\n--- PERFORMANCE (average over runs) ---\n";
    std::cout << "CSR Build Time:                " << csr_build_ms << " ms\n";
    std::cout << "Inverse Directory Build Time:  " << inv_build_ms << " ms\n";
    std::cout << "------------------------------------------\n";
    std::cout << "1. CSR Multiply (Sparse-Dense): " << t_csr << " ms\n";
    std::cout << "2. Inverse Sparse Multiply:     " << t_inv_sparse << " ms\n";
    std::cout << "3. Naive Dense Multiply:        " << t_dense_naive << " ms\n";
    
    std::cout << "\nSpeedup (Naive Dense / CSR): " << (t_dense_naive / t_csr) << "x\n";

    return 0;
}
