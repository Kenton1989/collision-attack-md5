#ifndef COLLISION_ATTACK_TUNNEL_EVAL_TEST_HPP_
#define COLLISION_ATTACK_TUNNEL_EVAL_TEST_HPP_

#include "constants.hpp"
#include "md5.hpp"
#include "tunnel.hpp"
#include "util.hpp"

inline void evaluate_q4(Words msg) {
    Md5::Md5BlockHasher h = Md5::Md5BlockHasher(Md5::IV, msg);
    h.cal_all();

    Word s5_1 = 0x00000000;
    Word s5_2 = 0xFFFFFFFF;
    Words state56_1 = Words{s5_1, s5_2};

    Word nq_1_1 = 0x0000FFFF;
    Word nq_1_2 = 0xFFFF0000;
    Word nq_1_3 = 0xAAAAAAAA;
    Words newq4_1 = Words{nq_1_1, nq_1_2, nq_1_3};

    Tunnel::verify_q4_tunnel(h, state56_1, newq4_1);
}

inline void evaluate_q9(Words msg) {
    Md5::Md5BlockHasher h = Md5::Md5BlockHasher(Md5::IV, msg);
    h.cal_all();

    Word s5_1 = 0x00000000;
    Word s5_2 = 0xFFFFFFFF;
    Words state56_1 = Words{s5_1, s5_2};

    Word nq_1_1 = 0x0000FFFF;
    Word nq_1_2 = 0xFFFF0000;
    Word nq_1_3 = 0xAAAAAAAA;
    Words newq4_1 = Words{nq_1_1, nq_1_2, nq_1_3};

    Tunnel::verify_q9_tunnel(h, state56_1, newq4_1);
}

#endif  // COLLISION_ATTACK_TUNNEL_EVAL_TEST_HPP_
