#include <stdio.h>

#include "md5.h"

int main()
{
    char s[] = "The quick brown fox jumps over the lazy dog";
    char *hash = md5String(s);
    printf("%s\n", s);
    for (unsigned int i = 0; i < 16; ++i)
    {
        printf("%02x", (int)(hash[i]) & 0xFF);
    }
    printf("\n");
    return 0;
}
