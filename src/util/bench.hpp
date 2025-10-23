#include <chrono>
namespace detra {
namespace util {

template <typename Func>
double benchmark(Func func, int runs = 20) {
  using namespace std::chrono;
  double total_ms = 0;
  for (int i = 0; i < runs; ++i) {
    auto t0 = high_resolution_clock::now();
    func();
    auto t1 = high_resolution_clock::now();
    total_ms += duration<double, std::milli>(t1 - t0).count();
  }
  return total_ms / runs;
}
} // namespace util
} // namespace detra
