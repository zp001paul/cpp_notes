#pragma once
#include "myconcept.hpp"
#include <algorithm>
#include <utility> // for std::move()

template <VectorType T>
void unordered_vec_remove(T &v, typename T::interface it) {
  if (it < v.end()) {
    *it = std::move(v.back());
    v.pop_back();
  }
}

template <VectorType Tvec, typename Tvalue>
void insert_sorted(Tvec &vec, const Tvalue &v) {
  const auto pos{std::ranges::lower_bound(vec, v)};
  vec.insert(pos, v);
}
