#include <cstdint>
#include <array>
#include <unordered_set>
#include <iostream>

constexpr size_t PERM_SIZE = 16;

constexpr std::array<uint64_t, PERM_SIZE> perm = {
  0x1ull, 0xCull, 0x3ull, 0x7ull,
  0x5ull, 0xBull, 0x6ull, 0xDull,
  0x2ull, 0x9ull, 0xEull, 0xFull,
  0x4ull, 0xAull, 0x8ull, 0x0ull};

uint64_t hash(uint64_t x, uint64_t N) {
  // scramble low bits using a tiny permutation table
  uint64_t lo = perm[x % PERM_SIZE];

  // multiply and mix
  constexpr uint64_t A = 0x6b43f8a9c1e34b27ull;
  constexpr uint64_t B = 0x95a7d3e0f1b2c648ull;

  uint64_t h = ((x * A + B) ^ lo ^ (x >> 11));
  return h % N;
}

uint64_t test_no_collisions(uint64_t N) {
  std::unordered_set<uint64_t> seen;

  uint64_t collisions = 0;
  for (uint64_t i = 0; i < N; ++i) {
    uint64_t h = hash(i, N);
    if (!seen.insert(h).second) {
      collisions++;
    }
  }
  return collisions;
}

int main() {
  for (uint64_t N : {10, 100, 1000, 10000, 100000}) {
    std::cout << "Testing N = " << N << "...\n";
    std::cout << "Collisions: " << test_no_collisions(N) << std::endl;
  }
  return 0;
}
