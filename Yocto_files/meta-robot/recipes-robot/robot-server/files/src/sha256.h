#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <stddef.h>

// Produce hex string lowecase de 64-character
void sha256_hex(const char *input, size_t len, char out[65]);

#endif