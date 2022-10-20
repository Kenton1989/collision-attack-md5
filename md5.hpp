#ifndef COLLISION_ATTACK_MD5
#define COLLISION_ATTACK_MD5

#include <stddef.h>

#include <string>
#include <vector>

namespace Md5 {

// common data type
using Byte = uint8_t;
using Word = uint32_t;
using Bytes = std::vector<Byte>;
using Words = std::vector<Word>;

/** Encoding methods
 * bytes to words:
 *      every 4 bytes becomes one word.
 *      the bytes with lower address(index) is put in the MSB of word.
 * words to bytes:
 *      every word becomes 4 bytes.
 *      the bytes with lower address(index) comes from the MSB of word.
 */
// encode bytes into words, requires length of input mod 4 == 0
Words encode_words(const std::string& s);
Words encode_words(const Bytes& s);
Words encode_words(const char* s);

Bytes encode_bytes(const std::string& s);
Bytes encode_bytes(const Words& s);
Bytes encode_bytes(const char* s);

std::string encode_string(const Words& s);
std::string encode_string(const Bytes& s);

// rotate operations
inline Word l_rotate(Word x, int shift) {
    return x << shift | (x >> (sizeof(x) * 8 - shift));
}
inline Word r_rotate(Word x, int shift) {
    return l_rotate(x, (sizeof(x) * 8 - shift));
}

class Md5BlockHasher {
   public:
    Md5BlockHasher(const Words& iv, const Words& msg);

    // change state
    void set_iv(const Words& iv);
    void set_msg(const Words& msg);
    void set_msg_word(int index, Word word);

    // get state
    const Words& get_iv() const;
    const Words& get_msg() const;

    // perform calculation for one step or a range of steps
    // all end points of range are inclusive
    void cal_step(int step);
    void cal_range(int from, int to);
    void cal_all();
    void cal_to_end(int from);
    void cal_from_begin(int until);

    // return the output of the specified step
    Word output_of(int step) const;

    // return the 4-word final output
    // make sure all 64 steps are performed before calling this method
    Words final_output() const;

   private:
    Words iv;
    Words outputs;
    Words message;
};

// constains utility function to calculate steps
class StepOf {
    // a1 = the 1st output of a, which is in step 1
    int a(int i) { return i * 4 - 3; }
    int b(int i) { return i * 4; }
    int c(int i) { return i * 4 - 1; }
    int d(int i) { return i * 4 - 2; }
};

const StepOf stepOf;

// the shifting of each step of MD5
const Word S[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                    5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
                    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

// the constants to add in each step of MD5
const Word K[64] = {
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

}  // namespace Md5

#endif  // COLLISION_ATTACK_MD5