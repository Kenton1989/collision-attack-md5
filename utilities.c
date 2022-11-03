#include "utilities.h"


void output_bstr_as_hex(char* binstr, int does_print){
    char digest[32];
    if (does_print){
        for (unsigned int i = 0; i < 16; ++i) {
            printf("%02x", (int)(binstr[i]) & 0xFF);
        }
        printf("\n");
        return;
    }
    else{
        //possibly unused case
    }
    
}
