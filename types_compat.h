#ifndef TYPES_COMPAT_H
#define TYPES_COMPAT_H

#include <cstdint>
#include <vector>
#include <algorithm>

typedef unsigned char  byte;
typedef unsigned int   uint;
typedef unsigned short ushort;

template<typename T>
inline void VectorErase(std::vector<T>& vec, T val) {
    auto it = std::find(vec.begin(), vec.end(), val);
    if (it != vec.end()) {
        vec.erase(it);
    }
}

#endif // TYPES_COMPAT_H
