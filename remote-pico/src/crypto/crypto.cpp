#include <pico/stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "crypto.h"
#include "../commands.h"
#include "../packet/packet.h"
#include "aes.h"


void
encrypt(uint8_t * data, size_t size, struct AES_ctx * ctx, uint8_t IV[IVSIZE])
{
    AES_ctx_set_iv(ctx, IV);
    AES_CBC_encrypt_buffer(ctx, data, size);
}


void
decrypt(uint8_t * data, size_t size, struct AES_ctx * ctx, uint8_t IV[IVSIZE])
{
    AES_ctx_set_iv(ctx, IV);
    AES_CBC_decrypt_buffer(ctx, data, size);
}


void
cbc_mac(uint8_t mac[BLOCKSIZE], uint8_t * data, size_t size, struct AES_ctx * ctx)
{
    uint8_t * temp = (uint8_t *) malloc(size);
    memcpy(temp, data, size);

    uint8_t IV[IVSIZE]; // CBC-MAC uses a constant IV
    memset(IV, 0, IVSIZE);

    AES_ctx_set_iv(ctx, IV);
    AES_CBC_encrypt_buffer(ctx, temp, size);

    size_t nblocks = size / BLOCKSIZE;
    memcpy(mac, temp + (nblocks-1)*BLOCKSIZE, BLOCKSIZE);

    free(temp);
}


void
encrypt_command(uint8_t data[DATASIZE], uint32_t command, struct AES_ctx * ctx, uint8_t IV[IVSIZE])
{
    int padding = 16 - sizeof(uint32_t);

    memcpy(data, &command, sizeof(uint32_t));
    memset(data + sizeof(uint32_t), padding, 16 - sizeof(uint32_t));

    encrypt(data, DATASIZE, ctx, IV);
}


bool
decrypt_command(uint32_t * command, uint8_t data[DATASIZE], struct AES_ctx * ctx, uint8_t IV[IVSIZE])
{
    decrypt(data, DATASIZE, ctx, IV);

    /* verify padding */
    int padding = 16 - sizeof(uint32_t);
    for (size_t i=sizeof(sizeof(uint32_t)); i<DATASIZE; ++i) {
        if (data[i] != padding) {
            printf("%d != %d", data[i], padding);
            return false;
        }
        printf("%d == %d", data[i], padding);
    }
    
    memcpy(command, data, sizeof(uint32_t));

    return true;
}
