#ifndef COLLISION_ATTACK_MD5_HPP_
#define COLLISION_ATTACK_MD5_HPP_

#include <stddef.h>

#include <string>
#include <vector>

#include "common.hpp"
#include "constants.hpp"

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

// padding the input bytes
Bytes padding(const Bytes &bytes);

// compute which word of message should be used as input for the given step
inline constexpr int msg_idx_of_step(int step) {
    if (step < 16)
        return step;
    else if (step < 32)
        return (5 * step + 1) % 16;
    else if (step < 48)
        return (3 * step + 5) % 16;
    else
        return (7 * step) % 16;
}

// compute ((a + K + msg + f(b,c,d)) <<< S) + b, f is one of F/G/H/I
// step is 0-index, first step is 0
Word perform_one_step(int step, Word msg, Word a, Word b, Word c, Word d);

// similar to another perform_one_step, but determine which word of message is used automatically
Word perform_one_step(int step, const Words &msg, Word a, Word b, Word c, Word d);

class Md5BlockHasher {
   public:
    Md5BlockHasher(const Words &iv, Words msg);

    const Words &get_iv() const;
    void set_iv(const Words &iv);

    const Words &get_msg() const;
    void set_msg(Words msg);
    void set_msg_word(int index, Word word);
    void set_msg_to_ensure_step(int step);

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

    // calculate epsilon of step:
    // epsilon = A + F(B, C, D) + M + K
    // and output = B + (epsilon << S)
    Word epsilon(int step) const;

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