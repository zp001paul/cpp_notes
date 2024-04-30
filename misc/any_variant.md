# std::any的使用

## 好文章

https://blog.csdn.net/qq_21438461/article/details/131829430 # 讲出了设计目的

## 应用场景

它的主要应用场景是在不知道或不关心具体类型的情况下存储和处理数据。例如，我们可以使用`std::any`来实现一个可以存储任何类型数据的数组，或者实现一个可以接受任何类型参数的函数。

它和variant的区别在于，它避免了使用模板，所以要自己实现visit，通常就是要使用typeid()

### 例子1

这个例子使用了typeid()，虽然不优雅，但是C++本身就是现实主义的

```c++
#include <any>
#include <vector>
#include <string>

int main() {
    std::vector<std::any> v;
    v.push_back(1); // int
    v.push_back(std::string("Hello world")); // std::string
    v.push_back(3.14); // double

    for (const auto& a : v) {
        if (a.type() == typeid(int)) {
            // process int
        } else if (a.type() == typeid(std::string)) {
            // process std::string
        } else if (a.type() == typeid(double)) {
            // process double
        }
    }
}

```

# std::variant的使用

这个可能更适合日常编程使用
