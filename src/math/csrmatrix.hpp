#pragma once
#include <vector>
#include "flatmatrix.hpp"
#include <cmath>
#include <algorithm>

namespace detra {
namespace math {

struct CSRMatrix {
    int N, M;
    std::vector<int> row_ptr;
    std::vector<int> col_idx;
    std::vector<float> val;

    using Vector = std::vector<float>;

    CSRMatrix() : N(0), M(0) { }

    CSRMatrix(const FlatMatrix<float>& matrix, float tolerance = 1e-6) : 
        N(matrix.getRowCount()), M(matrix.getColumnCount()) 
    {
        row_ptr.push_back(0); 
        int nnz_count = 0;
        
        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < M; ++j) {
                float value = matrix.get(i, j);
                if(std::abs(value) > tolerance) { 
                    val.push_back(value);
                    col_idx.push_back(j);
                    nnz_count++;
                }
            }
            row_ptr.push_back(nnz_count); 
        }
    }

    inline void apply_inplace(const std::vector<float>& x, std::vector<float>& y) {
        if (x.size() != M || y.size() != N) {
             y.resize(N, 0.0f);
        } else {
             std::fill(y.begin(), y.end(), 0.0f);
        }

        for (int i = 0; i < N; ++i) {
            for (int k = row_ptr[i]; k < row_ptr[i + 1]; ++k) {
                y[i] += val[k] * x[col_idx[k]];
            }
        }
    }
};
} // namespace math
} // namespace detra
