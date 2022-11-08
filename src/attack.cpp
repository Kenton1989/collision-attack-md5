#include "attack.hpp"

#include <cstdio>
#include <ctime>
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

inline Word enforce_cond(Word src, Word adj_val, const PartialWord &const_cond, const PartialWord &adj_cond) {
    Word new_val = src;
    new_val = (new_val & ~const_cond.mask) | const_cond.value;
    new_val = (new_val & ~adj_cond.mask) | (adj_cond.mask & (adj_val ^ ~adj_cond.value));
    return new_val;
}

inline bool check_cond(Word src, Word adj_val, const PartialWord &const_cond, const PartialWord &adj_cond) {
    return ((src & const_cond.mask) == const_cond.value) &&
           ((src & adj_cond.mask) == (adj_cond.mask & (adj_val ^ ~adj_cond.value)));
}

// calculate the value message given all the other state
inline Word cal_msg(int step, Word a0, Word a, Word b, Word c, Word d) {
    return r_rotate(a - b, Md5::S[step]) - a0 - Md5::step_to_func_result(step, b, c, d) - Md5::K[step];
}

std::pair<Words, Words> find_first_block(const Words &iv);
std::pair<Words, Words> find_second_block(const Words &h1a, const Words &h1b);

std::pair<Words, Words> collision(const Words &iv) {
    using std::tie;
    Words block0a, block0b, h1a, h1b, block1a, block1b;

    printf("searching 1st block\n");
    clock_t blk0_beg_clk = clock();
    tie(block0a, block0b) = find_first_block(iv);
    clock_t blk0_end_clk = clock();
    printf("found 1st block\n");
    printf("used time: %fs", (blk0_end_clk - blk0_beg_clk) / double(CLOCKS_PER_SEC));
    prtln_h(block0a);
    prtln_h(block0b);

    Md5::Md5BlockHasher hasher(iv, block0a);
    h1a = hasher.cal_all();

    hasher.set_msg(block0b);
    h1b = hasher.cal_all();

    printf("searching 2nd block\n");
    clock_t blk1_beg_clk = clock();
    tie(block1a, block1b) = find_second_block(h1a, h1b);
    clock_t blk1_end_clk = clock();
    printf("found 2nd block\n");
    printf("used time: %fs", (blk1_end_clk - blk1_beg_clk) / double(CLOCKS_PER_SEC));
    prtln_h(block1a);
    prtln_h(block1b);

    block0a.insert(block0a.end(), block1a.begin(), block1a.end());
    block0b.insert(block0b.end(), block1b.begin(), block1b.end());

    return std::make_pair(block0a, block0b);
}

void apply_complex_modification_blk_0(Md5::Md5BlockHasher &hasher);

std::pair<Words, Words> find_first_block(const Words &iv) {
    long long cnt = 0;
    Md5::Md5BlockHasher hasher0(iv, rand_words(Md5::BLOCK_WORDS));
    Md5::Md5BlockHasher hasher1(iv, rand_words(Md5::BLOCK_WORDS));
    while (true) {
        ++cnt;
        if (cnt % 20000000 == 0) {
            printf("searching %ldth candidate blocks\n", cnt);
        }
        hasher0.set_msg(rand_words(Md5::BLOCK_WORDS));
        apply_simple_modification(hasher0, CONST_COND_0, ADJ_COND_0);
        apply_complex_modification_blk_0(hasher0);
        const int OK_STEPS = 20;

        Words h1a = hasher0.cal_to_end(OK_STEPS);

        hasher1.set_msg(add_diff(hasher0.get_msg(), M0_DIFF));
        Words h1b = hasher1.cal_all();

        for (int i = 0; i < OK_STEPS; ++i) {
            bool bad = false;
            Word o0 = hasher0.output_of(i);
            Word o0true = hasher1.cal_step(i);
            if (o0 != o0true) {
                prtln_h(o0);
                prtln_h(o0true);
                printf("result mismatch at step %d\n", i);
                bad = true;
            }
            Word o1 = hasher1.output_of(i);
            Word diff = o1 - o0;
            Word true_diff = OUTPUT_DIFF_0[i];
            if (true_diff != diff) {
                prtln_h(hasher0.full_input(i));
                prtln_h(hasher1.full_input(i));
                prtln_h(o0);
                prtln_h(o1);
                prtln_h(diff);
                prtln_h(true_diff);
                printf("bad output at step %d\n", i);
                bad = true;
            }
            if (bad) {
                throw "bad result";
            }
        }
        if (!assert_diff(h1a, h1b, H1_DIFF)) continue;

        return std::make_pair(hasher0.get_msg(), hasher1.get_msg());
    }
}

std::pair<Words, Words> find_second_block(const Words &h1a, const Words &h1b) {
    long long cnt = 0;
    while (true) {
        ++cnt;
        if (cnt % 20000000 == 0) {
            printf("searching %ldth candidate blocks\n", cnt);
        }
        Md5::Md5BlockHasher hasher(h1a, rand_words(Md5::BLOCK_WORDS));
        apply_simple_modification(hasher, CONST_COND_1, ADJ_COND_1);
        Words h2a = hasher.cal_to_end(16);

        Words blk1b = add_diff(hasher.get_msg(), M1_DIFF);
        Words h2b = hash_blk(blk1b, h1b);

        if (!assert_diff(h2a, h2b, H2_DIFF)) continue;

        return std::make_pair(hasher.get_msg(), blk1b);
    }
}

