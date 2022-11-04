#ifndef COLLISION_ATTACK_UTIL_HPP_
#define COLLISION_ATTACK_UTIL_HPP_

#include <cstdio>

#include "common.hpp"

inline void prt_h(Byte b) {
    printf("%02x", b);
}
inline void prt_h(const Bytes& bytes) {
    for (Byte b : bytes) {
        prt_h(b);
    }
}

inline void prt_h(Word w) {
    printf("0x%08x", w);
}
inline void prt_h(const Words& words) {
    for (Word w : words) {
        prt_h(w);
        printf(" ");
    }
}

template <typename T>
inline void prtln_h(const T& v) {
    prt_h(v);
    printf("\n");
}

#endif  // COLLISION_ATTACK_UTIL_HPP_