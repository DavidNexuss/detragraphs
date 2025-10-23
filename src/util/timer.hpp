#pragma once
#include <chrono>
#include <iostream>

namespace detra {
namespace util {

struct Timer {
  std::chrono::time_point<std::chrono::high_resolution_clock> t0;
  std::string                                                 regioname;

  Timer(const std::string& regioname) :
    regioname(regioname) {
    t0 = std::chrono::high_resolution_clock::now();
  }

  ~Timer() {
    auto t1 = std::chrono::high_resolution_clock::now();
    std::cout << "Done " << regioname << " =[ " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "ms ]=" << std::endl;
  }
};
} // namespace util
} // namespace detra

#define DETRA_TIMER(X) detra::util::Timer X(#X);
