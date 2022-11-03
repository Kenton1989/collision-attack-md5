#include <stdio.h>

#include "md5.h"

#include "utilities.h"

int main() {
    char s[] = "Hello World\n";
    char s2[] = "Hello world\0";
    char *hash = md5String(s);
    printf(s);
    output_bstr_as_hex(hash, 1);
    printf("hello");
    // unsigned char *pc = "\x00\x00\x00\x01";
    // int *pa = (int*)pc;

    // printf("%08x\n", *pa);
    // printf("%02x%02x%02x%02x", pc[0], pc[1], pc[2], pc[3]);
    // return 0;
}