void apply_simple_modification(Md5::Md5BlockHasher &hasher,
                               const PartialWord const_conds[], const PartialWord adj_conds[]) {
    for (int step = 0; step < 16; ++step) {
        hasher.cal_step(step);
        Word val = hasher.output_of(step);
        Word in[] = {hasher.output_of(step - 4),
                     hasher.output_of(step - 1),
                     hasher.output_of(step - 2),
                     hasher.output_of(step - 3)};
        Word adj_val = in[1];

        Word new_val = enforce_cond(val, adj_val, const_conds[step], adj_conds[step]);

        Word new_msg = cal_msg(step, in[0], new_val, in[1], in[2], in[3]);

        hasher.set_output_of(step, new_val);
        hasher.set_msg_word(step, new_msg);
    }
}

//  complex modification for each block
void apply_complex_modification_blk_0(Md5::Md5BlockHasher &hasher) {
    using Md5::step_of;
    // step (c) of paper
    Word b2 = hasher.output_of(step_of.b(2));
    Word a3 = hasher.output_of(step_of.a(3));
    Word d3 = hasher.output_of(step_of.d(3));
    Word c3 = hasher.output_of(step_of.c(3));
    Word b3 = hasher.output_of(step_of.b(3));
    Word a4 = hasher.output_of(step_of.a(4));
    Word d4 = hasher.output_of(step_of.d(4));
    Word c4 = hasher.output_of(step_of.c(4));
    Word b4 = hasher.output_of(step_of.b(4));
    Word a5, d5, c5, b5;
    {
        constexpr int a5step = step_of.a(5);
        a5 = enforce_cond(rand_word(), b4, CONST_COND_0[a5step], ADJ_COND_0[a5step]);
        hasher.set_output_of(a5step, a5);

        Word new_b4;
        constexpr int d5step = step_of.d(5);
        constexpr int d5msgi = Md5::msg_idx_of_step(d5step);
        Word d5msg = hasher.get_msg()[d5msgi];
        for (SubmaskIter it(p2sum(8, 20, 22)); it.has_next(); it.next()) {
            Word mask = it.val();
            new_b4 = b4 ^ mask;
            d5 = Md5::perform_one_step(d5step, d5msg, d4, a5, new_b4, c4);
            if (check_cond(d5, a5, CONST_COND_0[d5step], ADJ_COND_0[d5step]))
                break;
        }
        constexpr int b4step = step_of.b(4);
        Word new_m15 = cal_msg(b4step, b3, new_b4, c4, d4, a4);
        b4 = new_b4;
        hasher.set_output_of(b4step, new_b4);
        hasher.set_output_of(d5step, d5);
        hasher.set_msg_word(15, new_m15);
    }
    // step (d) of paper
    {
        Word new_b3, new_m11, new_c5;
        constexpr int b3step = step_of.b(3);
        constexpr int c5step = step_of.c(5);
        constexpr Word eps19_mask = p2sum(3, 4, 5, 6, 7);
        for (SubmaskIter it(p2sum(3, 4, 5, 6, 20, 21, 22, 23)); it.has_next(); it.next()) {
            Word mask = it.val();
            new_b3 = b3 ^ mask;
            new_m11 = cal_msg(b3step, b2, new_b3, c3, d3, a3);

            Word eps19 = c4 + Md5::G(d5, a5, b4) + new_m11 + Md5::K[c5step];
            // if all bits under the mask is 1
            if (!(eps19_mask & ~eps19)) {
                continue;
            }

            new_c5 = d5 + l_rotate(eps19, Md5::S[c5step]);
            if (!check_cond(new_c5, d5, CONST_COND_0[c5step], ADJ_COND_0[c5step])) {
                continue;
            }
            break;
        }
        b3 = new_b3;
        c5 = new_c5;
        hasher.set_msg_word(11, new_m11);
        hasher.set_output_of(b3step, b3);
        hasher.set_output_of(c5step, c5);
        hasher.set_msg_to_ensure_step(12);
        hasher.set_msg_to_ensure_step(13);
        hasher.set_msg_to_ensure_step(14);
        hasher.set_msg_to_ensure_step(15);
    }
    // step (e) of paper
    {
        constexpr Word eps20_mask = p2sum(29, 30, 31);
        constexpr int b5step = step_of.b(5);
        while (true) {
            b5 = enforce_cond(rand_word(), c5, CONST_COND_0[b5step], ADJ_COND_0[b5step]);
            Word new_m0 = cal_msg(b5step, b4, b5, c5, d5, a5);
            Word eps20 = b4 + Md5::G(c5, d5, a5) + new_m0 + Md5::K[b5step];
            // if any bits under the mask is 1
            if (eps20_mask & eps20) {
                break;
            }
        }

        constexpr int a5step = step_of.a(5);
        hasher.set_output_of(b5step, b5);
        hasher.set_msg_to_ensure_step(a5step);
        hasher.set_msg_to_ensure_step(b5step);
        hasher.cal_range(0, 1);
        hasher.set_msg_to_ensure_step(2);
        hasher.set_msg_to_ensure_step(3);
        hasher.set_msg_to_ensure_step(4);
        hasher.set_msg_to_ensure_step(5);
    }
}

}  // namespace Attack
