#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

struct IOSlot {
    // IOSlot() : slot(0) {}
    explicit IOSlot(int id) : slot{id} {
        std::cout << "slot " << slot << " constructed\n";
    }
    IOSlot(const IOSlot &s) : slot{s.slot} {
        std::cout << "slot " << slot << " copied\n";
    }
    int slot;
    bool operator==(const IOSlot &s);
};

bool IOSlot::operator==(const IOSlot &s) { return slot == s.slot; }

auto findIOSlot(std::vector<IOSlot> &v) {
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
    auto opt = findIOSlot(v);
    if (opt != std::end(v)) {
        std::cout << "we got slot: " << opt->slot << '\n';
        std::cout << "we got slot addr: " << static_cast<void *>(&(opt->slot))
                  << '\n';
    }

    std::cout << "begin to find2 \n";
    auto it = std::find_if(std::begin(v), std::end(v),
                           [](const IOSlot &s) { return s.slot == 8; });
    if (it != std::end(v)) {
        std::cout << "we got slot: " << it->slot << '\n';
        std::cout << "we got slot addr: " << static_cast<void *>(&(it->slot))
                  << '\n';
    }

    return 0;
}
