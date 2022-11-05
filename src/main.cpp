#include <cstdio>

#include "attack.hpp"
#include "md5.hpp"
#include "tunnel_eval_test.hpp"
#include "util.hpp"

int main() {
    // char s[] = "";
    char s[] = "The quick brown fox jumps over the lazy dog";
    Md5::Bytes hash = Md5::hash_string(s);
    printf("%s\n", s);
    prtln_h(hash);

    Words msg = rand_words(16);
    // evaluate_q4(msg);
    // evaluate_q9(msg);

    auto [m0, m1] = Attack::collision();
    prtln_h(m0);

    return 0;
}
