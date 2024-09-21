int main(int argc, char *argv[]) {
    int x{9};
    int y{8};
    const int *p1{&x};
    int const *p2{&x};
    *p1 = 8;
    *p2 = 8;
    p1 = &y;
    return 0;
}
