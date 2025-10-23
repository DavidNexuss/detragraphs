#pragma once
#include <vector>
#include <iostream>
#include <type_traits>
#include <utility>

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
  using ReturnType = decltype(function(start));
  std::vector<ReturnType> values;
  values.reserve(end - start);
  for (size_t i = start; i < end; ++i)
    values.push_back(function(i));
  return values;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
  os << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    if constexpr (is_vector<T>::value)
      os << v[i]; // recurse automatically
    else
      os << v[i];
    if (i + 1 < v.size())
      os << ", ";
  }
  os << "]";
  return os;
}
