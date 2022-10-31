#ifndef COLLISION_ATTACK_MD5_HPP_
#define COLLISION_ATTACK_MD5_HPP_

#include <stddef.h>

#include <string>
#include <vector>

namespace Md5
{

    // common data type
    using Byte = uint8_t;
    using Word = uint32_t;
    using Bytes = std::vector<Byte>;
    using Words = std::vector<Word>;

    // encode bytes into words.
    // if length of input mod 4 != 0, the remainder are discarded.
    inline Words encode_words(const Bytes &s, bool big_endian = false)
    {
        size_t word_cnt = s.size() / 4;
        Words res(word_cnt);
        for (size_t i = 0; i < res.size(); ++i)
        {
            size_t j = i * 4;
            if (big_endian)
            {
                res[i] =
                    (Word(s[j]) << 24) |
                    (Word(s[j + 1]) << 16) |
                    (Word(s[j + 2]) << 8) |
                    Word(s[j + 3]);
            }
            else
            {
                res[i] =
                    (Word(s[j])) |
                    (Word(s[j + 1]) << 8) |
                    (Word(s[j + 2]) << 16) |
                    Word(s[j + 3] << 24);
            }
        }
        return res;
    }

    // encode words into bytes.
    inline Bytes encode_bytes(const Words &words, bool big_endian = false)
    {
        size_t byte_cnt = words.size() * 4;
        Bytes res;
        res.reserve(byte_cnt);

        for (size_t i = 0; i < words.size(); ++i)
        {
            Word w = words[i];
            if (big_endian)
            {
                res.push_back(w >> 24);
                res.push_back((w >> 16) & 0xFF);
                res.push_back((w >> 8) & 0xFF);
                res.push_back(w & 0xFF);
            }
            else
            {
                res.push_back(w & 0xFF);
                res.push_back((w >> 8) & 0xFF);
                res.push_back((w >> 16) & 0xFF);
                res.push_back(w >> 24);
            }
        }
        return res;
    }

    inline Bytes encode_bytes(const std::string &s)
    {
        return Bytes(s.begin(), s.end());
    }
    inline std::string encode_string(const Bytes &bytes)
    {
        return std::string(bytes.begin(), bytes.end());
    }

