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

    Word inv_searched = ~searched_bits;
    /**
     * this piece of code uses a bit of trick, that
     * it sets bits that are not in search bits so that they can take carry to next search bit toward MSB.
     */
    for (Word changed_bits = (inv_searched + 1) & searched_bits;            // a mask with only the lsb search bit as 1
         changed_bits != 0;                                                 // if this is 0, it loops a cycle actually and equal original message already
         changed_bits = (changed_bits + inv_searched + 1) & searched_bits)  // update to next searching pattern
    {
        Word new_q9 = q9 ^ changed_bits;

        // solve for x that caused by new q9 in q9 tunnel
        // use K[8] for q9
        Word new_x8 = Md5::r_rotate(new_q9 - q8, 7) - Md5::F(q8, q7, q6) - q5 - Md5::K[8];
        Word new_x9 = Md5::r_rotate(q10 - new_q9, 12) - Md5::F(new_q9, q8, q7) - q6 - Md5::K[9];
        Word new_x12 = Md5::r_rotate(q13 - q12, 7) - Md5::F(q12, q11, q10) - new_q9 - Md5::K[12];

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
        Word new_x3 = Md5::r_rotate(new_q4 - q3, 22) - Md5::F(q3, q2, q1) - q0 - Md5::K[3];
        Word new_x4 = Md5::r_rotate(q5 - new_q4, 7) - Md5::F(new_q4, q3, q2) - q1 - Md5::K[4];
        Word new_x7 = Md5::r_rotate(q8 - q7, 7) - Md5::F(q7, q6, q5) - new_q4 - Md5::K[7];

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

Word solve_x_for_step(const Md5::Md5BlockHasher &h, const int step) {
    Word q_3 = h.output_of(step - 4);
    Word q_2 = h.output_of(step - 3);
    Word q_1 = h.output_of(step - 2);
    Word q0 = h.output_of(step - 1);
    Word q1 = h.output_of(step);

    // Word new_x7 = Md5::r_rotate(q8 - q7, 7) - Md5::F(q7, q6, q5) - new_q4 - Md5::K[7];
    Word new_x = Md5::r_rotate(q1 - q0, Md5::S[step]) - Md5::step_to_func_result(step, q0, q_1, q_2) - q_3 - Md5::K[step];
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

    // length of specified states should be 2 for q9 verification, specifically q10 and q11
    // while they must follow q9 tunnel def on them
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
    verify_h.cal_range(5, 23);
    Word old_q24 = verify_h.output_of(23);
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
        std::cout << "\n>>> verifying current q9:" << cur_q4 << "\n";
        Word q4_changed = cur_q4 ^ old_q4;
        if (q4_changed & ~search_bit_constraint != 0) {
            std::cout << "Current q9 " << cur_q4 << " violates tunnel constraint on q10 and q11, continue\n";
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
            throw "q12 verification fails, something goes wrong";
        }

        // since modification on x4 may violate 32th bit suff cond for q24,
        // need to further verify q24
        verify_h.cal_range(7, 24);
        Word cur_q24 = verify_h.output_of(23);
        if (cur_q24 == old_q24) {
            std::cout << "Lucky! new q24 equals to old q24 and have value " << cur_q24 << ";\n";
            eq_case += 1;
        } else {
            std::cout << "Opps unlucky, current q24 " << cur_q24 << " != old q24 " << old_q24 << "\n";
            ne_case += 1;
        }

        std::cout << "passed in q4 cases to check: " << ne_case + eq_case << ";\n";
        std::cout << "32th bit suff cond pass rate: " << eq_case / (ne_case + eq_case) << ";\n";
    };
    return;
}

}  // namespace Tunnel