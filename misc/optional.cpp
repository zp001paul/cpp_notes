#include <iostream>
#include <iterator>
#include <list>
#include <optional>
#include <vector>

struct IOSlot {
    IOSlot() : slot(0) {}
    explicit IOSlot(int id) : slot{id} {
        std::cout << "slot " << slot << " constructed\n";
    }
    IOSlot(const IOSlot &s) : slot{s.slot} {
        std::cout << "slot " << slot << " copied\n";
    }
    int slot;
};

// 这样做会导致IOSlot被拷贝
std::optional<IOSlot> findIOSlot(std::vector<IOSlot> &v) {
    for (auto &s : v) {
        if (s.slot == 8) {
            std::cout << "return slot addr: " << static_cast<void *>(&(s.slot))
                      << '\n';
            return s;
        }
    }
    return std::nullopt;
}

auto findIOSlot2(std::vector<IOSlot> &v) {
    for (auto i = std::begin(v); i != std::end(v); ++i) {
        if (i->slot == 8) {
            std::cout << "return slot addr: " << static_cast<void *>(&(i->slot))
                      << '\n';
            return i;
        }
    }
    return std::end(v);
}

int main(int argc, char *argv[]) {
    std::vector<IOSlot> v;
    v.reserve(10);
    for (int i = 0; i < 10; i++) {
        v.emplace_back(i);
    }

    std::cout << "begin to find\n";
    auto opt = findIOSlot2(v);
    if (opt != std::end(v)) {
        std::cout << "we got slot: " << opt->slot << '\n';
        std::cout << "we got slot addr: " << static_cast<void *>(&(opt->slot))
                  << '\n';
    }

    return 0;
}
