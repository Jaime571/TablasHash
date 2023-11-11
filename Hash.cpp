#include <string.h>
#include "Hash.h"

int Hash(char* key){
    int sum = 0;
    int len = strlen(key);
    if(len % 2 == 1) len++;
    for(int j = 0; j < len; j+=2)
        sum = (sum + 100 * key[j] + key[j + 1]) % 19937;
    return sum;
}

int MakeAddress(char* key, int depth){
    int retval = 0;
    int mask = 1;
    int hashVal = Hash(key);
    ///Invertir orden de bits
    for(int j = 0; j < depth; j++){
        retval = retval << 1;
        int lowbit = hashVal & mask;
        retval = retval | lowbit;
        hashVal = hashVal >> 1;
    }
    return retval;
}
