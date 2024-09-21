#include <boost/format.hpp>
#include <iostream>
#include <thread>

class A {
  public:
    int m_value;

  public:
    A(int a) : m_value(a) {
        std::cout << boost::format("A ctor(%1%), thread_id: ") % a
                  << std::this_thread::get_id() << '\n';
    }
    A(const A &a) : m_value(a.m_value) {
        std::cout << boost::format("A copy ctor(%1%), thread_id: ") % a.m_value
                  << std::this_thread::get_id() << '\n';
    }
    ~A() {
        std::cout << boost::format("A dtor(%1%), thread_id: ") % m_value
                  << std::this_thread::get_id() << '\n';
    }
};

void myprt(const A obj) {
    std::cout << boost::format("myprint(): %1%\n") % obj.m_value;
}

int main(int argc, char *argv[]) {
    std::cout << boost::format("main() thread_id: %1%\n") %
                     std::this_thread::get_id();

    int val{8};
    std::thread t{myprt, val};
    t.join();

    return 0;
}
