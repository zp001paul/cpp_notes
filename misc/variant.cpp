#include <iostream>
#include <variant>
struct X {
    X() : v_(10) {}
    int v_;
};

struct Y {
    Y(double v) : v_(v) {}
    double v_;
};

int main(int argc, char *argv[]) {

    std::variant<X, Y> a1;
    return 0;
}
