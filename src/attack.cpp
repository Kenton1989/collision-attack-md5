#include "attack.hpp"

#include <cstdio>
#include <tuple>

#include "util.hpp"

namespace Attack {

inline Words add_diff(const Words w, const Word diff[]) {
    Words res = w;
    for (size_t i = 0; i < res.size(); ++i) {
        res[i] = w[i] + diff[i];
    }
    return res;
}

inline bool assert_diff(const Words w1, const Words w2, const Word diff[]) {
    for (size_t i = 0; i < w1.size(); ++i) {
        if (w2[i] - w1[i] != diff[i]) {
            return false;
        }
    }
    return true;
}

inline Words hash_blk(const Words &blk, const Words &iv = Md5::IV) {
    return Md5::Md5BlockHasher(iv, blk).cal_all();
}

std::pair<Words, Words> find_first_block(const Words &iv);
std::pair<Words, Words> find_second_block(const Words &h1a, const Words &h1b);

std::pair<Words, Words> collision(const Words &iv) {
    using std::tie;

    Words block0a, block0b, h1a, h1b, block1a, block1b;

    printf("searching 1st block\n");
    tie(block0a, block0b) = find_first_block(iv);
    printf("found 1st block\n");
    prtln_h(block0a);
    prtln_h(block0b);

    Md5::Md5BlockHasher hasher(iv, block0a);
    h1a = hasher.cal_all();

    hasher.set_msg(block0b);
    h1b = hasher.cal_all();

    printf("searching 2nd block\n");
    tie(block1a, block1b) = find_second_block(h1a, h1b);
    printf("found 2nd block\n");
    prtln_h(block1a);
    prtln_h(block1b);

    block0a.insert(block0a.end(), block1a.begin(), block1a.end());
    block0b.insert(block0b.end(), block1b.begin(), block1b.end());

    return std::make_pair(block0a, block0b);
}

std::pair<Words, Words> find_first_block(const Words &iv) {
    long long cnt = 0;
    Md5::Md5BlockHasher hasher(iv, rand_words(Md5::BLOCK_WORDS));
    while (true) {
        ++cnt;
        if (cnt % 1000000 == 0) {
            printf("searching %dth candidate blocks\n", cnt);
        }
        hasher.set_msg(rand_words(Md5::BLOCK_WORDS));
        apply_simple_modification(hasher, SIMPLE_CONST_COND_0, SIMPLE_ADJ_COND_0);

        Words h1a = hasher.cal_to_end(16);

        Words blk0b = add_diff(hasher.get_msg(), M0_DIFF);
        Words h1b = hash_blk(blk0b, iv);

        if (!assert_diff(h1a, h1b, H1_DIFF)) continue;

        return std::make_pair(hasher.get_msg(), blk0b);
    }
}

std::pair<Words, Words> find_second_block(const Words &h1a, const Words &h1b) {
    long long cnt = 0;
    while (true) {
        ++cnt;
        if (cnt % 1000000 == 0) {
            printf("searching %dth candidate blocks\n", cnt);
        }
        Md5::Md5BlockHasher hasher(h1a, rand_words(Md5::BLOCK_WORDS));
        apply_simple_modification(hasher, SIMPLE_CONST_COND_1, SIMPLE_ADJ_COND_1);
        Words h2a = hasher.cal_to_end(16);

        Words blk1b = add_diff(hasher.get_msg(), M1_DIFF);
        Words h2b = hash_blk(blk1b, h1b);

        if (!assert_diff(h2a, h2b, H2_DIFF)) continue;

        return std::make_pair(hasher.get_msg(), blk1b);
    }
}

void apply_simple_modification(Md5::Md5BlockHasher &hasher,
                               const PartialWord const_cond[], const PartialWord adj_cond[]) {
    for (int step = 0; step < 16; ++step) {
        hasher.cal_step(step);
        Word val = hasher.output_of(step);
        Word in[] = {hasher.output_of(step - 4),
                     hasher.output_of(step - 1),
                     hasher.output_of(step - 2),
                     hasher.output_of(step - 3)};
        Word adj_val = in[1];
        const PartialWord const_c = const_cond[step];
        const PartialWord adj_c = adj_cond[step];

        Word new_val = val;
        new_val = (new_val & ~const_c.mask) | const_c.value;
        new_val = (new_val & ~adj_c.mask) | (adj_c.mask & (adj_c.value ^ adj_val));

        Word new_msg =
            Md5::r_rotate(new_val - adj_val, Md5::S[step]) - in[0] - Md5::F(in[1], in[2], in[3]) - Md5::K[step];

        hasher.set_output_of(step, new_val);
        hasher.set_msg_word(step, new_msg);
    }
}

// complex modification for each step (0-index)
void mod_msg0_step16_17(Md5::Md5BlockHasher &hasher) {
    // a5,4 = b4,4, a5,16 = b4,16, a5,18 = 0, a5,32 = b4,32
    // a5, 31 = b4, 31 + 1 = 1
}
void mod_msg0_step18(Md5::Md5BlockHasher &hasher);
void mod_msg0_step19(Md5::Md5BlockHasher &hasher);
void mod_msg1_step14_15(Md5::Md5BlockHasher &hasher);
void mod_msg1_step16(Md5::Md5BlockHasher &hasher);
void mod_msg1_step17(Md5::Md5BlockHasher &hasher);
void mod_msg1_step18(Md5::Md5BlockHasher &hasher);
void mod_msg1_step19(Md5::Md5BlockHasher &hasher);

}  // namespace Attack
