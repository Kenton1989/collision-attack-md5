#ifndef COLLISION_ATTACK_MD5_HPP_
#define COLLISION_ATTACK_MD5_HPP_

#include <stddef.h>

#include <string>
#include <vector>

namespace Md5 {

// common data type
using Byte = uint8_t;
using Word = uint32_t;
using Bytes = std::vector<Byte>;
using Words = std::vector<Word>;

// decode bytes into words.
// if length of input mod 4 != 0, the remainder are discarded.
Words encode_words(const Bytes &s);
// encode words into bytes.
Bytes encode_bytes(const Words &words);

Bytes encode_bytes(const std::string &s);
std::string encode_string(const Bytes &bytes);

// rotate operations
inline Word l_rotate(Word x, int shift) {
    return x << shift | (x >> (sizeof(x) * 8 - shift));
}
inline Word r_rotate(Word x, int shift) {
    return l_rotate(x, (sizeof(x) * 8 - shift));
}

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

// padding the input bytes
Bytes padding(const Bytes &bytes);

// compute which word of message should be used as input for the given step
int msg_idx_of_step(int step);

// compute ((a + K + msg + f(b,c,d)) <<< S) + b, f is one of F/G/H/I
// step is 0-index, first step is 0
Word perform_one_step(int step, Word msg, Word a, Word b, Word c, Word d);

// similar to another perform_one_step, but determine which word of message is used automatically
Word perform_one_step(int step, const Words &msg, Word a, Word b, Word c, Word d);

class Md5BlockHasher {
   public:
    Md5BlockHasher(const Words &iv, const Words &msg);

    const Words &get_iv() const;
    void set_iv(const Words &iv);

    const Words &get_msg() const;
    void set_msg(const Words &msg);
    void set_msg_word(int index, Word word);

    // perform calculation for a range of steps
    // both end points of range are inclusive
    // step is 0 index
    void cal_range(int from, int to);
    // calculate all steps and return final 4-word output.
    Words cal_all();
    // calculate steps from parameter "from" to the end and return final 4-word output.
    Words cal_to_end(int from);
    // calculate steps from begin to parameter "until", return the 1-word result of last calculated step.
    Word cal_from_begin(int until);
    // calculate one step and return the 1-word output of the step
    Word cal_step(int step);

    // return the output of the specified step
    Word output_of(int step) const;
    void set_output_of(int step, Word val);

    // return the main input of the specified step
    // e.g.
    // main input of output a2 is a1
    // main input of output b7 is b6
    // main input of output c1 is C
    Word main_input_of(int step) const;
    void set_main_input_of(int step, Word val);

    // get full 4-word input of the step
    Words full_input(int step) const;
    void set_full_input(int step, const Words &val);

    // get full 4-word output of the step
    Words full_output(int step) const;
    void set_full_output(int step, const Words &val);

    // return the 4-word final output
    // make sure all 64 steps are performed before calling this method
    Words final_output() const;

   private:
    // store the IV
    Words _iv;
    // store the message
    Words _message;
    // store 68 words of states, including 4 words from IV and
    // 64 words generated during calculation
    Words _states = Words(68);
};

Bytes hash_string(const std::string &s);

}  // namespace Md5

#endif  // COLLISION_ATTACK_MD5_HPP_