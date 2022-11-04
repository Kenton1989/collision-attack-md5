#ifndef COLLISION_ATTACK_TUNNEL_HPP_
#define COLLISION_ATTACK_TUNNEL_HPP_

#include <stddef.h>

#include <string>
#include <vector>

#include "common.hpp"
#include "constants.hpp"
#include "md5.hpp"

namespace Tunnel {

struct Modification {
    int index;
    Word new_val;
};

struct Solution {
    int pov;  // first step (0 index) that need to recalculate
    std::vector<Modification> modifications;
};

// searched_bits_mask & ~(q10 ^ 0) & ~(q11 ^ -1)
std::vector<Solution> q9_solve(const Md5::Md5BlockHasher &h, Word searched_bits_mask);

// searched_bits_mask & ~(q5 ^ 0) & ~(q6 ^ -1)
std::vector<Solution> q4_solve(const Md5::Md5BlockHasher &h, Word searched_bits_mask);

using Solver = std::vector<Solution> (*)(const Md5::Md5BlockHasher &h, Word searched_bits_mask);

const std::vector<Solver> solvers = {q9_solve, q4_solve};

}  // namespace Tunnel

#endif