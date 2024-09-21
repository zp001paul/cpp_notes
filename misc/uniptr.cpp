#include <memory>
struct X {
    int m;
    std::string_view str_ref;
    explicit X(int i, std::string_view sv) : m{i}, str_ref(sv) {}
};

int main(int argc, char *argv[]) {
    auto p1 = std::make_unique<X>(10, "hello");
    auto p2 = std::make_unique<X>(*p1);
    return 0;
}
