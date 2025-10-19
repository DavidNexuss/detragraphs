#include <vector>

template <typename T>
struct is_vector : std::false_type {};

template <typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {};

template <typename Container, typename F>
auto mapf(const Container& source, F&& function) {
  if constexpr (is_vector<Container>::value) {
    using InnerType = typename Container::value_type;
    std::vector<decltype(mapf(std::declval<InnerType>(), std::forward<F>(function)))> result;
    result.reserve(source.size());
    for (auto&& elem : source)
      result.push_back(mapf(elem, std::forward<F>(function)));
    return result;
  } else {
    return function(source);
  }
}

template <typename F>
auto mapexec(F&& function, size_t start, size_t end) {
  std::vector<decltype(std::forward<F>(function))> values;
  for (size_t i = start; i < end; i++) { values[i] = function(i); }
  return values;
}
