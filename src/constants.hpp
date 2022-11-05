#ifndef COLLISION_ATTACK_CONSTANTS_HPP_
#define COLLISION_ATTACK_CONSTANTS_HPP_

#include <stddef.h>

#include <string>
#include <vector>

#include "common.hpp"

namespace Md5 {

// define POV of md5, which is proven to be 24
const int POV = 24;

// define 0-index steps
const int FIRST_STEP = 0;
const int LAST_STEP = FIRST_STEP + 63;

// define size
const size_t BLOCK_BITS = 512;
const size_t BLOCK_BYTES = BLOCK_BITS / 8;
const size_t BLOCK_WORDS = BLOCK_BYTES / 4;

// utility function to calculate steps
class StepOf {
   public:
    // a1 = the 1st output of a, which is in step 0
    int a(int i) const { return i * 4 - 4; }
    int b(int i) const { return i * 4 - 1; }
    int c(int i) const { return i * 4 - 2; }
    int d(int i) const { return i * 4 - 3; }
};

const StepOf step_of;

// the shifting of each step of MD5
const Word S[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                  5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                  4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                  6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

// the constants to add in each step of MD5
const Word K[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
    0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
    0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
    0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
    0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
    0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

// the initial state/initial vector of MD5
const Words IV = {
    0x67452301,  // A
    0xefcdab89,  // B
    0x98badcfe,  // C
    0x10325476,  // D
};

// non linear function used in MD5
inline Word F(Word b, Word c, Word d) { return (b & c) | (d & ~b); }
inline Word G(Word b, Word c, Word d) { return (b & d) | (c & ~d); }
inline Word H(Word b, Word c, Word d) { return b ^ c ^ d; }
inline Word I(Word b, Word c, Word d) { return c ^ (b | ~d); }

inline Word step_to_func_result(const int step, const Word b, const Word c, const Word d) {
    if (0 <= step && step < 16) {
        return F(b, c, d);
    }
    if (16 <= step && step < 32) {
        return G(b, c, d);
    }
    if (32 <= step && step < 48) {
        return H(b, c, d);
    }
    if (48 <= step && step < 64) {
        return I(b, c, d);
    }
    throw;
}

}  // namespace Md5

#endif  // COLLISION_ATTACK_CONSTANTS_HPP_
