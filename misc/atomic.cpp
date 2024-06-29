#include "xsbase.h"
int main(int argc, char *argv[]) {
    // std::atomic_int i;
    unsigned i;
    atomic_store_release(&i, 10);
    return 0;
}
