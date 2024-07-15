
/* this example is not complated
 * */
#include <vector>
class Arena {
  public:
    void *p;
    int s;

  public:
    Arena(void *pp, int ss);
};

template <typename T> struct MyAlloc {
    using value_type = T;
    Arena &a;
    MyAlloc(Arena &aa) : a(aa) {}
    T *allocate(size_t n) { return reinterpret_cast<T *>(a.p); }
};

int main(int argc, char *argv[]) {
    constexpr int sz{1000};
    Arena myArena{new char[sz], sz};
    std::vector<int, MyAlloc<int>> v1{MyAlloc<int>(myArena)};
    return 0;
}
