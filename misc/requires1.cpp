#define FMT_HEADER_ONLY
#include <concepts>
#include <fmt/core.h>
#include <type_traits>

template <typename T>
concept Averageable = std::is_destructible_v<T> && requires(T a, T b) {
  { a + b } -> std::same_as<T>;
  { a / size_t{1} } -> std::same_as<T>;
  // 这里不能直接写T，因为这里要写一个概念
  // 其实这里不用管返回值，返回值应该在mean()函数中定义，概念是“一个”T类型的约束
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
