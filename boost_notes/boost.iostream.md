# 编译 1

./bootstrap.sh --prefix=/home/zp001/App/boost185
./b2 install

这种编译后有些头文件找不到，比如：

```txt
❯ make
g++ -std=c++20 -I/home/zp001/App/boost185/include std_boost.cpp
std_boost.cpp:3:10: fatal error: libs/iostreams/example/container_device.hpp: No such file or directory
    3 | #include <libs/iostreams/example/container_device.hpp>
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make: *** [Makefile:2: std_boost] Error 1
```

# 编译2

直接： yum -y install boost-iostreams
