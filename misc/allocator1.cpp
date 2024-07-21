#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char *argv[]) {
    int ia[]{11, 12, 13, 14, 15, 16};
    std::vector<int> vi(ia, ia + 6);

    std::cout << vi.back() << std::endl;
    return 0;
}