    // rotate operations
    inline Word l_rotate(Word x, int shift)
    {
        return x << shift | (x >> (sizeof(x) * 8 - shift));
    }
    inline Word r_rotate(Word x, int shift)
    {
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
    class StepOf
    {
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
        0x67452301, // A
        0xefcdab89, // B
        0x98badcfe, // C
        0x10325476, // D
    };

    // non linear function used in MD5
    inline Word F(Word b, Word c, Word d) { return (b & c) | (d & ~b); }
    inline Word G(Word b, Word c, Word d) { return (b & d) | (c & ~d); }
    inline Word H(Word b, Word c, Word d) { return b ^ c ^ d; }
    inline Word I(Word b, Word c, Word d) { return c ^ (b | ~d); }

    // padding the input bytes
    inline Bytes padding(const Bytes &bytes)
    {
        uint64_t msg_len = bytes.size() * 8;
        size_t blocks = (bytes.size() +
                         1 /*1 byte for necessary padding*/ +
                         8 /*8 byte for storing message length*/ +
                         63 /*change the floor division to celling division*/) /
                        64;

        Bytes res = bytes;
        res.push_back(0x80);
        res.resize(blocks * BLOCK_BYTES, 0x00);

        size_t size_beg = res.size() - 8;
        for (int i = 0; i < 8; ++i)
        {
            Word shifted = (msg_len >> (i * 8));
            res[size_beg + i] = shifted & 0xFF;
        }
        return res;
    }

    // compute which word of message should be used as input for the given step
    inline int msg_idx_of_step(int step)
    {
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
    // first step (which output a1) is step 0, not step 1
    inline Word perform_one_step(int step, Word msg, Word a, Word b, Word c, Word d)
    {
        Word f;
        if (step < 16)
            f = F(b, c, d);
        else if (step < 32)
            f = G(b, c, d);
        else if (step < 48)
            f = H(b, c, d);
        else
            f = I(b, c, d);

        Word temp = f + a + K[step] + msg;

        Word res = b + l_rotate(temp, S[step]);

        return res;
    }

    // similar to another perform_one_step, but determine which word of message is used automatically
    inline Word perform_one_step(int step, const Words &msg, Word a, Word b, Word c, Word d)
    {
        int i = msg_idx_of_step(step);
        return perform_one_step(step, msg[i], a, b, c, d);
    }

    class Md5BlockHasher
    {
    public:
        Md5BlockHasher(const Words &iv, const Words &msg) : _message(msg)
        {
            set_iv(iv);
        }

        // change data
        void set_iv(const Words &iv)
        {
            _iv = iv;
            // copy the IV into the first 4 values of _states
            _states[0] = _iv[0]; // A
            _states[1] = _iv[3]; // D
            _states[2] = _iv[2]; // C
            _states[3] = _iv[1]; // B
        }
        void set_msg(const Words &msg)
        {
            _message = msg;
        }
        void set_msg_word(int index, Word word)
        {
            _message[index] = word;
        }

        // get state
        const Words &get_iv() const
        {
            return _iv;
        }
        const Words &get_msg() const
        {
            return _message;
        }

        // perform calculation for one step or a range of steps
        // both end points of range are inclusive
        // step is 0 index
        void cal_range(int from, int to)
        {
            for (int i = from; i <= to; ++i)
            {
                Word res = perform_one_step(i, _message,
                                            _states[i],
                                            _states[i + 3],
                                            _states[i + 2],
                                            _states[i + 1]);
                _states[i + 4] = res;
            }
        }
        // calculate all steps and return final 4-word output.
        Words cal_all()
        {
            cal_range(FIRST_STEP, LAST_STEP);
            return final_output();
        }
        // calculate steps from parameter "from" to the end and return final 4-word output.
        Words cal_to_end(int from)
        {
            cal_range(from, LAST_STEP);
            return final_output();
        }
        // calculate steps from begin to parameter "until", return the 1-word result of last calculated step.
        Word cal_from_begin(int until)
        {
            cal_range(FIRST_STEP, until);
            return output_of(until);
        }
        // calculate one step and return the 1-word output of the step
        Word cal_step(int step)
        {
            cal_range(step, step);
            return output_of(step);
        }

        // return the output of the specified step
        Word output_of(int step) const
        {
            return _states[step + 4];
        }

        // return the main input of the specified step
        // e.g.
        // main input of output a2 is a1
        // main input of output b7 is b6
        // main input of output c1 is C
        Word main_input_of(int step) const
        {
            return _states[step];
        }

        // return the 4-word final output
        // make sure all 64 steps are performed before calling this method
        Words final_output() const
        {
            return Words({_iv[0] + output_of(step_of.a(16)),
                          _iv[1] + output_of(step_of.b(16)),
                          _iv[2] + output_of(step_of.c(16)),
                          _iv[3] + output_of(step_of.d(16))});
        }

    private:
        // store the IV
        Words _iv;
        // store the message
        Words _message;
        // store 68 words of states, including 4 words from IV and
        // 64 words generated during calculation
        Words _states = Words(68);
    };

    inline Bytes hash_string(const std::string &s)
    {
        Bytes input = encode_bytes(s);
        input = padding(input);

        Words words = encode_words(input);

        Words state = IV;

        for (size_t i = 0; i < words.size(); i += BLOCK_WORDS)
        {
            Words block(words.begin() + i, words.begin() + i + BLOCK_WORDS);
            Md5BlockHasher hasher(IV, block);
            state = hasher.cal_all();
        }

        return encode_bytes(state);
    }

} // namespace Md5

#endif // COLLISION_ATTACK_MD5_HPP_