#pragma once
#ifndef __SHT_COMMON_TABLE_H__
#define __SHT_COMMON_TABLE_H__

#include <unordered_map>

struct EnumClassHash {
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

template <typename K, typename T>
using EnumTable = std::unordered_map<K, T, EnumClassHash>;

#endif