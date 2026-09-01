#ifndef ENUMUTILS_H
#define ENUMUTILS_H

#include <array>
#include <cstddef>
#include <functional>
#include <string>

template <typename T>
struct EnumStrings;

namespace EnumUtils
{
    template <typename T>
    void forEachEnum(std::function<void(T)> action)
    {
        for (int i = 0; i < static_cast<int>(T::COUNT); ++i)
        {
            action(static_cast<T>(i));
        }
    }

    template <typename T>
    constexpr const char *enumToString(T value)
    {
        return EnumStrings<T>::names.at(static_cast<size_t>(value));
    }

    template <typename T>
    T stringToEnum(const std::string &str)
    {
        return static_cast<T>(std::stoi(str));
    }

};

#endif // ENUMUTILS_H
