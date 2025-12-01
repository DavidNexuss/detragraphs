#include <stdlib.h>
#include <graph.hpp>
#include <math/multiply.hpp>
#include <math/flatmatrix.hpp>
#include <detrarandom/random_sources.hpp>
#include <util/vector_utils.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

// stb image write
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace detra;
using namespace detra::math;

// Convert scalar to RGB heatmap (blue→cyan→green→yellow→red)
inline void scalar_to_heatmap(float v, unsigned char& r, unsigned char& g, unsigned char& b) {
  v         = std::clamp(v, 0.0f, 1.0f);
  float r_f = 0.0f, g_f = 0.0f, b_f = 0.0f;

  if (v <= 0.25f) { // blue → cyan
    float t = v / 0.25f;
    r_f     = 0.0f;
    g_f     = t;
    b_f     = 1.0f;
  } else if (v <= 0.5f) { // cyan → green
    float t = (v - 0.25f) / 0.25f;
    r_f     = 0.0f;
    g_f     = 1.0f;
    b_f     = 1.0f - t;
  } else if (v <= 0.75f) { // green → yellow
    float t = (v - 0.5f) / 0.25f;
    r_f     = t;
    g_f     = 1.0f;
    b_f     = 0.0f;
  } else { // yellow → red
    float t = (v - 0.75f) / 0.25f;
    r_f     = 1.0f;
    g_f     = 1.0f - t;
    b_f     = 0.0f;
  }

  r = static_cast<unsigned char>(r_f * 255);
  g = static_cast<unsigned char>(g_f * 255);
  b = static_cast<unsigned char>(b_f * 255);
}

// Save normalized float grid to PNG heatmap
void save_png_heatmap(const std::vector<float>& data, int width, int height, const std::string& filename) {
  // normalize values
  float minv  = *std::min_element(data.begin(), data.end());
  float maxv  = *std::max_element(data.begin(), data.end());
  float scale = (maxv - minv > 1e-8f) ? 1.0f / (maxv - minv) : 1.0f;

  std::vector<unsigned char> rgb(width * height * 3);

  for (int i = 0; i < width * height; i++) {
    float         v = (data[i] - minv) * scale;
    unsigned char r, g, b;
    scalar_to_heatmap(v, r, g, b);
    rgb[3 * i + 0] = r;
    rgb[3 * i + 1] = g;
    rgb[3 * i + 2] = b;
  }

  stbi_write_png(filename.c_str(), width, height, 3, rgb.data(), width * 3);
  std::cerr << "Wrote " << filename << " (range: " << minv << " - " << maxv << ")\n";
}

void save_hdr(const std::vector<float>& data, int width, int height, const std::string& filename) {
  stbi_write_hdr(filename.c_str(), width, height, 1, data.data());
  std::cerr << "Wrote " << filename << std::endl;
}

template <typename RandomSource = random_sources::XORand>
FlatMatrix<float> randomMatrix(int N, RandomSource rand = {}) {
  FlatMatrix<float> A(N, N);

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (i == j) continue;
      if (rand.randf() > 0.7f)
        A[i][j] = rand.randi() % 10;
    }
  }

  A.row_normalize();
  return A;
}

void random_jacobi() {
  int N = 10;

  auto matrix = randomMatrix(N);
  matrix.print();
  matrix.transpose();
  std::cout << math::jacobi(matrix) << std::endl;
}

struct Heat2D {
  int N;
  int M;
  int n;
  int m;

  using Vector = std::vector<float>;

  Heat2D(int _N, int _M) :
    N(_N * _M), M(_N * _M), m(_M), n(_N) {}

  void apply_inplace(const std::vector<float>& input, std::vector<float>& output) const {
    float alpha = 0.1f;

#pragma omp parallel for
    for (int i = 1; i < n - 1; i++)
      for (int j = 1; j < m - 1; j++) {
        int   idx       = i * m + j;
        float laplacian = (input[idx + 1] +
                           input[idx - 1] +
                           input[idx + m] +
                           input[idx - m] -
                           4.0f * input[idx]);

        output[idx] = input[idx] + alpha * laplacian;
      }
  }
};

struct Heat2D_RB {
  int   N, M;
  int   n, m;
  float alpha;

  using Vector = std::vector<float>;

  Heat2D_RB(int _n, int _m, float _alpha = 0.1f) :
    N(_n * _n), M(_m * _m), n(_n), m(_m), alpha(_alpha) {}

  void apply_inplace(Vector& u) const {
    //Red update
#pragma omp parallel for
    for (int i = 1; i < n - 1; i++) {
      for (int j = 1 + (i % 2); j < m - 1; j += 2) {
        int idx = i * m + j;
        u[idx]  = u[idx] + alpha * (u[idx + 1] + u[idx - 1] + u[idx + m] + u[idx - m] - 4.0f * u[idx]);
      }
    }

    //Black update
#pragma omp parallel for
    for (int i = 1; i < n - 1; i++) {
      for (int j = 1 + ((i + 1) % 2); j < m - 1; j += 2) {
        int idx = i * m + j;
        u[idx]  = u[idx] + alpha * (u[idx + 1] + u[idx - 1] + u[idx + m] + u[idx - m] - 4.0f * u[idx]);
      }
    }
  }
};

void heat_jacobi_influx() {
  int    N = 512;
  Heat2D heatfunction(N, N);

  std::vector<float> x(N * N);
  x[N * N / 2 + N / 2] = 100.0f;

  save_png_heatmap(math::jacobi(heatfunction, 1e-8, 6000, x, x), N, N, "heat_influx.png");
}

void heat_jacobi() {
  int    N = 512;
  Heat2D heatfunction(N, N);

  std::vector<float> x(N * N);
  x[N * N / 2 + N / 2] = 100.0f;

  save_png_heatmap(math::jacobi(heatfunction, 1e-8, 6000, x), N, N, "heat.png");
}

void heat_gauss() {
  int       N = 512;
  Heat2D_RB heatfunction(N, N);

  std::vector<float> x(N * N);
  x[N * N / 2 + N / 2] = 100.0f;

  save_png_heatmap(math::gauss(heatfunction, 1e-8, 6000, x), N, N, "heat_gauss.png");
}

int main() {
  heat_jacobi_influx();
  heat_jacobi();
  heat_gauss();
}
