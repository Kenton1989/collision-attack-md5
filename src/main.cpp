#include <cstdio>
#include <iostream>

#include "attack.hpp"
#include "md5.hpp"
#include "tunnel_eval_test.hpp"
#include "util.hpp"

int main() {
    printf("seed: %ld\n", RAND_SEED);
    // char s[] = "";
    char s[] = "The quick brown fox jumps over the lazy dog";
    Md5::Bytes hash = Md5::hash_string(s);
    printf("%s\n", s);
    prtln_h(hash);

    // std::string str;
    // const auto CIN_EOF = decltype(std::cin)::traits_type::eof();
    // while (true) {
    //     char ch = std::cin.get();
    //     if (ch == CIN_EOF) {
    //         break;
    //     }
    //     str.push_back(ch);
    // }
    // if (str.size() % 64 != 0) {
    //     throw "bad input";
    // }
    // Words input = Md5::encode_words(Md5::encode_bytes(str));
    // Words iv = Md5::IV;
    // for (size_t i = 0; i < input.size(); i += 16) {
    //     Words blk(input.begin() + i, input.begin() + i + 16);
    //     Md5::Md5BlockHasher h(iv, blk);
    //     iv = h.cal_all();
    // }
    // prtln_h(iv);
    // return 0;

    // printf("block 1 suff condi:\n");
    // for (int i = 0; i < 20; ++i) {
    //     printf("%2d ", i + 1);
    //     prtln_cond(Attack::CONST_COND_0[i], Attack::ADJ_COND_0[i]);
    // }

    // printf("block 2 suff condi:\n");
    // for (int i = 0; i < 20; ++i) {
    //     printf("%2d ", i + 1);
    //     prtln_cond(Attack::CONST_COND_1[i], Attack::ADJ_COND_1[i]);
    // }

    // Words msg = rand_words(16);
    // evaluate_q4(msg);
    // evaluate_q9(msg);

    // int step = 19;
    // Word in0[] = {0x3a587273, 0xb642a92c, 0x6dbbd766, 0x72756b81};
    // Word in1[] = {0x9a587273, 0x3644a92c, 0xedbbd766, 0xf2756b81};
    // // Msg index: 0
    // Word m00 = 0xc07c3795;
    // Word out0 = 0xa7c4ee8e;
    // Word diff = 0x80080040;
    // Word true_diff = 0x80000000;
    // Word b5 = Md5::perform_one_step(step, m00, in0[0], in0[1], in0[2], in0[3]);
    // printf("%x == %x is %d\n", out0, b5, out0 == b5);
    // Word c4step = Md5::step_of.c(4);
    // Word c4 = 0x98badcfe;
    // Word b4 = in0[0];
    // Word a5 = in0[1];
    // Word d5 = in0[2];
    // Word c5 = in0[3];
    // printf("b4: %d\n", Attack::check_cond(b4, c4, Attack::CONST_COND_0[15],Attack::ADJ_COND_0[15]));
    // printf("a5: %d\n", Attack::check_cond(a5, b4, Attack::CONST_COND_0[16],Attack::ADJ_COND_0[16]));
    // printf("d5: %d\n", Attack::check_cond(d5, a5, Attack::CONST_COND_0[17],Attack::ADJ_COND_0[17]));
    // printf("c5: %d\n", Attack::check_cond(c5, d5, Attack::CONST_COND_0[18],Attack::ADJ_COND_0[18]));
    // printf("b5: %d\n", Attack::check_cond(b5, c5, Attack::CONST_COND_0[19],Attack::ADJ_COND_0[19]));

    Words special_iv = {0xb9c93963, 0x48775557, 0xd2f539f7, 0x377aa9cc};
    auto [m0, m1] = Attack::collision(special_iv);
    prtln_h(m0);
    prtln_h(m1);

    return 0;
}
