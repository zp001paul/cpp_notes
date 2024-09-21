#pragma once
#include <list>
#include <map>
#include <type_traits>
#include <unordered_map>
#include <vector>

template <typename T>
concept VectorType =
    // value_type是vector<X>中填入的X，在<vector>中定义，T和vector<X>相等，说明T就是一个vector，下同
    std::is_same_v<std::remove_cvref_t<T>,
                   std::vector<typename std::remove_cvref_t<T>::value_type>>;
;

template <typename T>
concept ListType =
    std::is_same_v<std::remove_cvref_t<T>,
                   std::list<typename std::remove_cvref_t<T>::value_type>>;

template <typename T>
// 其实完整的写法应该是是使用remove_cvref_t，这里为了自己能一眼看明白，省了
concept MapType =
    std::is_same_v<T, std::map<typename T::key_type, typename T::mapped_type>>;

template <typename T>
concept UnorderMapType = std::is_same_v<
    T, std::unordered_map<typename T::key_type, typename T::mapped_type>>;

template <typename T>
concept AllMapType = MapType<T> || UnorderMapType<T>;
