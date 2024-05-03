#pragma once
#include <map>
#include <type_traits>
#include <unordered_map>
#include <vector>

template <typename T>
concept VectorType =
    // value_type也是T，在<vector>中定义，T和value_type相等，说明T被实例化成vector<T>
    std::is_same_v<std::remove_cvref_t<T>,
                   std::vector<typename std::remove_cvref_t<T>::value_type>>;
;

template <typename T>
concept MapType =
    std::is_same_v<T, std::map<typename T::key_type, typename T::mapped_type>>;

template <typename T>
concept UnorderMapType = std::is_same_v<
    T, std::unordered_map<typename T::key_type, typename T::mapped_type>>;

template <typename T>
concept AllMapType = MapType<T> || UnorderMapType<T>;
