template <bool _Test, class _Ty = void> struct my_enable_if {};

template <class _Ty> struct my_enable_if<true, _Ty> {
    using type = _Ty;
};

int main(int argc, char *argv[]) {
    my_enable_if<true>::type *p = nullptr;
    return 0;
}
