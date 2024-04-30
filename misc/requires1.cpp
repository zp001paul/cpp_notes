#define FMT_HEADER_ONLY
#include <concepts>
#include <fmt/core.h>
#include <type_traits>

template <typename T>
concept Averageable = std::is_destructible_v<T> && requires(T a, T b) {
  { a + b } -> std::convertible_to<T>; // 这里不能直接写T，真奇怪
  { a / size_t{1} } -> std::convertible_to<T>;
};

template <Averageable T> T mean(const T *arr, std::size_t length) {
  T sum{0};
  for (size_t i{}; i < length; i++) {
    sum += arr[i];
  }
  return sum / length;
}

int main() {
  double arr[]{0, 1, 2, 3, 4, 5, 6.0};
  fmt::print("average: {}\n", mean(arr, sizeof(arr) / sizeof(double)));
  return 0;
}
