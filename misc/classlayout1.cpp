#include <iostream>

class A {
    virtual void fn();
    int m_int;
};
class B {
    void fn();
    int m_int;
};
class C {
    int m_int;
};

int main(int argc, char *argv[]) {
    std::cout << sizeof(A) << std::endl;
    std::cout << sizeof(B) << std::endl;
    std::cout << sizeof(C) << std::endl;
    return 0;
}
