#include <utility> // for std::move()

template <typename T> void unordered_vec_del(T &v, typename T::interface it) {
  if (it < v.end()) {
    *it = std::move(v.back());
    v.pop_back();
  }
}
