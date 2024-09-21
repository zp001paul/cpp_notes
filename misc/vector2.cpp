#include <boost/assign.hpp>
#include <iostream>
#include <vector>
int main(int argc, char *argv[]) {
    using namespace boost::assign;
    std::vector<int> v;
    v += 1, 2, 3, 4, 5;

    v.erase(v.begin(), v.begin() + 3);
    std::ostream_iterator<int> it(std::cout, " ");
    std::copy(v.begin(), v.end(), it);
    std::cout << std::endl;
    return 0;
}
