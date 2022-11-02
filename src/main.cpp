#include <cstdio>

#include "md5.hpp"

int main() {
    // char s[] = "";
    char s[] = "The quick brown fox jumps over the lazy dog";

    Md5::Bytes hash = Md5::hash_string(s);

    printf("%s\n", s);

    for (unsigned int i = 0; i < 16; ++i) {
        printf("%02x", (int)(hash[i]) & 0xFF);
    }
    printf("\n");
    return 0;
}
