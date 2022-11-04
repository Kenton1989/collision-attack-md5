#include "md5.hpp"

#include <stddef.h>

#include <string>
#include <vector>

#include "constants.hpp"

namespace Md5 {

Words encode_words(const Bytes &s) {
    size_t word_cnt = s.size() / 4;
    Words res(word_cnt);
    for (size_t i = 0; i < res.size(); ++i) {
        size_t j = i * 4;
        // little endian decoding
        res[i] =
            (Word(s[j])) |
            (Word(s[j + 1]) << 8) |
            (Word(s[j + 2]) << 16) |
            Word(s[j + 3] << 24);
    }
    return res;
}

Bytes encode_bytes(const Words &words) {
    size_t byte_cnt = words.size() * 4;
    Bytes res;
    res.reserve(byte_cnt);

    for (size_t i = 0; i < words.size(); ++i) {
        Word w = words[i];
        // little endian encoding
        res.push_back(w & 0xFF);
        res.push_back((w >> 8) & 0xFF);
        res.push_back((w >> 16) & 0xFF);
        res.push_back(w >> 24);
    }
    return res;
}

Bytes encode_bytes(const std::string &s) {
    return Bytes(s.begin(), s.end());
}
std::string encode_string(const Bytes &bytes) {
    return std::string(bytes.begin(), bytes.end());
}

Bytes padding(const Bytes &bytes) {
    uint64_t msg_len = bytes.size() * uint64_t(8);
    size_t blocks = (bytes.size() +
                     1 /*1 byte for necessary padding*/ +
                     8 /*8 byte for storing message length*/ +
                     BLOCK_BYTES - 1 /*change the floor division to celling division*/) /
                    BLOCK_BYTES;

    Bytes res = bytes;
    res.push_back(0x80);
    res.resize(blocks * BLOCK_BYTES, 0x00);

    // the beginning index of the bytes used to store the length of message
    size_t size_beg = res.size() - 8;
    // little endian encoding the size into 8 bytes
    for (int i = 0; i < 8; ++i) {
        Word shifted = (msg_len >> (i * 8));
        res[size_beg + i] = shifted & 0xFF;
    }
    return res;
}

int msg_idx_of_step(int step) {
    if (step < 16)
        return step;
    else if (step < 32)
        return (5 * step + 1) % 16;
    else if (step < 48)
        return (3 * step + 5) % 16;
    else
        return (7 * step) % 16;
}

Word perform_one_step(int step, Word msg, Word a, Word b, Word c, Word d) {
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

Word perform_one_step(int step, const Words &msg, Word a, Word b, Word c, Word d) {
    int i = msg_idx_of_step(step);
    return perform_one_step(step, msg[i], a, b, c, d);
}

Md5BlockHasher::Md5BlockHasher(const Words &iv, const Words &msg) : _message(msg) {
    set_iv(iv);
}

void Md5BlockHasher::set_iv(const Words &iv) {
    _iv = iv;
    // copy the IV into the first 4 values of _states
    _states[0] = _iv[0];  // A
    _states[1] = _iv[3];  // D
    _states[2] = _iv[2];  // C
    _states[3] = _iv[1];  // B
}
void Md5BlockHasher::set_msg(const Words &msg) {
    _message = msg;
}
void Md5BlockHasher::set_msg_word(int index, Word word) {
    _message[index] = word;
}

const Words &Md5BlockHasher::get_iv() const {
    return _iv;
}
const Words &Md5BlockHasher::get_msg() const {
    return _message;
}

void Md5BlockHasher::cal_range(int from, int to) {
    for (int i = from; i <= to; ++i) {
        Word res = perform_one_step(i, _message,
                                    _states[i],
                                    _states[i + 3],
                                    _states[i + 2],
                                    _states[i + 1]);
        _states[i + 4] = res;
    }
}

Words Md5BlockHasher::cal_all() {
    cal_range(FIRST_STEP, LAST_STEP);
    return final_output();
}

Words Md5BlockHasher::cal_to_end(int from) {
    cal_range(from, LAST_STEP);
    return final_output();
}

Word Md5BlockHasher::cal_from_begin(int until) {
    cal_range(FIRST_STEP, until);
    return output_of(until);
}

Word Md5BlockHasher::cal_step(int step) {
    cal_range(step, step);
    return output_of(step);
}

Word Md5BlockHasher::output_of(int step) const {
    return _states[step + 4];
}

void Md5BlockHasher::set_output_of(int step, Word val) {
    _states[step + 4] = val;
}

Word Md5BlockHasher::main_input_of(int step) const {
    return _states[step];
}

void Md5BlockHasher::set_main_input_of(int step, Word val) {
    _states[step] = val;
}

Words Md5BlockHasher::full_input(int step) const {
    return Words({
        _states[step],
        _states[step + 3],
        _states[step + 2],
        _states[step + 1],
    });
}

void Md5BlockHasher::set_full_input(int step, const Words &val) {
    _states[step] = val[0];
    _states[step + 3] = val[1];
    _states[step + 2] = val[2];
    _states[step + 1] = val[3];
}

Words Md5BlockHasher::full_output(int step) const {
    return Words({
        _states[step + 1],
        _states[step + 4],
        _states[step + 3],
        _states[step + 2],
    });
}

void Md5BlockHasher::set_full_output(int step, const Words &val) {
    _states[step + 1] = val[0];
    _states[step + 4] = val[1];
    _states[step + 3] = val[2];
    _states[step + 2] = val[3];
}

Words Md5BlockHasher::final_output() const {
    return Words({_iv[0] + output_of(step_of.a(16)),
                  _iv[1] + output_of(step_of.b(16)),
                  _iv[2] + output_of(step_of.c(16)),
                  _iv[3] + output_of(step_of.d(16))});
}

Bytes hash_string(const std::string &s) {
    Bytes input = encode_bytes(s);
    input = padding(input);

    Words words = encode_words(input);

    Words state = IV;

    for (size_t i = 0; i < words.size(); i += BLOCK_WORDS) {
        Words block(words.begin() + i, words.begin() + i + BLOCK_WORDS);
        Md5BlockHasher hasher(IV, block);
        state = hasher.cal_all();
    }

    return encode_bytes(state);
}

}  // namespace Md5
