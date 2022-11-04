#include "tunnel.hpp"

#include <string>
#include <vector>

#include "constants.hpp"
#include "md5.hpp"

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
        Word new_x7 = Md5::r_rotate(q8 - q7, 7) - Md5::F(q7, q6, q5) - new_q4 - Md5::K[12];

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

}  // namespace Tunnel