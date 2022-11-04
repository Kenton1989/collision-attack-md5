#include "tunnel.hpp"

#include <string>
#include <vector>

#include "constants.hpp"
#include "md5.hpp"

namespace Tunnel {

inline std::vector<Solution> q9_solve(const Md5::Md5BlockHasher &h, Word searched_bits_mask) {
    Word q9 = h.output_of(8);
    Word q10 = h.output_of(9);
    Word q11 = h.output_of(10);

    Word searched_bits = searched_bits_mask & ~(q10 ^ 0) & ~(q11 ^ -1);

    Word inv_searched = ~searched_bits;
    for (Word changed_bits = (inv_searched + 1) & searched_bits;
         changed_bits != 0;
         changed_bits = (changed_bits + inv_searched + 1) & searched_bits) {
        Word new_q9 = q9 ^ changed_bits;
    }
}

}  // namespace Tunnel