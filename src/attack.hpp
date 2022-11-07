#ifndef COLLISION_ATTACK_ATTACK_HPP_
#define COLLISION_ATTACK_ATTACK_HPP_

#include <ctime>

#include "common.hpp"
#include "md5.hpp"
#include "tunnel.hpp"

namespace Attack {

std::pair<Words, Words> collision(const Words &iv = Md5::IV);

void apply_simple_modification(Md5::Md5BlockHasher &hasher,
                               const PartialWord const_cond[], const PartialWord adj_cond[]);

// p = power, p2 = 2 to the power of
constexpr inline Word p2(int power) {
    return 1u << power;
}
// base case of p2sum
constexpr inline Word p2sum(int power) {
    Word base = 1;
    if (power < 0) {
        base = -1;
        power = -power;
    }
    return base << power;
}
// sum of the power of 2
// negative will be considered as subtraction
// -1 cannot be represented with this function, please manually add -1 after this function
template <typename... Ts>
constexpr inline Word p2sum(int power, Ts... powers) {
    return p2sum(power) + p2sum(powers...);
}

const Word M0_DIFF[] = {0x0, 0x0, 0x0, 0x0,
                        1u << 31, 0x0, 0x0, 0x0,
                        0x0, 0x0, 0x0, 1u << 15,
                        0x0, 0x0, 1u << 31, 0x0};
const Word M1_DIFF[] = {0x0, 0x0, 0x0, 0x0,
                        1u << 31, 0x0, 0x0, 0x0,
                        0x0, 0x0, 0x0, -1u << 15,
                        0x0, 0x0, 1u << 31, 0x0};

const Word H1_DIFF[] = {
    p2(31),
    p2sum(31, 25),
    p2sum(31, 25),
    p2sum(31, 25)};

const Word H2_DIFF[] = {0, 0, 0, 0};

constexpr Word OUTPUT_DIFF_0[Md5::POV] = {
    0x0, 0x0, 0x0, 0x0,
    -p2(6), p2sum(-6, 23, 31), p2sum(-6, 23, -27) - 1, p2sum(0, -15, -17, -23),
    p2sum(0, -6, 31), p2sum(12, 31), p2sum(30, 31), p2sum(-7, -13, 31),
    p2sum(24, 31), p2(31), p2sum(3, -15, 31), p2sum(-29, 31),
    p2(31), p2(31), p2sum(17, 31), p2(31),
    p2(31), p2(31), 0x0, 0x0};

constexpr Word OUTPUT_DIFF_1[Md5::POV] = {
    p2sum(25, 31), p2sum(5, 25, 31), p2sum(5, 11, 16, 25, 31), p2sum(-1, 5, 25, 31),
    p2sum(0, 6, 8, 9, 31), p2sum(-16, -20, 31), p2sum(-6, -27, 31), p2sum(15, -17, -23, 31),
    p2sum(0, 6, 31), p2sum(12, 31), p2sum(31), p2sum(-7, -13, 31),
    p2sum(24, 31), p2sum(31), p2sum(3, 15, 31), p2sum(-29, 31),
    p2sum(31), p2sum(31), p2sum(17, 31), p2sum(31),
    p2sum(31), p2sum(31), 0x0, 0x0};
// 2332032997U, 41925697U
constexpr PartialWord SIMPLE_CONST_COND_0[] = {
    PartialWord(),              // step 0 a1
    PartialWord(),              // step 1 d1
    PartialWord(-7, -12, -20),  // step 2 c1
    PartialWord(-7, 12, 20, -24, 32,
                /*epsilon extra*/ 5, 6),  // step 3 b1
    PartialWord(1, 3, 6, -7, -8, -9, -10, -11, -12,
                -13, -14, -15, -16, -17, -18, -19,
                -20, -21, -22, 23, -24, -26, 28, 32,
                /*epsilon extra*/ -5, -27, -29, -30, -31),  // step 4 a2
    PartialWord(1, -3, -6, 7, -8, -9, -10, 11, 12,
                13, 14, -15, 16, 17, 18, 19, 20,
                21, 22, 23, -24, 26, -28, -32),  // step 5 d2
    PartialWord(-1, -2, -3, -4, -5, 6, -7, -8,
                -9, -10, -11, 12, 13, 14, 15, 16,
                -17, 18, 19, 20, 21, 22, 23, 24, 25,
                26, -27, -28, -29, -30, -31, -32),  // step 6 c2
    PartialWord(-1, -2, -3, -4, -5, -6, 7, -8,
                9, -10, 11, -12, -14, -16, 17,
                -18, -19, -20, 21, 24, 25, -26,
                -27, -28, -29, -30, -31, -32),  // step 7 b2
    PartialWord(1, -2, 3, 4, 5, 6, -7, -8,
                9, 10, 11, 12, 14, -16, -17,
                -18, -19, -20, 21, 25, 26,
                -27, 28, 29, 30, 31, 32),  // step 8 a3
    PartialWord(-1, -2, 7, -8, -9, 13, -14, 16,
                17, 18, 19, 20, 21, -24, 31, -32,
                /*epsilon extra*/ 29, 30),  // step 9 d3
    PartialWord(-1, 2, 7, 8, -9, -13, -14, 16,
                17, -18, -19, -20, -31, -32,
                /*epsilon extra*/ -29, 30),  // step 10 c3
    PartialWord(-8, 9, 13, -14, -15, -16, -17,
                -18, 20, -19, -31, -32,
                /*epsilon extra*/ -30),  // step 11 b3
    PartialWord(4, -8, -9, 14, 15, 16, 17,
                18, 19, 20, 25, -26, 31, -32),  // step 12 a4
    PartialWord(4, 8, 9, 14, 15, 16, 17, 18,
                -19, 20, -25, -26, -30, -32,
                /*epsilon extra*/ -31),  // step 13 d4
    PartialWord(-4, 16, 25, -26, 30,
                /*epsilon extra*/ -15, 31,
                /*search extra*/ -9, -21, -23),  // step 14 c4
    PartialWord(30 /*epsilon extra*/, -31),      // step 15 b4
};

constexpr PartialWord SIMPLE_ADJ_COND_0[] = {
    PartialWord(),  // step 0 a1
    PartialWord(),  // step 1 d1
    PartialWord(),  // step 2 c1
    PartialWord(8, 9, 10, 11, 13, 14, 15,
                16, 17, 18, 19, 21, 22, 23
                // 8, 9, 10, 11, 13, 14, 15,
                // 16, 17, 18, 19, 21, 22, 23
                ),  // step 3 b1
    PartialWord(),  // step 4 a2
    PartialWord(2, 4, 5, 25, 27, 29, 30, 31
                // 2,4,5,25,27,29,30,31
                ),                           // step 5 d2
    PartialWord(),                           // step 6 c2
    PartialWord(),                           // step 7 b2
    PartialWord(13),                         // step 8 a3
    PartialWord(),                           // step 9 d3
    PartialWord(15),                         // step 10 c3
    PartialWord(25, 26),                     // step 11 b3
    PartialWord(),                           // step 12 a4
    PartialWord(),                           // step 13 d4
    PartialWord(),                           // step 14 c4
    PartialWord(32, /*epsilon extra*/ -22),  // step 15 b4
};

constexpr PartialWord SIMPLE_CONST_COND_1[] = {
    PartialWord(-6, -12, 22, -26, 27, -28, 32,
                /*epsilon extra*/ 17),  // step a1 0
    PartialWord(-2, -3, -6, 12, 17, -22, -26, 27, 28,
                /*epsilon extra*/ -16),  // step d1 1
    PartialWord(2, 3, 6, 7, -8, 9, 12, -13, 17, 18,
                19, 20, 21, -22, 26, 27, 28, 29, 30, -31,
                /*epsilon extra*/ 1, 16),  // step c1 2
    PartialWord(1, -2, -3, -4, 5, -6, -7, -8, -9,
                -12, -13, -17, -18, 19, -20, -21,
                -22, 26, -27, 28, 29, 30, -31),  // step b1 3
    PartialWord(-1, -2, -3, -4, 5, -6, 7, -8, -9,
                10, 11, 12, -13, 17, 18, 19, 20,
                -21, 22, -27, 28, -29, -30, 31),  // step a2 4
    PartialWord(-1, 2, 3, -4, 5, -6, 7, -8, -9,
                -10, 11, 12, -13, -17, 18, -21, 22,
                -26, 27, -28, -29,
                /*epsilon extra*/ 15),  // step d2 5
    PartialWord(1, -7, -8, -9, 10, 11, 12, 13, 17,
                -18, -21, -22, 26, 27, -28, 29,
                /*epsilon extra*/ -15),  // step c2 6
    PartialWord(-1, 7, 8, 9, 10, 16, -17, 18,
                21, 22, -24, -25, -26, 27, -28, -29,
                /*epsilon extra*/ 6),  // step b2 7
    PartialWord(1, -2, 7, 8, 9, -10, -16, 17, -18,
                -24, -25, -26, 27, 28, 29,
                /*epsilon extra*/ -6),  // step a3 8
    PartialWord(-1, -2, 7, 8, 9, 10, -13, 16,
                17, 18, -19, 24, 25, 26, 27,
                /*epsilon extra*/ 12),  // step d3 9
    PartialWord(1, 2, 7, 8, 9, 10, -13, 16, 17, -18, 19,
                /*epsilon extra*/ -12),                    // step c3 10
    PartialWord(8, 13, -14, -15, -16, -17, -18, -19, 20),  // step b3 11
    PartialWord(4, -8, 14, 15, 16, 17, 18, 19, 20,
                25, 26, 27, 28, 29, 30, -31,
                /*epsilon extra*/ -24),  // step a4 12
    PartialWord(4, 8, 14, 15, 16, 17, 18, -19, 20,
                -25, -26, -27, -28, -29, -30, 31,
                /*epsilon extra*/ 24,
                /*search extra*/ 21, 22, 23),  // step d4 13
    PartialWord(-4, -16, 25, -26, 27, 28, 29, 30, 31,
                /*epsilon extra*/ -17),             // step c4 14
    PartialWord(4, /*epsilon extra*/ 16, 17, -29),  // step b4 15
};

constexpr PartialWord SIMPLE_ADJ_COND_1[] = {
    PartialWord(-32),                                       // step a1 0
    PartialWord(7, 8, 13, 18, 19, 20, 21, 29, 30, 31, 32),  // step d1 1
    PartialWord(4, 5, 32),                                  // step c1 2
    PartialWord(10, 11, 32),                                // step b1 3
    PartialWord(-32),                                       // step a2 4
    PartialWord(32),                                        // step d2 5
    PartialWord(16, 24, 25, -32),                           // step c2 6
    PartialWord(2, 32),                                     // step b2 7
    PartialWord(13, 32),                                    // step a3 8
    PartialWord(32),                                        // step d3 9
    PartialWord(14, 15, 20, 32),                            // step c3 10
    PartialWord(25, 26, 27, 28, 29, 30, 31, 32),            // step b3 11
    PartialWord(-32),                                       // step a4 12
    PartialWord(32),                                        // step d4 13
    PartialWord(),                                          // step c4 14
    PartialWord(32),                                        // step b4 15
};

}  // namespace Attack

#endif  // COLLISION_ATTACK_ATTACK_HPP_
