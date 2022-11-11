#include "attack.hpp"

#include <cstdio>
#include <ctime>
#include <iostream>

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

bool check_cond(Word src, Word adj_val, const PartialWord &const_cond, const PartialWord &adj_cond) {
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
    std::cout << "used time (s): " << (blk0_end_clk - blk0_beg_clk) / double(CLOCKS_PER_SEC) << std::endl;
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
    std::cout << "used time (s): " << (blk1_end_clk - blk1_beg_clk) / double(CLOCKS_PER_SEC) << std::endl;
    prtln_h(block1a);
    prtln_h(block1b);

    block0a.insert(block0a.end(), block1a.begin(), block1a.end());
    block0b.insert(block0b.end(), block1b.begin(), block1b.end());

    return std::make_pair(block0a, block0b);
}

bool apply_complex_modification_blk_0(Md5::Md5BlockHasher &hasher);

std::pair<Words, Words> find_first_block(const Words &iv) {
    // return a existing collision

    long long cnt = 0;
    long long success_cnt = 0;
    Md5::Md5BlockHasher hasher0(iv, Md5::M1_PAIR.first);
    Md5::Md5BlockHasher hasher1(iv, Md5::M1_PAIR.second);
    hasher0.cal_all();
    hasher1.cal_all();
    if (assert_diff(hasher0.final_output(), hasher1.final_output(), H1_DIFF)) {
        return Md5::M1_PAIR;
    } else {
        throw "something wrong";
    }
    clock_t beg_clk = clock();
    while (true) {
        ++cnt;
        if (cnt % 10000000 == 0) {
            std::cout << "searching " << cnt << "th candidate, "
                      << success_cnt << " success modification, "
                      << "spent " << double(clock() - beg_clk) / CLOCKS_PER_SEC << "s" << std::endl;
        }
        hasher0.set_msg(rand_words(Md5::BLOCK_WORDS));
        apply_simple_modification(hasher0, CONST_COND_0, ADJ_COND_0);
        bool success = apply_complex_modification_blk_0(hasher0);
        if (!success) {
            continue;
        }
        ++success_cnt;
        const int OK_STEPS = 20;

        Words h1a = hasher0.cal_to_end(OK_STEPS);

        hasher1.set_msg(add_diff(hasher0.get_msg(), M0_DIFF));
        Words h1b = hasher1.cal_all();

        // for (int i = 0; i < OK_STEPS; ++i) {
        //     bool bad = false;
        //     Word o0 = hasher0.output_of(i);
        //     Word o0true = hasher0.cal_step(i);
        //     if (o0 != o0true) {
        //         prtln_h(o0);
        //         prtln_h(o0true);
        //         printf("result mismatch at step %d of trial %ld\n", i, cnt);
        //         break;
        //     }
        //     if (!check_cond(o0, hasher0.output_of(i-1), CONST_COND_0[i], ADJ_COND_0[i])) {
        //         Word last = hasher0.output_of(i-1);
        //         prtln_h(o0);
        //         prtln_h(last);
        //         prtln_h(CONST_COND_0[i].mask);
        //         prtln_h(CONST_COND_0[i].value);
        //         prtln_h(ADJ_COND_1[i].mask);
        //         prtln_h(ADJ_COND_1[i].value);
        //         printf("condition not satisfied at step %d of trial %ld\n", i, cnt);
        //         break;
        //     }
        //     Word o1 = hasher1.output_of(i);
        //     Word diff = o1 - o0;
        //     Word true_diff = OUTPUT_DIFF_0[i];
        //     if (true_diff != diff) {
        //         prtln_h(hasher0.full_input(i));
        //         prtln_h(hasher1.full_input(i));
        //         int msg_i = Md5::msg_idx_of_step(i);
        //         printf("Msg index: %d\n", msg_i);
        //         prtln_h(hasher0.get_msg()[msg_i]);
        //         prtln_h(hasher1.get_msg()[msg_i]);
        //         prtln_h(o0);
        //         prtln_h(o1);
        //         prtln_h(diff);
        //         prtln_h(true_diff);
        //         printf("diff not satisfied at step %d of trial %ld\n", i, cnt);
        //         break;
        //     }
        // }
        if (!assert_diff(h1a, h1b, H1_DIFF)) continue;

        return std::make_pair(hasher0.get_msg(), hasher1.get_msg());
    }
}

bool apply_complex_modification_blk_1(Md5::Md5BlockHasher &hasher);

