#include <cstdio>
#include <fstream>
#include <iostream>
#include <tuple>

#include "attack.hpp"
#include "md5.hpp"
#include "tunnel_eval_test.hpp"
#include "util.hpp"
void md5_quick_example() {
    // char s[] = "";
    char s[] = "The quick brown fox jumps over the lazy dog";
    Md5::Bytes hash = Md5::hash_string(s);
    printf("%s\n", s);
    prtln_h(hash);
    fflush(stdout);
}

void print_sufficient_conditions() {
    printf("block 1 suff condi:\n");
    for (int i = 0; i < 20; ++i) {
        printf("%2d ", i + 1);
        prtln_cond(Attack::CONST_COND_0[i], Attack::ADJ_COND_0[i]);
    }

    printf("block 2 suff condi:\n");
    for (int i = 0; i < 20; ++i) {
        printf("%2d ", i + 1);
        prtln_cond(Attack::CONST_COND_1[i], Attack::ADJ_COND_1[i]);
    }
}

void hash_input_without_padding() {
    std::string str;
    const auto CIN_EOF = decltype(std::cin)::traits_type::eof();
    while (true) {
        char ch = std::cin.get();
        if (ch == CIN_EOF) {
            break;
        }
        str.push_back(ch);
    }
    if (str.size() % 64 != 0) {
        throw "bad input";
    }
    Words input = Md5::encode_words(Md5::encode_bytes(str));
    Words iv = Md5::IV;
    for (size_t i = 0; i < input.size(); i += 16) {
        Words blk(input.begin() + i, input.begin() + i + 16);
        Md5::Md5BlockHasher h(iv, blk);
        iv = h.cal_all();
    }
    prtln_h(iv);
}

void perform_md5_attack() {
    printf("seed: %ld\n", RAND_SEED);
    Words iv = Md5::IV;
    // iv = {0xb9c93963, 0x48775557, 0xd2f539f7, 0x377aa9cc};
    Words m0, m1;
    std::tie(m0, m1) = Attack::collision(iv);
    prtln_h(m0);
    prtln_h(m1);
}

void output_sample_file() {
    using namespace std;

    Words m0 = {
        0x0f06cc1f,
        0xb80d3846,
        0xd5e06941,
        0x544cf76f,
        0x9f420444,
        0xe6f86c03,
        0xd9a385d5,
        0x940f7ab5,
        0x0534adc5,
        0xc1b409fa,
        0x864affeb,
        0x55e90724,
        0x694d02ef,
        0x068b8fec,
        0x18b4a804,
        0x10e12ad2,
        0xa1f4eac5,
        0xfed07b84,
        0x8a5001f5,
        0xd2948d9f,
        0x241b1383,
        0x2d0014ca,
        0xe7358d12,
        0xaf1d3e50,
        0xeb372825,
        0xc87e4267,
        0x616f84ee,
        0x95aa3478,
        0xddfb10c9,
        0x797baa63,
        0xba227878,
        0x00c1f75c,
    };

    Words m1 = {
        0x0f06cc1f,
        0xb80d3846,
        0xd5e06941,
        0x544cf76f,
        0x1f420444,
        0xe6f86c03,
        0xd9a385d5,
        0x940f7ab5,
        0x0534adc5,
        0xc1b409fa,
        0x864affeb,
        0x55e98724,
        0x694d02ef,
        0x068b8fec,
        0x98b4a804,
        0x10e12ad2,
        0xa1f4eac5,
        0xfed07b84,
        0x8a5001f5,
        0xd2948d9f,
        0xa41b1383,
        0x2d0014ca,
        0xe7358d12,
        0xaf1d3e50,
        0xeb372825,
        0xc87e4267,
        0x616f84ee,
        0x95a9b478,
        0xddfb10c9,
        0x797baa63,
        0x3a227878,
        0x00c1f75c,
    };

    Bytes b0 = Md5::encode_bytes(m0);
    Bytes b1 = Md5::encode_bytes(m1);
    if (Md5::encode_words(b0) != m0) {
        throw "something wrong";
    }
    if (Md5::encode_words(b1) != m1) {
        throw "something wrong";
    }
    string s0 = Md5::encode_string(b0);
    string s1 = Md5::encode_string(b1);

    Bytes h0 = Md5::hash_string(s0);
    Bytes h1 = Md5::hash_string(s1);
    prtln_h(b0);
    prtln_h(h0);

    prtln_h(b1);
    prtln_h(h1);

    fstream o0("m0.bin", ios::binary | ios::out);
    if (!o0.is_open()) {
        throw "file cannot open";
    }
    for (Byte b : b0) {
        o0.put(b);
    }
    o0.close();

    fstream o1("m1.bin", ios::binary | ios::out);
    if (!o1.is_open()) {
        throw "file cannot open";
    }
    for (Byte b : b1) {
        o1.put(b);
    }
    o1.close();
}

int main() {
    md5_quick_example();

    // print_sufficient_conditions();

    // hash_input_without_padding();

    // Words msg = rand_words(16);
    // evaluate_q4(msg);
    // evaluate_q9(msg);

    perform_md5_attack();

    // output_sample_file();

    return 0;
}
