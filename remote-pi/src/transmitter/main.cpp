#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../radio/radio.h"
#include "../packet/packet.h"
#include "../crypto/crypto.h"
#include "../commands.h"
#include "../key.h"
#include "../crypto/aes.h"


RF24 * radio = NULL;
struct AES_ctx ctx_encryption;
struct AES_ctx ctx_auth;


static void
setup()
{
    printf("Setting up ...\n");
    radio = radio_setup(1, PACKETSIZE);
    if (!radio) {
        printf("Radio failed to initialize\n");
    }
    radio->stopListening(); // set the radio in TX mode
    radio->powerDown();

    AES_init_ctx(&ctx_encryption, PRESHARED_KEY_ENCRYPTION);

    /* CBC-MAC IV is always 0 */
    uint8_t iv_cbc_mac[BLOCKSIZE];
    memset(iv_cbc_mac, 0, BLOCKSIZE);
    AES_init_ctx_iv(&ctx_auth, PRESHARED_KEY_AUTH, iv_cbc_mac);
    printf("Finished setting up\n");
}


static void
loop_function(void * arg)
{
    printf("Starting\n");
    (void) arg;

    /* return to normal-power mode */
    radio->powerUp();

    radio->flush_rx();

    /* Send a request to start */
    uint8_t packet[PACKETSIZE];
    packet_request_create(packet);
    radio_send_packets(radio, packet, 1);
    printf("Sent init packet\n");

    /* Wait up to 4 seconds for a response -- the receiver will reply with an IV */
    memset(packet, 0, PACKETSIZE);
    if (!radio_receive_packet_timeout(radio, packet, 4*1e6)) {
        printf("Timeout for IV response\n");
        return;
    }

    /* Decode the response */
    uint8_t ptype;
    uint8_t npackets;
    uint8_t IV[DATASIZE];
    packet_decode(&ptype, &npackets, IV, packet);

    /* Verify response */
    if (ptype != PACKETTYPE_IV || npackets != 1) {
        printf("Packet could not be decoded\n");
        return;
    }

    /* Encrypt the turn on command using the IV from the receiver */
    uint8_t data[DATASIZE];
    encrypt_command(data, CMD_TURN_ON, &ctx_encryption, IV);

    /* Compute the CBC-MAC on the ciphertext */
    uint8_t mac[BLOCKSIZE];
    cbc_mac(mac, data, DATASIZE, &ctx_auth);

    /* Create a packet with as payload the encrypted command and the mac */
    uint8_t packet_data[DATASIZE + BLOCKSIZE]; // data + mac
    memcpy(packet_data, data, DATASIZE);
    memcpy(packet_data + DATASIZE, mac, BLOCKSIZE);
    uint8_t packets_command[2*PACKETSIZE];
    npackets = packet_data_create(packets_command, 2*PACKETSIZE, packet_data, DATASIZE + BLOCKSIZE);
    printf("Sending final packet\n");
    radio_send_packets(radio, packets_command, npackets);
    
    /* enter low-power mode */
    // radio->powerDown();
}


int
main()
{
    setup();

    loop_function(NULL);
}
