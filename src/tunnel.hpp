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

    Modification(int index, Word new_val) {
        this->index = index;
        this->new_val = new_val;
    }
};

struct Solution {
    int pov;  // first step (0 index) that need to recalculate
    std::vector<Modification> modifications;

    Solution(int pov, std::vector<Modification> mods) {
        this->pov = pov;
        this->modifications = mods;
    }
};

// searched_bits_mask & ~(q10 ^ 0) & ~(q11 ^ -1)
std::vector<Solution> q9_solve(const Md5::Md5BlockHasher &h, Word searched_bits_mask);

// searched_bits_mask & ~(q5 ^ 0) & ~(q6 ^ -1)
std::vector<Solution> q4_solve(const Md5::Md5BlockHasher &h, Word searched_bits_mask);

inline std::vector<Solution> dummy_solver(const Md5::Md5BlockHasher &h, Word searched_bits_mask) {
    Solution dummy1(0, {Modification(0, 0)});
    Solution dummy2(3, {Modification(3, 1919810), Modification(4, 114514)});
    return {dummy1, dummy2};
}

using Solver = std::vector<Solution> (*)(const Md5::Md5BlockHasher &h, Word searched_bits_mask);

const std::vector<Solver> solvers = {dummy_solver, dummy_solver};

}  // namespace Tunnel

#endif