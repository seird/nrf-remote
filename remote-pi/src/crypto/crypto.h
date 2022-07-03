#ifndef __CRYPTO_H__
#define __CRYPTO_H__


#define KEYSIZE 16
#define BLOCKSIZE 16
#define IVSIZE 16


#include <stdint.h>
#include "../packet/packet.h"
#include "aes.h"
#include "../commands.h"



void encrypt(uint8_t * data, size_t size, struct AES_ctx * ctx, uint8_t IV[IVSIZE]);
void decrypt(uint8_t * data, size_t size, struct AES_ctx * ctx, uint8_t IV[IVSIZE]);

void encrypt_command(uint8_t data[DATASIZE], uint32_t command, struct AES_ctx * ctx, uint8_t IV[IVSIZE]);
bool decrypt_command(uint32_t * command, uint8_t data[DATASIZE], struct AES_ctx * ctx, uint8_t IV[IVSIZE]);

void cbc_mac(uint8_t mac[BLOCKSIZE], uint8_t * data, size_t size, struct AES_ctx * ctx);


#endif // __CRYPTO_H__
