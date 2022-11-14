#ifndef COLLISION_ATTACK_TUNNEL_EVAL_TEST_HPP_
#define COLLISION_ATTACK_TUNNEL_EVAL_TEST_HPP_

#include "constants.hpp"
#include "md5.hpp"
#include "tunnel.hpp"
#include "util.hpp"

inline void evaluate_q4(Words msg) {
    Md5::Md5BlockHasher h = Md5::Md5BlockHasher(Md5::IV, msg);
    h.cal_all();

    Word s5 = 0x00000000;
    Word s6 = 0xFFFFFFFF;
    Words state56_1 = Words{s5, s6};

    Word nq_1_1 = 0x0000FFFF;
    Word nq_1_2 = 0xFFFF0000;
    Word nq_1_3 = 0xAAAAAAAA;
    Words newq4_1 = Words{nq_1_1, nq_1_2, nq_1_3};

    Tunnel::verify_q4_tunnel(h, state56_1, newq4_1);
}

inline void evaluate_q9(Words msg) {
    Md5::Md5BlockHasher h = Md5::Md5BlockHasher(Md5::IV, msg);
    h.cal_all();

    Word s10 = 0x00000000;
    Word s11 = 0xFFFFFFFF;
    Words state56_1 = Words{s10, s11};

    Word nq_1_1 = 0x0000FFFF;
    Word nq_1_2 = 0xFFFF0000;
    Word nq_1_3 = 0xAAAAAAAA;
    Words newq4_1 = Words{nq_1_1, nq_1_2, nq_1_3};

    Tunnel::verify_q9_tunnel(h, state56_1, newq4_1);
}

inline void evaluate_q14(Words msg) {
    std::cout << std::hex;
    Md5::Md5BlockHasher h = Md5::Md5BlockHasher(Md5::IV, msg);
    h.cal_all();

    // s3 and s4 should be dynamically computed by q5
    Word q5 = h.output_of(4);
    Word old_q3 = h.output_of(2);
    Word old_q4 = h.output_of(3);
    std::cout << "old q5: " << q5 << "\n";
    Word s3 = old_q3 ^ (0xFFFFFFFF & q5);
    Word s4 = old_q4 ^ (0xFFFFFFFF & ~q5);
    Word s15 = 0x00000000;
    Word s16 = 0x00000000;
    Words state56_1 = Words{s3, s4, s15, s16};

    Tunnel::verify_q14_tunnel(h, state56_1);
}

#endif  // COLLISION_ATTACK_TUNNEL_EVAL_TEST_HPP_
