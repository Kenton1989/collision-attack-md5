#include <cstdio>

#include "attack.hpp"
#include "md5.hpp"
#include "util.hpp"

int main() {
    // char s[] = "";
    char s[] = "The quick brown fox jumps over the lazy dog";

    Md5::Bytes hash = Md5::hash_string(s);

    printf("%s\n", s);

    prtln_h(hash);

    return 0;
}
