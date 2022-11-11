#ifndef COLLISION_ATTACK_ATTACK_HPP_
#define COLLISION_ATTACK_ATTACK_HPP_

#include <ctime>

#include "common.hpp"
#include "md5.hpp"
#include "tunnel.hpp"

namespace Attack {

// return the bits of a and b under the mask is equal
inline constexpr bool mask_eq(Word a, Word b, Word mask) {
    return ((a ^ b) & mask) == 0;
}
// return the p-th bit of a and b is equal (bit are 0-index)
inline constexpr bool bit_eq(Word a, Word b, int p) {
    return mask_eq(a, b, 1u << p);
}

std::pair<Words, Words> collision(const Words &iv = Md5::IV);

void apply_simple_modification(Md5::Md5BlockHasher &hasher,
                               const PartialWord const_cond[], const PartialWord adj_cond[]);

bool check_cond(Word src, Word adj_val, const PartialWord &const_cond, const PartialWord &adj_cond);

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

constexpr PartialWord CONST_COND_0[] = {
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
    PartialWord(-18 /*epsilon extra*/, 31),      // step 16 a5
    PartialWord(18),                             // step 17 d5
    PartialWord(-18),                            // step 18 c5
    PartialWord(),                               // step 19 b5
};

constexpr PartialWord ADJ_COND_0[] = {
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
    PartialWord(4, 16, 32),                  // step 16 a5
    PartialWord(30, 32),                     // step 17 d5
    PartialWord(32),                         // step 18 c5
    PartialWord(32)                          // step 19 b5
};

inline bool block_0_other_cond_ok(Md5::Md5BlockHasher &h) {
    constexpr Md5::StepOf s = Md5::step_of;

    auto oa = [&h](int i) { return h.output_of(Md5::step_of.a(i)); };
    auto ob = [&h](int i) { return h.output_of(Md5::step_of.b(i)); };
    auto oc = [&h](int i) { return h.output_of(Md5::step_of.c(i)); };
    auto od = [&h](int i) { return h.output_of(Md5::step_of.d(i)); };
    auto adj_eq = [&h](int step, int pos) {
        return bit_eq(h.output_of(step), h.output_of(step - 1), pos);
    };

    Words out = h.final_output();
    Word aa0 = out[0];
    Word bb0 = out[1];
    Word cc0 = out[2];
    Word dd0 = out[3];

    return
        // a6,18=b5,18, d6,32=a6,32=b5,32, c6,32 = 0, b6,32 = c6,32 + 1, b12,32 = d12,32
        adj_eq(s.a(6), 17) &&
        adj_eq(s.a(6), 31) &&
        adj_eq(s.d(6), 31) &&
        bit_eq(oc(6), 0, 31) &&
        !adj_eq(s.b(6), 31) &&
        bit_eq(ob(12), od(12), 31) &&
        // eps23, 18=0, eps35, 16=0
        bit_eq(h.epsilon(22), 0, 17) &&
        bit_eq(h.epsilon(34), 0, 15) &&
        // a13,32 = c12,32, d13,32 = b12,32+1, c13,32 = a13,32, b13,32 = d13,32
        bit_eq(oa(13), oc(12), 31) &&
        !bit_eq(od(13), ob(12), 31) &&
        bit_eq(oc(13), oa(13), 31) &&
        bit_eq(ob(13), od(13), 31) &&
        // a14,32 = c13,32, d14,32 = b13,32, c14,32 = a14,32, b14,32 =d14,32
        bit_eq(oa(14), oc(13), 31) &&
        bit_eq(od(14), ob(13), 31) &&
        bit_eq(oc(14), oa(14), 31) &&
        bit_eq(ob(14), od(14), 31) &&
        // a15,32 = c14,32 , d15,32 = b14,32 , c15,32 = a15,32, b15,32 = d15,32 + 1, b15,26 = 0,
        bit_eq(oa(15), oc(14), 31) &&
        bit_eq(od(15), ob(14), 31) &&
        bit_eq(oc(15), oa(15), 31) &&
        !bit_eq(ob(15), od(15), 31) &&
        bit_eq(ob(15), 0, 25) &&
        // a16,26 = 1, a16,32 = c15,32
        bit_eq(oa(16), -1, 25) &&
        bit_eq(oa(16), oc(15), 31) &&
        // dd0,26 = 0, d16,26= 0, d16,32 = b15,32
        bit_eq(dd0, 0, 25) &&
        bit_eq(od(16), 0, 25) &&
        bit_eq(od(16), ob(15), 31) &&
        // eps62,16~eps62,22 not all ones
        !mask_eq(h.epsilon(61), -1, p2sum(15, 16, 17, 18, 19, 20, 21)) &&
        // cc0,26 = 1, cc0,27 = 0, c16,26= 0, c16,32 = a16,32
        bit_eq(cc0, -1, 25) &&
        bit_eq(cc0, 0, 26) &&
        bit_eq(oc(16), 0, 25) &&
        bit_eq(oc(16), oa(16), 31) &&
        // bb0,26 = 0, bb0,27 = 0, bb0,6 = 0, bb0,32 = cc0,32 = dd0,32
        bit_eq(bb0, 0, 25) &&
        bit_eq(bb0, 0, 26) &&
        bit_eq(bb0, 0, 5) &&
        bit_eq(bb0, cc0, 31) &&
        bit_eq(dd0, cc0, 31) &&
        // dummy true
        true;
}

constexpr PartialWord CONST_COND_1[] = {
    PartialWord(-6, -12, 22, -26, 27, -28,
                /*epsilon extra*/ 17),  // step 0 a1
    PartialWord(-2, -3, -6, 12, 17, -22, -26, 27, 28,
                /*epsilon extra*/ -16),  // step 1 d1
    PartialWord(2, 3, 6, 7, -8, 9, 12, -13, 17, 18,
                19, 20, 21, -22, 26, 27, 28, 29, 30, -31,
                /*epsilon extra*/ 1, 16),  // step 2 c1
    PartialWord(1, -2, -3, -4, 5, -6, -7, -8, -9,
                -12, -13, -17, -18, 19, -20, -21,
                -22, 26, -27, 28, 29, 30, -31),  // step 3 b1
    PartialWord(-1, -2, -3, -4, 5, -6, 7, -8, -9,
                10, 11, 12, -13, 17, 18, 19, 20,
                -21, 22, -27, 28, -29, -30, 31),  // step 4 a2
    PartialWord(-1, 2, 3, -4, 5, -6, 7, -8, -9,
                -10, 11, 12, -13, -17, 18, -21, 22,
                -26, 27, -28, -29,
                /*epsilon extra*/ 15),  // step 5 d2
    PartialWord(1, -7, -8, -9, 10, 11, 12, 13, 17,
                -18, -21, -22, 26, 27, -28, 29,
                /*epsilon extra*/ -15),  // step 6 c2
    PartialWord(-1, 7, 8, 9, 10, 16, -17, 18,
                21, 22, -24, -25, -26, 27, -28, -29,
                /*epsilon extra*/ 6),  // step 7 b2
    PartialWord(1, -2, 7, 8, 9, -10, -16, 17, -18,
                -24, -25, -26, 27, 28, 29,
                /*epsilon extra*/ -6),  // step 8 a3
    PartialWord(-1, -2, 7, 8, 9, 10, -13, 16,
                17, 18, -19, 24, 25, 26, 27,
                /*epsilon extra*/ 12),  // step 9 d3
    PartialWord(1, 2, 7, 8, 9, 10, -13, 16, 17, -18, 19,
                /*epsilon extra*/ -12),                    // step 10 c3
    PartialWord(8, 13, -14, -15, -16, -17, -18, -19, 20),  // step 11 b3
    PartialWord(4, -8, 14, 15, 16, 17, 18, 19, 20,
                25, 26, 27, 28, 29, 30, -31,
                /*epsilon extra*/ -24),  // step 12 a4
    PartialWord(4, 8, 14, 15, 16, 17, 18, -19, 20,
                -25, -26, -27, -28, -29, -30, 31,
                /*epsilon extra*/ 24,
                /*search extra*/ 21, 22, 23),  // step 13 d4
    PartialWord(-4, -16, 25, -26, 27, 28, 29, 30, 31,
                /*epsilon extra*/ -17),  // step c4 14
    PartialWord(4, /*epsilon extra*/ 16, 17, -29,
                /* fastest fix */ 30),  // step b4 15
    PartialWord(-18),                   // step 16 a5
    PartialWord(18),                    // step 17 d5
    PartialWord(-18),                   // step 18 c5
    PartialWord(),                      // step 19 b5
};

constexpr PartialWord ADJ_COND_1[] = {
    PartialWord(-32),                                       // step 0  a1
    PartialWord(7, 8, 13, 18, 19, 20, 21, 29, 30, 31, 32),  // step 1  d1
    PartialWord(4, 5, 32),                                  // step 2  c1
    PartialWord(10, 11, 32),                                // step 3  b1
    PartialWord(-32),                                       // step 4  a2
    PartialWord(32),                                        // step 5  d2
    PartialWord(16, 24, 25, -32),                           // step 6  c2
    PartialWord(2, 32),                                     // step 7  b2
    PartialWord(13, 32),                                    // step 8  a3
    PartialWord(32),                                        // step 9  d3
    PartialWord(14, 15, 20, 32),                            // step 10 c3
    PartialWord(25, 26, 27, 28, 29, 30, 31, 32),            // step 11 b3
    PartialWord(-32),                                       // step 12 a4
    PartialWord(32),                                        // step 13 d4
    PartialWord(),                                          // step 14 c4
    PartialWord(32),                                        // step 15 b4
    PartialWord(4, 16, 32,
                /* fastest fix */ -31),  // step 16 a5
    PartialWord(30, 32),                 // step 17 d5
    PartialWord(32),                     // step 18 c5
    PartialWord(32),                     // step 19 b5
};

constexpr PartialWord BAD_EPS_1[] = {
    PartialWord(),            // step 0  a1
    PartialWord(),            // step 1  d1
    PartialWord(),            // step 2  c1
    PartialWord(),            // step 3  b1
    PartialWord(),            // step 4  a2
    PartialWord(),            // step 5  d2
    PartialWord(),            // step 6  c2
    PartialWord(),            // step 7  b2
    PartialWord(),            // step 8  a3
    PartialWord(),            // step 9  d3
    PartialWord(),            // step 10 c3
    PartialWord(),            // step 11 b3
    PartialWord(),            // step 12 a4
    PartialWord(),            // step 13 d4
    PartialWord(),            // step 14 c4
    PartialWord(),            // step 15 b4
    PartialWord(25, 26, 27),  // step 16 a5
    PartialWord(),            // step 17 d5
    PartialWord(4, 5, 6, 7, 8, 9, 10, 11, 12,
                13, 14, 15, 16, 17, 18),  // step 18 c5
    PartialWord(-30, -31, -32),           // step 19 b5
};

}  // namespace Attack

#endif  // COLLISION_ATTACK_ATTACK_HPP_
