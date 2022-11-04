#ifndef COLLISION_ATTACK_COMMON_HPP_
#define COLLISION_ATTACK_COMMON_HPP_

#include <stddef.h>

#include <string>
#include <vector>

// common data type
using Byte = uint8_t;
using Word = uint32_t;
using Bytes = std::vector<Byte>;
using Words = std::vector<Word>;

// represent a subset of word
struct PartialWord {
    // define the part to be represented
    Word mask = 0;
    // define the value of the partial word
    Word value = 0;

    constexpr PartialWord() : mask(0), value(0) {}

    // create a partial word
    // bit_pos is 1-index position of bits
    // if bit_pos is negative, the value of corresponding bit will be 0
    // e.g PartialWord(1, 2, 3) == {mask: 7, value: 7}
    // e.g PartialWord(-1, -2, 3) == {mask: 7, value: 4}
    template <typename... Ts>
    constexpr PartialWord(int bit_pos, Ts... other_pos) : PartialWord(other_pos...) {
        Word bit_val = 1;
        if (bit_pos < 0) {
            bit_val = 0;
            bit_pos = -bit_pos;
        }
        --bit_pos;
        mask |= (1 << bit_pos);
        value |= (bit_val << bit_pos);
    }
};

#endif