std::pair<Words, Words> find_second_block(const Words &h1a, const Words &h1b) {
    if (!assert_diff(h1a, h1b, H1_DIFF)) {
        throw "bad input";
    }
    long long cnt = 0;
    long long success_cnt = 0;
    Md5::Md5BlockHasher hasher0(h1a, rand_words(Md5::BLOCK_WORDS));
    Md5::Md5BlockHasher hasher1(h1b, rand_words(Md5::BLOCK_WORDS));
    clock_t beg_clk = clock();
    while (true) {
        ++cnt;
        if (cnt % 10000000 == 0) {
            std::cout << "searching " << cnt << "th candidate, "
                      << success_cnt << " success modification, "
                      << "spent " << double(clock() - beg_clk) / CLOCKS_PER_SEC << "s" << std::endl;
        }
        hasher0.set_msg(rand_words(Md5::BLOCK_WORDS));
        apply_simple_modification(hasher0, CONST_COND_1, ADJ_COND_1);
        // bool success = apply_complex_modification_blk_1(hasher0);
        // if (!success) {
        //     continue;
        // }
        ++success_cnt;
        const int OK_STEPS = 16;

        Words h2a = hasher0.cal_to_end(OK_STEPS);

        hasher1.set_msg(add_diff(hasher0.get_msg(), M1_DIFF));
        Words h2b = hasher1.cal_all();

        for (int i = 0; i < OK_STEPS; ++i) {
            Word o0 = hasher0.output_of(i);
            Word o0true = hasher0.cal_step(i);
            if (o0 != o0true) {
                prtln_h(o0);
                prtln_h(o0true);
                printf("result mismatch at step %d of trial %ld\n", i, cnt);
                throw "bad result";
            }
            if (!check_cond(o0, hasher0.output_of(i - 1), CONST_COND_1[i], ADJ_COND_1[i])) {
                Word last = hasher0.output_of(i - 1);
                prtln_h(o0);
                prtln_h(last);
                prtln_h(CONST_COND_1[i].mask);
                prtln_h(CONST_COND_1[i].value);
                prtln_h(ADJ_COND_1[i].mask);
                prtln_h(ADJ_COND_1[i].value);
                printf("condition not satisfied at step %d of trial %ld\n", i, cnt);
                throw "bad result";
            }
            if (BAD_EPS_1[i].mask &&
                (hasher0.epsilon(i) & BAD_EPS_1[i].mask) == BAD_EPS_1[i].value) {
                prtln_h(hasher0.epsilon(i));
                prtln_h(BAD_EPS_1[i].mask);
                prtln_h(BAD_EPS_1[i].value);
                printf("epsilon is bad at step %d of trial %ld\n", i, cnt);
                throw "bad result";
            }
            Word o1 = hasher1.output_of(i);
            Word diff = o1 - o0;
            Word true_diff = OUTPUT_DIFF_1[i];
            if (true_diff != diff) {
                int msg_i = Md5::msg_idx_of_step(i);
                printf("Msg index: %d\n", msg_i);
                prtln_h(hasher0.get_msg()[msg_i]);
                prtln_h(hasher1.get_msg()[msg_i]);
                prtln_h(hasher0.full_input(i));
                prtln_h(hasher1.full_input(i));
                prtln_h(o0);
                prtln_h(o1);
                prtln_h(diff);
                prtln_h(true_diff);
                printf("diff not satisfied at step %d of trial %ld\n", i, cnt);
                throw "bad result";
            }
        }
        if (!assert_diff(h2a, h2b, H2_DIFF)) continue;

        return std::make_pair(hasher0.get_msg(), hasher1.get_msg());
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

//  complex modification for 1st block
bool apply_complex_modification_blk_0(Md5::Md5BlockHasher &hasher) {
    using Md5::step_of;
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
    // step (c) of paper
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
        b4 = new_b4;
        hasher.set_output_of(b4step, new_b4);
        hasher.set_output_of(d5step, d5);
        hasher.set_msg_to_ensure_step(b4step);
    }
    // step (d) of paper
    {
        Word new_b3, new_m11, new_c5;
        constexpr int c3step = step_of.b(3);
        constexpr int b3step = step_of.b(3);
        constexpr int c5step = step_of.c(5);
        constexpr PartialWord c5const = CONST_COND_0[c5step];
        constexpr Word eps19_mask = p2sum(3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17);
        bool good_c5 = false;
        for (SubmaskIter it(p2sum(3, 4, 5, 6, 20, 21, 22, 23)); it.has_next(); it.next()) {
            Word mask = it.val();
            new_b3 = b3 ^ mask;
            new_m11 = cal_msg(b3step, b2, new_b3, c3, d3, a3);

            Word eps19 = c4 + Md5::G(d5, a5, b4) + new_m11 + Md5::K[c5step];
            // if all bits under the mask is 1
            if ((eps19_mask & eps19) == eps19_mask) {
                continue;
            }

            new_c5 = d5 + l_rotate(eps19, Md5::S[c5step]);
            if (!check_cond(new_c5, d5, c5const, ADJ_COND_0[c5step])) {
                continue;
            }
            good_c5 = true;
            break;
        }
        if (!good_c5) {
            return false;
        }
        b3 = new_b3;
        c5 = new_c5;
        hasher.set_output_of(c5step, c5);
        hasher.set_output_of(b3step, b3);
        hasher.set_msg_to_ensure_step(b3step);
        hasher.set_msg_to_ensure_step(b3step + 1);
        hasher.set_msg_to_ensure_step(b3step + 2);
        hasher.set_msg_to_ensure_step(b3step + 3);
        hasher.set_msg_to_ensure_step(b3step + 4);
    }
    // step (e) of paper
    {
        constexpr Word eps20_mask = p2sum(29, 30, 31);
        constexpr int b5step = step_of.b(5);
        while (true) {
            b5 = enforce_cond(rand_word(), c5, CONST_COND_0[b5step], ADJ_COND_0[b5step]);
            Word new_m0 = cal_msg(b5step, b4, b5, c5, d5, a5);
            Word eps20 = b4 + Md5::G(c5, d5, a5) + new_m0 + Md5::K[b5step];
            // if bits under the mask is not all 0
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
    return true;
}
//  complex modification for 2nd block
bool apply_complex_modification_blk_1(Md5::Md5BlockHasher &hasher) {
    using Md5::step_of;
    Word d1 = hasher.output_of(step_of.d(1));
    Word c1 = hasher.output_of(step_of.c(1));
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
    while (true) {
        // step (c) of paper
        {
            constexpr int c4step = step_of.c(4);
            constexpr int b4step = step_of.b(4);
            c4 = enforce_cond(rand_word(), d4, CONST_COND_1[c4step], ADJ_COND_1[c4step]);
            b4 = enforce_cond(rand_word(), c4, CONST_COND_1[b4step], ADJ_COND_1[b4step]);
            hasher.set_output_of(c4step, c4);
            hasher.set_output_of(b4step, b4);
            hasher.set_msg_to_ensure_step(c4step);
            hasher.set_msg_to_ensure_step(b4step);
        }
        // step (d) of paper
        {
            constexpr int d1step = step_of.d(1);
            constexpr int c1step = step_of.c(1);
            constexpr int b4step = step_of.b(4);
            constexpr int a5step = step_of.a(5);
            constexpr Word eps17_mask = p2sum(24, 25, 26);
            bool ok = false;
            Word old_d1 = d1, old_b4 = b4;
            for (SubmaskIter d1it(p2sum(3, 4)); d1it.has_next(); d1it.next()) {
                d1 = old_d1 ^ d1it.val();
                hasher.set_output_of(d1step, d1);
                hasher.set_msg_to_ensure_step(d1step);
                Word m1 = hasher.get_msg()[1];
                for (SubmaskIter b4it(p2sum(20, 21, 22, 23)); b4it.has_next(); b4it.next()) {
                    b4 = old_b4 ^ b4it.val();
                    Word eps17 = a4 + Md5::G(b4, c4, d4) + Md5::K[a5step] + m1;
                    // all bits under the mask are 1
                    if ((eps17_mask & eps17) == eps17_mask) {
                        continue;
                    }
                    hasher.set_output_of(b4step, b4);
                    a5 = b4 + l_rotate(eps17, Md5::S[a5step]);
                    hasher.set_output_of(a5step, a5);

                    if (!bit_eq(a5, b4, 31)) {
                        continue;
                    }
                    ok = true;
                    hasher.set_msg_to_ensure_step(b4step);
                    break;
                }
                if (ok) {
                    break;
                }
            }
            if (!ok) {
                return false;
            }
            // second searching of step (d)
            ok = false;
            old_d1 = d1;
            for (SubmaskIter d1it(p2sum(10, 22, 24)); d1it.has_next(); d1it.next()) {
                d1 = old_d1 ^ d1it.val();
                hasher.set_output_of(d1step, d1);
                hasher.set_msg_to_ensure_step(d1step);
                Word m1 = hasher.get_msg()[1];
                Word eps17 = a4 + Md5::G(b4, c4, d4) + Md5::K[a5step] + m1;
                // all bits under the mask are 1
                if ((eps17_mask & eps17) == eps17_mask) {
                    continue;
                }
                a5 = b4 + l_rotate(eps17, Md5::S[a5step]);
                hasher.set_output_of(a5step, a5);
                if (!check_cond(a5, b4, CONST_COND_1[a5step], ADJ_COND_1[a5step])) {
                    continue;
                }
                ok = true;
                break;
            }
            if (!ok) {
                return false;
            }
            c1 = enforce_cond(c1, d1, CONST_COND_1[c1step], ADJ_COND_1[c1step]);
            hasher.set_output_of(c1step, c1);
            hasher.set_msg_to_ensure_step(2);
            hasher.set_msg_to_ensure_step(3);
            hasher.set_msg_to_ensure_step(4);
            hasher.set_msg_to_ensure_step(5);
            hasher.set_msg_to_ensure_step(6);
        }
        // step (e) of paper
        {
            constexpr int d5step = step_of.d(5);
            d5 = hasher.cal_step(d5step);
            if (!(bit_eq(d5, -1, 17) & bit_eq(d5, a5, 31))) {
                continue;
            }
            if (!bit_eq(d5, a5, 29)) {
                constexpr int c2step = step_of.c(2);
                Word c2 = hasher.output_of(c2step);
                c2 ^= p2(5);
                hasher.set_output_of(c2step, c2);
                hasher.set_msg_to_ensure_step(c2step);
                hasher.set_msg_to_ensure_step(c2step + 1);
                hasher.set_msg_to_ensure_step(c2step + 2);
                hasher.set_msg_to_ensure_step(c2step + 3);
                hasher.set_msg_to_ensure_step(c2step + 4);
                d5 = hasher.cal_step(d5step);
                if (!(bit_eq(d5, -1, 17) & bit_eq(d5, a5, 31))) {
                    continue;
                }
            }
        }
        // step (f) of paper
        {
            Word new_b3, new_c5, new_m11;
            constexpr int b3step = step_of.b(3);
            constexpr int c5step = step_of.c(5);
            constexpr Word eps19_mask = p2sum(3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17);
            constexpr PartialWord c5const = CONST_COND_1[c5step];
            bool good_c5 = false;
            for (SubmaskIter it(p2sum(3, 4, 5, 6, 20, 21, 22, 23)); it.has_next(); it.next()) {
                Word mask = it.val();
                new_b3 = b3 ^ mask;
                new_m11 = cal_msg(b3step, b2, new_b3, c3, d3, a3);

                Word eps19 = c4 + Md5::G(d5, a5, b4) + new_m11 + Md5::K[c5step];
                // if all bits under the mask is 1
                if ((eps19_mask & eps19) == eps19_mask) {
                    continue;
                }

                new_c5 = d5 + l_rotate(eps19, Md5::S[c5step]);
                if (!check_cond(new_c5, d5, c5const, ADJ_COND_0[c5step])) {
                    continue;
                }
                good_c5 = true;
                break;
            }
            if (!good_c5) {
                return false;
            }
            b3 = new_b3;
            c5 = new_c5;
            hasher.set_output_of(c5step, c5);
            hasher.set_output_of(b3step, b3);
            hasher.set_msg_to_ensure_step(b3step);
            hasher.set_msg_to_ensure_step(b3step + 1);
            hasher.set_msg_to_ensure_step(b3step + 2);
            hasher.set_msg_to_ensure_step(b3step + 3);
            hasher.set_msg_to_ensure_step(b3step + 4);
        }
        // step (g) of paper
        {
            constexpr Word eps20_mask = p2sum(29, 30, 31);
            constexpr int b5step = step_of.b(5);
            Word m0 = hasher.get_msg()[0];
            Word eps20 = b4 + Md5::G(c5, d5, a5) + m0 + Md5::K[b5step];
            // if all bits under the mask are 0
            if (!(eps20 & eps20_mask)) {
                continue;
            }
            b5 = hasher.cal_step(b5step);
            if (!check_cond(b5, c5, CONST_COND_1[b5step], ADJ_COND_1[b5step])) {
                continue;
            }
        }
        break;
    }
    return true;
}
}  // namespace Attack
