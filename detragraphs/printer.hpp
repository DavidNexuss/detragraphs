#include <vector>
#include <iostream>

namespace printer {

template <typename T>
void vector(const std::vector<T>& elements) {
  for (size_t i = 0; i < elements.size(); i++) {
    std::cout << i << ": " << elements[i] << std::endl;
  }
  std::cout << std::endl;
}

template <typename T>
void vectorlist(const std::vector<std::vector<T>>& elements) {
  for (size_t i = 0; i < elements.size(); i++) {
    std::cout << i << ": ";
    for (size_t j = 0; j < elements[i].size(); j++) {
      std::cout << elements[i][j] << " ";
    }
    std::cout << std::endl;
  }
}
} // namespace printer
