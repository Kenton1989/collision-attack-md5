#include "tunnel.hpp"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "constants.hpp"
#include "md5.hpp"
#include "util.hpp"

namespace Tunnel {

/*
struct Modification {
    int index;
    Word new_val;
};
*/

inline std::vector<Solution> q9_solve(const Md5::Md5BlockHasher &h, Word searched_bits_mask) {
    Word q5 = h.output_of(4);
    Word q6 = h.output_of(5);
    Word q7 = h.output_of(6);
    Word q8 = h.output_of(7);
    Word q9 = h.output_of(8);
    Word q10 = h.output_of(9);
    Word q11 = h.output_of(10);
    Word q12 = h.output_of(11);
    Word q13 = h.output_of(12);

    std::vector<Solution> solutions = std::vector<Solution>();

    // Word searched_bits = searched_bits_mask & ~(q10 ^ 0) & ~(q11 ^ -1);
    Word searched_bits = searched_bits_mask & ~q10 & q11;

    /**
     * this piece of code uses a bit of trick, that
     * it sets bits that are not in search bits so that they can take carry to next search bit toward MSB.
     */
    for (SubmaskIter it(searched_bits, false); it.has_next(); it.next()) {
        Word new_q9 = q9 ^ it.val();

        // solve for x that caused by new q9 in q9 tunnel
        // use K[8] for q9
        Word new_x8 = r_rotate(new_q9 - q8, 7) - Md5::F(q8, q7, q6) - q5 - Md5::K[8];
        Word new_x9 = r_rotate(q10 - new_q9, 12) - Md5::F(new_q9, q8, q7) - q6 - Md5::K[9];
        Word new_x12 = r_rotate(q13 - q12, 7) - Md5::F(q12, q11, q10) - new_q9 - Md5::K[12];

        // instantiate current step modification
        Modification m8 = Modification(8, new_x8);
        Modification m9 = Modification(9, new_x9);
        Modification m12 = Modification(12, new_x12);

        std::vector<Modification> mods = std::vector<Modification>{m8, m9, m12};

        // pov is hardcoded to 24 for q9 tunnel: all accessed message block
        // doesn't appear before q25
        Solution cur_s = Solution(24, mods);
        solutions.push_back(cur_s);
    }

    return solutions;
}

// searched_bits_mask & ~(q5 ^ 0) & ~(q6 ^ -1)
std::vector<Solution> q4_solve(const Md5::Md5BlockHasher &h, Word searched_bits_mask) {
    Word q0 = h.output_of(-1);
    Word q1 = h.output_of(0);
    Word q2 = h.output_of(1);
    Word q3 = h.output_of(2);
    Word q4 = h.output_of(3);
    Word q5 = h.output_of(4);
    Word q6 = h.output_of(5);
    Word q7 = h.output_of(6);
    Word q8 = h.output_of(7);

    std::vector<Solution> solutions = std::vector<Solution>();

    // Word searched_bits = searched_bits_mask & ~q5 & q6
    Word searched_bits = searched_bits_mask & ~q5 & q6;

    Word inv_searched = ~searched_bits;
    /**
     * this piece of code uses a bit of trick, that
     * it sets bits that are not in search bits so that they can take carry to next search bit toward MSB.
     */
    for (Word changed_bits = (inv_searched + 1) & searched_bits;            // a mask with only the lsb search bit as 1
         changed_bits != 0;                                                 // if this is 0, it loops a cycle actually and equal original message already
         changed_bits = (changed_bits + inv_searched + 1) & searched_bits)  // update to next searching pattern
    {
        Word new_q4 = q4 ^ changed_bits;

        // solve for x that caused by new q4 in q4 tunnel
        // use K[8] for q9
        Word new_x3 = r_rotate(new_q4 - q3, Md5::S[3]) - Md5::F(q3, q2, q1) - q0 - Md5::K[3];
        Word new_x4 = r_rotate(q5 - new_q4, Md5::S[4]) - Md5::F(new_q4, q3, q2) - q1 - Md5::K[4];
        Word new_x7 = r_rotate(q8 - q7, Md5::S[7]) - Md5::F(q7, q6, q5) - new_q4 - Md5::K[7];

        // instantiate current step modification
        Modification m3 = Modification(3, new_x3);
        Modification m4 = Modification(4, new_x4);
        Modification m7 = Modification(7, new_x7);

        std::vector<Modification> mods = std::vector<Modification>{m3, m4, m7};

        // pov is hardcoded to 23 for q9 tunnel: all accessed message block except x4
        // doesn't appear before q25.
        // but, x4 can violate sufficient condition on bit 32 for q24 computation, by taking carry to that bit,
        // with a low prob
        Solution cur_s = Solution(23, mods);
        solutions.push_back(cur_s);
    }

    return solutions;
}

// // q15 and q16 have relevant bits 0
// // deprecated: I can't solve boolean-modulus ring mixed two equations with two vars
// inline std::vector<Solution> q14_solve(const Md5::Md5BlockHasher &h, Word searched_bits_mask) {
//     Word q_1 = h.output_of(-2);
//     Word q0 = h.output_of(-1);
//     Word q1 = h.output_of(0);
//     Word q2 = h.output_of(1);
//     Word q3 = h.output_of(2);
//     Word q4 = h.output_of(3);
//     Word q5 = h.output_of(4);
//     Word q6 = h.output_of(5);
//     Word q7 = h.output_of(6);
//     Word q8 = h.output_of(7);
//     Word q10 = h.output_of(9);
//     Word q11 = h.output_of(10);
//     Word q12 = h.output_of(11);
//     Word q13 = h.output_of(12);
//     Word q14 = h.output_of(13);
//     Word q15 = h.output_of(14);
//     Word q16 = h.output_of(15);
//     Word q17 = h.output_of(16);
//     Word q18 = h.output_of(17);

//     std::vector<Solution> solutions = std::vector<Solution>();

//     // q15 = q16 = 0 on relevant bits
//     Word searched_bits = searched_bits_mask & ~q15 & ~q16;

//     Word inv_searched = ~searched_bits;
//     /**
//      * this piece of code uses a bit of trick, that
//      * it sets bits that are not in search bits so that they can take carry to next search bit toward MSB.
//      */
//     for (Word changed_bits = (inv_searched + 1) & searched_bits;            // a mask with only the lsb search bit as 1
//          changed_bits != 0;                                                 // if this is 0, it loops a cycle actually and equal original message already
//          changed_bits = (changed_bits + inv_searched + 1) & searched_bits)  // update to next searching pattern
//     {
//         // update temp q14 to next valid value
//         Word new_q14 = q14 ^ changed_bits;

//         // use perform_one_step for simplicity, though hardcoded
//         // use K[8] for q9
//         Word new_x13 = r_rotate(new_q14 - q13, 12) - Md5::F(q13, q12, q11) - q10 - Md5::K[13];
//         Word new_x14 = r_rotate(q15 - new_q14, 17) - Md5::F(new_q14, q13, q12) - q11 - Md5::K[14];
//         // eq. to x[17]
//         Word new_x6 = r_rotate(q18 - q17, 9) - Md5::G(q17, q16, q15) - new_q14 - Md5::K[17];

//         // since q5 and x5 unchanged, back-solve q4 and q3: since x5 , q5, q6 unchanged, F(5,4,3) must keep unchanged
//         // then if q5i=1 change q4 else q3
//         Word new_q4 = q5 ^ q4;
//         Word new_q3 = ~q5 ^ q3;
//         // after get new q4 and q3, back solve msg block 2,3,4,7
//         Word new_x2 = r_rotate(new_q3 - q2, 17) - Md5::F(q2, q1, q0) - q_1 - Md5::K[2];
//         Word new_x3 = r_rotate(new_q4 - new_q3, 22) - Md5::F(new_q3, q2, q1) - q0 - Md5::K[3];
//         Word new_x4 = r_rotate(q5 - new_q4, 7) - Md5::F(new_q4, new_q3, q2) - q1 - Md5::K[4];
//         Word new_x7 = r_rotate(q8 - q7, 22) - Md5::F(q7, q6, q5) - new_q4 - Md5::K[7];

//         // instantiate current step modification
//         Modification m2 = Modification(2, new_x2);
//         Modification m3 = Modification(3, new_x3);
//         Modification m4 = Modification(4, new_x4);
//         Modification m6 = Modification(4, new_x6);
//         Modification m7 = Modification(4, new_x7);
//         Modification m13 = Modification(4, new_x13);
//         Modification m14 = Modification(4, new_x14);

//         std::vector<Modification> mods = std::vector<Modification>{m2, m3, m4, m6, m7, m13, m14};

//         // pov is hardcoded to 24 for q9 tunnel: all accessed message block
//         // doesn't appear before q25
//         Solution cur_s = Solution(24, mods);
//         solutions.push_back(cur_s);
//     }

//     return solutions;
// }

Word solve_x_for_step(const Md5::Md5BlockHasher &h, const int step) {
    Word q_3 = h.output_of(step - 4);
    Word q_2 = h.output_of(step - 3);
    Word q_1 = h.output_of(step - 2);
    Word q0 = h.output_of(step - 1);
    Word q1 = h.output_of(step);

    // Word new_x7 = r_rotate(q8 - q7, 7) - Md5::F(q7, q6, q5) - new_q4 - Md5::K[7];
    Word new_x = r_rotate(q1 - q0, Md5::S[step]) - Md5::step_to_func_result(step, q0, q_1, q_2) - q_3 - Md5::K[step];
    return new_x;
}

void verify_q9_tunnel(Md5::Md5BlockHasher &verify_h,
                      const Words specified_states,
                      const Words new_q9) {
    // all words (uint32) should be printed as hex. setting this state
    std::cout << std::hex;

    // length of specified states should be 2 for q9 verification, specifically q10 and q11
    // while they must follow q9 tunnel def on them
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << ">>> Verifying q9 tunnel with speficied q10 and q11:\n";
    std::cout << "----------------------------------------------------------------------\n";
    if (specified_states.size() != 2) throw("in verify q9 tunnel: length of specified state != 2");
    Word q6 = verify_h.output_of(5);
    Word q7 = verify_h.output_of(6);
    Word q8 = verify_h.output_of(7);
    Word old_q9 = verify_h.output_of(8);
    Word q10 = specified_states[0];
    Word q11 = specified_states[1];

    Word search_bit_constraint = ~q10 & q11;

    Word x10;
    Word x11;
    Word cur_x8;
    Word cur_x9;
    Word cur_x12;

    std::cout << "old q9: " << old_q9 << "\n";
    std::cout << "stated q10: " << q10 << "\n";
    std::cout << "stated q11: " << q11 << "\n";
    std::cout << "bits that available for this verification: " << search_bit_constraint << "\n";

    // set output for q10 and q11
    int ct = 9;
    for (Word q : specified_states) {
        verify_h.set_output_of(ct, q);
        ct += 1;
    }

    // 0-indexing of step
    Word q12 = verify_h.cal_step(11);
    verify_h.cal_range(12, 15);
    x10 = solve_x_for_step(verify_h, 10);
    x11 = solve_x_for_step(verify_h, 11);

    // set static yet changed msg words for specified q10 and q11
    verify_h.set_msg_word(10, x10);
    verify_h.set_msg_word(11, x11);

    std::cout << "For specified q10 and q11, x10 and x11 have the following modification:\n";
    std::cout << "x10 to: " << x10 << " ,\n";
    std::cout << "x11 to: " << x11 << " \n";
    std::cout << "these are kept unchanged regardless of q9 if q10 and q11 satisfy tunnel q9 definition.\n";
    std::cout << "computed q12 from q11, q10, old q9, q8, x11: " << q12 << "\n";
    std::cout << "the following evaluation states that as long as q10 and q11 satisfies condition, q12 is independent from";
    std::cout << " corresponding bit in q9.";

    for (Word cur_q9 : new_q9) {
        std::cout << "\n>>> verifying current q9:" << cur_q9 << "\n";

        Word q9_changed = cur_q9 ^ old_q9;
        if (q9_changed & ~search_bit_constraint != 0) {
            std::cout << "Current q9 " << cur_q9 << " violates tunnel constraint on q10 and q11, continue\n";
            continue;
        }

        verify_h.set_output_of(8, cur_q9);
        cur_x8 = solve_x_for_step(verify_h, 8);
        cur_x9 = solve_x_for_step(verify_h, 9);
        cur_x12 = solve_x_for_step(verify_h, 12);

        verify_h.set_msg_word(8, cur_x8);
        verify_h.set_msg_word(9, cur_x9);
        verify_h.set_msg_word(12, cur_x12);
        std::cout << "to generate this new q9 from old q9 " << old_q9 << ", solution for x8 is: " << cur_x8 << "\n";
        std::cout << "to keep q13 unchanged, solution for x12 is: " << cur_x12 << "\n";
        std::cout << "q10 = q9 + RL12(F(9,8,7) + q6 + x9 + K[9])\n";
        std::cout << "    = " << cur_q9 << " + RL12( " << Md5::step_to_func_result(9, cur_q9, q8, q7) << " + "
                  << q6 << " + " << cur_x9 << " + " << Md5::K[9] << ")\n";
        // need to set h message!
        std::cout << "    = " << verify_h.cal_step(9) << "       (q10 specified = " << q10 << ")\n";

        Word cur_q10 = verify_h.output_of(9);
        std::cout << "q11 = q10 + RL17(F(10,9,8) + q7 + x10 + K[10])\n";
        std::cout << "    = " << cur_q10 << " + RL17( " << Md5::step_to_func_result(10, cur_q10, cur_q9, q8) << " + "
                  << q7 << " + " << x10 << " + " << Md5::K[10] << ")\n";
        std::cout << "    = " << verify_h.cal_step(10) << "       (q11 specified = " << q11 << ")\n";

        Word cur_q11 = verify_h.output_of(10);
        std::cout << "q12 = q11 + RL22(F(11,10,9) + q8 + x11 + K[11])\n";
        std::cout << "    = " << cur_q11 << " + RL17( " << Md5::step_to_func_result(11, cur_q11, cur_q10, cur_q9) << " + "
                  << q8 << " + " << x11 << " + " << Md5::K[11] << ")\n";
        std::cout << "    = " << verify_h.cal_step(11) << "       (q12 calculated = " << q12 << ")\n";

        Word cur_q12 = verify_h.output_of(11);
        if (cur_q12 == q12) {
            std::cout << "qiang qiang ~ verified new q12 from q9' equal to old q12~\n"
                      << "since x8, x9, x12 don't appear before q25, by definition q24 keeps the same.\n";
        } else {
            throw "q12 verification fails, something goes wrong, either input q9 conflict with specified q10 and q11 or program fails";
        }
    };
    return;
}

void verify_q4_tunnel(Md5::Md5BlockHasher &verify_h,
                      const Words specified_states,
                      const Words new_q4) {
    // all words (uint32) should be printed as hex. setting this state
    std::cout << std::hex;

    // length of specified states should be 2 for q4 verification, specifically q5 and q6
    // while they must follow q4 tunnel def on them
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << ">>> Verifying q4 tunnel with speficied q5 and q6:\n";
    std::cout << "----------------------------------------------------------------------\n";
    if (specified_states.size() != 2) throw("in verify q4 tunnel: length of specified state != 2");
    Word q1 = verify_h.output_of(0);
    Word q2 = verify_h.output_of(1);
    Word q3 = verify_h.output_of(2);
    Word old_q4 = verify_h.output_of(3);
    Word q5 = specified_states[0];
    Word q6 = specified_states[1];

    Word search_bit_constraint = ~q5 & q6;

    Word x5;
    Word x6;
    Word cur_x3;
    Word cur_x4;
    Word cur_x7;

    std::cout << "old q4: " << old_q4 << "\n";
    std::cout << "stated q5: " << q5 << "\n";
    std::cout << "stated q6: " << q6 << "\n";
    std::cout << "bits that available for this verification: " << search_bit_constraint << "\n";

    int ct = 4;
    for (Word q : specified_states) {
        verify_h.set_output_of(ct, q);
        ct += 1;
    }

    // 0-indexing of step
    Word q7 = verify_h.cal_step(6);
    verify_h.cal_range(7, 10);
    x5 = solve_x_for_step(verify_h, 5);
    x6 = solve_x_for_step(verify_h, 6);

    // set static yet changed message words by q5 and q6
    verify_h.set_msg_word(5, x5);
    verify_h.set_msg_word(6, x6);

    // need to compare old q24 to verify 32nd bit suff cond for q24
    verify_h.cal_range(5, 24);
    Word old_q24 = verify_h.output_of(23);
    Word old_q23 = verify_h.output_of(22);
    double eq_case = 0;
    double ne_case = 0;

    std::cout << "For specified q5 and q6, x5 and x6 have the following modification:\n";
    std::cout << "x5 to: " << x5 << " ,\n";
    std::cout << "x6 to: " << x6 << " \n";
    std::cout << "these are kept unchanged regardless of q9 if q5 and q6 satisfy tunnel q4 definition.\n";
    std::cout << "computed q7 from q6, q5, old q4, q3, x6 " << q7 << "\n";
    std::cout << "the following evaluation states that as long as q5 and q6 satisfies condition, q7 is independent from";
    std::cout << " corresponding bit in q4.";

    for (Word cur_q4 : new_q4) {
        std::cout << "\n>>> verifying current q4:" << cur_q4 << "\n";
        Word q4_changed = cur_q4 ^ old_q4;
        if (q4_changed & ~search_bit_constraint != 0) {
            std::cout << "Current q9 " << cur_q4 << " violates tunnel constraint on q5 and q6, continue\n";
            continue;
        }

        verify_h.set_output_of(3, cur_q4);
        cur_x3 = solve_x_for_step(verify_h, 3);
        cur_x4 = solve_x_for_step(verify_h, 4);
        cur_x7 = solve_x_for_step(verify_h, 7);

        // set new solutions in
        verify_h.set_msg_word(3, cur_x3);
        verify_h.set_msg_word(4, cur_x4);
        verify_h.set_msg_word(7, cur_x7);
        std::cout << "to generate this new q4 from old q4 " << old_q4 << ", solution for x3 is: " << cur_x3 << "\n";
        std::cout << "to keep q8 unchanged, solution for x7 is: " << cur_x7 << "\n";
        std::cout << "q5 = q4 + RL7(F(4,3,2) + q1 + x4 + K[4])\n";
        std::cout << "    = " << cur_q4 << " + RL7( " << Md5::step_to_func_result(4, cur_q4, q3, q2) << " + "
                  << q1 << " + " << cur_x4 << " + " << Md5::K[4] << ")\n";
        std::cout << "    = " << verify_h.cal_step(4) << "       (q5 specified = " << q5 << ")\n";

        Word cur_q5 = verify_h.output_of(4);
        std::cout << "q6 = q5 + RL12(F(5,4,3) + q2 + x5 + K[5])\n";
        std::cout << "    = " << cur_q5 << " + RL12( " << Md5::step_to_func_result(5, cur_q5, cur_q4, q3) << " + "
                  << q2 << " + " << x5 << " + " << Md5::K[5] << ")\n";
        std::cout << "    = " << verify_h.cal_step(5) << "       (q6 specified = " << q6 << ")\n";

        Word cur_q6 = verify_h.output_of(5);
        std::cout << "q7 = q6 + RL17(F(6,5,4) + q3 + x6 + K[6])\n";
        std::cout << "    = " << cur_q6 << " + RL17( " << Md5::step_to_func_result(6, cur_q6, cur_q5, cur_q4) << " + "
                  << q3 << " + " << x6 << " + " << Md5::K[6] << ")\n";
        std::cout << "    = " << verify_h.cal_step(6) << "       (q7 calculated = " << q7 << ")\n";

        Word cur_q7 = verify_h.output_of(6);
        if (cur_q7 == q7) {
            std::cout << "qiang qiang ~ verified new q7 from q4' equal to old q7~\n";
        } else {
            throw "q7 verification fails, something goes wrong";
        }

        // since modification on x4 may violate 32th bit suff cond for q24,
        // need to further verify q24
        verify_h.cal_range(7, 24);
        Word cur_q23 = verify_h.output_of(22);
        Word cur_q24 = verify_h.output_of(23);

        if (cur_q23 == old_q23) {
            std::cout << "new q23 is the same as old q23 and both have value " << cur_q23 << "\n";
        } else {
            std::cout << "cur q23 " << cur_q23 << " is different from old q23 " << old_q23 << "\n";
        }

        if ((cur_q24 >> 31) == 1) {
            std::cout << "Lucky! new q24 has 32th bit = 1, satisfying suff cond and have value " << cur_q24 << ";\n";
            eq_case += 1;
        } else {
            std::cout << "Opps unlucky, current q24 " << cur_q24 << " violate suff condition =1 on bit 32\n";
            ne_case += 1;
        }
    };
    std::cout << "passed in q4 cases to check: " << ne_case + eq_case << ";\n";
    std::cout << "32th bit suff cond pass rate: " << eq_case / (ne_case + eq_case) << ";\n";
    return;
}

void verify_q14_tunnel(Md5::Md5BlockHasher &verify_h,
                       const Words specified_states) {
    // all words (uint32) should be printed as hex. setting this state
    std::cout << std::hex;

    // length of specified states should be 4 for q14 verification, specifically q3 and q4, q10 and q11
    // while they must follow q14 tunnel def on them
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << ">>> Verifying q14 tunnel with speficied q15 and q16:\n";
    std::cout << "----------------------------------------------------------------------\n";
    if (specified_states.size() != 4) throw("in verify q14 tunnel: length of specified state != 4");
    Word q11 = verify_h.output_of(10);
    Word q12 = verify_h.output_of(11);
    Word q13 = verify_h.output_of(12);
    Word old_q14 = verify_h.output_of(13);

    // need to customize q3 and q4 for this verification based on q5
    Word specified_q3 = specified_states[0];
    Word specified_q4 = specified_states[1];
    Word old_q3 = verify_h.output_of(2);
    Word old_q4 = verify_h.output_of(3);

    // by setting q3 and q4 free we actually let x6 free which relax a condition on group
    // of non-linear equations
    Word q5 = verify_h.output_of(4);
    Word q3 = (specified_q3 & q5) ^ old_q3;
    Word q4 = (specified_q4 & ~q5) ^ old_q4;

    Word q15 = specified_states[2];
    Word q16 = specified_states[3];
    Word q17 = verify_h.output_of(16);
    Word q18 = verify_h.output_of(17);

    Word search_bit_constraint = ~q15 & ~q16;

    Word x15;
    Word x16;
    Word cur_x13;
    Word cur_x14;
    Word cur_x6;

    std::cout << "old q14: " << old_q14 << "\n";
    std::cout << "stated q3: " << q3 << "\n";
    std::cout << "stated q4: " << q4 << "\n";
    std::cout << "stated q15: " << q15 << "\n";
    std::cout << "stated q16: " << q16 << "\n";

    verify_h.set_output_of(2, q3);
    verify_h.set_output_of(3, q4);
    verify_h.set_output_of(15, q15);
    verify_h.set_output_of(16, q16);

    Word x2 = solve_x_for_step(verify_h, 2);
    Word x3 = solve_x_for_step(verify_h, 3);
    Word x4 = solve_x_for_step(verify_h, 4);
    Word x6 = solve_x_for_step(verify_h, 6);
    Word x7 = solve_x_for_step(verify_h, 7);
    verify_h.set_msg_word(2, x2);
    verify_h.set_msg_word(3, x3);
    verify_h.set_msg_word(4, x4);
    verify_h.set_msg_word(6, x6);
    verify_h.set_msg_word(7, x7);

    std::cout << "For specified q3 and q4, to keep q5, x5 unchanged, msg has following modification:\n";
    std::cout << "x2 to " << x2 << "\nx3 to " << x3 << "\nx4 to " << x4 << "\nx6 to " << x6 << "\nx7 to " << x7 << "\n";

    // set static yet changed message words by q15 and q16
    verify_h.set_msg_word(15, x15);
    verify_h.set_msg_word(16, x16);

    Word default_q14 = r_rotate(q18 - q17, 9) - Md5::step_to_func_result(17, q17, q16, q15) - x6 - Md5::K[17];

    // need to compare old q24 to verify 32nd bit suff cond for q24
    verify_h.cal_range(1, 24);
    Word old_q24 = verify_h.output_of(23);
    Word old_q23 = verify_h.output_of(22);

    std::cout << "For specified q15 and q16, x15 and x16 have the following modification:\n";
    std::cout << "x15 to: " << x15 << " ,\n";
    std::cout << "x16 to: " << x16 << " \n";
    std::cout << "computed q17 from q16, q15, old q14, q13, x16 == x0 " << q17 << "\n";
    std::cout << "the following evaluation states that as long as q15 and q16 satisfies condition, q17 is independent from";
    std::cout << " corresponding bit in q14.\n";
    std::cout << "q14 is set to " << default_q14 << " based on x6 condition.\n";

    std::cout << ">>> verifying current q14:" << default_q14 << "\n";
    Word q14_changed = default_q14 ^ old_q14;
    if (q14_changed & ~search_bit_constraint != 0) {
        std::cout << "q14 computed from x6: " << default_q14 << " violates tunnel constraint on q15 and q16, thus return\n";
        return;
    }

    verify_h.set_output_of(13, default_q14);
    cur_x13 = solve_x_for_step(verify_h, 13);
    cur_x14 = solve_x_for_step(verify_h, 14);
    // x6 equivalent to x17
    cur_x6 = solve_x_for_step(verify_h, 17);

    // set new solutions in
    verify_h.set_msg_word(13, cur_x13);
    verify_h.set_msg_word(14, cur_x14);
    verify_h.set_msg_word(17, cur_x6);
    std::cout << "to generate this new q14 from old q14 " << old_q14 << ", solution for x13 is: " << cur_x13 << "\n";
    std::cout << "to keep q18 unchanged, solution for x17 == x6 is: " << cur_x6 << "\n";
    std::cout << "q15 = q14 + RL17(F(14,13,12) + q11 + x14 + K[14])\n";
    std::cout << "    = " << default_q14 << " + RL17( " << Md5::step_to_func_result(14, default_q14, q13, q12) << " + "
              << q11 << " + " << cur_x14 << " + " << Md5::K[14] << ")\n";
    std::cout << "    = " << verify_h.cal_step(14) << "       (q15 specified = " << q15 << ")\n";

    Word cur_q15 = verify_h.output_of(14);
    std::cout << "q16 = q15 + RL22(F(15,14,13) + q12 + x15 + K[15])\n";
    std::cout << "    = " << cur_q15 << " + RL22( " << Md5::step_to_func_result(15, cur_q15, default_q14, q13) << " + "
              << q12 << " + " << x15 << " + " << Md5::K[15] << ")\n";
    std::cout << "    = " << verify_h.cal_step(15) << "       (q16 specified = " << q16 << ")\n";

    Word cur_q16 = verify_h.output_of(15);
    std::cout << "q17 = q16 + RL5(F(16,15,14) + q13 + x16 + K[16])\n";
    std::cout << "    = " << cur_q16 << " + RL5( " << Md5::step_to_func_result(16, cur_q16, cur_q15, default_q14) << " + "
              << q13 << " + " << x16 << " + " << Md5::K[16] << ")\n";
    std::cout << "    = " << verify_h.cal_step(16) << "       (q17 calculated = " << q17 << ")\n";

    Word cur_q17 = verify_h.output_of(16);
    if (cur_q17 == q17) {
        std::cout << "qiang qiang ~ verified new q17 from q14' equal to old q17~\n";
    } else {
        throw "q17 verification fails, something goes wrong";
    }

    // since modification on x4 may violate 32th bit suff cond for q24,
    // need to further verify q24
    verify_h.cal_range(1, 24);
    Word cur_q23 = verify_h.output_of(22);
    Word cur_q24 = verify_h.output_of(23);

    if (cur_q23 == old_q23) {
        std::cout << "new q23 is the same as old q23 and both have value " << cur_q23 << "\n";
    } else {
        std::cout << "cur q23 " << cur_q23 << " is different from old q23 " << old_q23 << "\n";
    }

    if ((cur_q24 >> 31) == 1) {
        std::cout << "Lucky! new q24 has 32th bit = 1, satisfying suff cond and have value " << cur_q24 << ";\n";
    } else {
        std::cout << "Opps unlucky, current q24 " << cur_q24 << " violate suff condition =1 on bit 32\n";
    }
    return;
}

}  // namespace Tunnel