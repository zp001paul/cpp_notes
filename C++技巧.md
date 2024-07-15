# offsetof宏

https://en.cppreference.com/w/cpp/types/offsetof

> If `type` is not a [*PODType*](https://en.cppreference.com/w/cpp/named_req/PODType)(until C++11)[standard-layout](https://en.cppreference.com/w/cpp/language/data_members#Standard-layout) type(since C++11), the result of `offsetof` is undefined(until C++17)use of the `offsetof` macro is conditionally-supported(since C++17).

妈的我就知道是这样。

那什么是standard-layout ?

https://en.cppreference.com/w/cpp/language/data_members#Standard-layout

