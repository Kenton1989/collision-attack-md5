#ifndef COLLISION_ATTACK_UTIL_HPP_
#define COLLISION_ATTACK_UTIL_HPP_

#include <cstdio>
#include <ctime>
#include <random>

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

const long long RAND_SEED = time(0);
// const long long RAND_SEED = 0;

inline std::default_random_engine& rand_engine() {
    static std::default_random_engine e(time(0));
    return e;
}

template <typename IntT = int>
inline IntT randint() {
    static std::uniform_int_distribution<IntT> dist;
    return dist(rand_engine());
}
template <>
inline Byte randint<Byte>() {
    return randint() & 0xFF;
}

template <typename IntT = int>
inline std::vector<IntT> randvec(size_t sz) {
    std::vector<IntT> result;
    result.reserve(sz);

    for (size_t i = 0; i < sz; ++i) {
        result.push_back(randint<IntT>());
    }

    return result;
}

inline Word rand_word() { return randint<Word>(); }
inline Words rand_words(size_t sz) { return randvec<Word>(sz); }
inline Byte rand_byte() { return randint<Byte>(); }
inline Bytes rand_bytes(size_t sz) { return randvec<Byte>(sz); }

#endif  // COLLISION_ATTACK_UTIL_HPP_