#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <stdlib.h>

#include "../button/button.h"
#include "../pin/pin.h"
#include "../radio/radio.h"
#include "../packet/packet.h"
#include "../crypto/crypto.h"
#include "../sleep/sleep.h"
#include "../commands.h"
#include "../key.h"
#include "pins.h"
#include "../crypto/aes.h"


RF24 * radio = NULL;
struct AES_ctx ctx_encryption;
struct AES_ctx ctx_auth;


static void
setup()
{
    stdio_init_all();

    sleep_ms(100);

    pin_init(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    button_init(PIN_BTN);

    radio = radio_setup(1, PACKETSIZE);
    radio->stopListening(); // set the radio in TX mode
    radio->powerDown();

    AES_init_ctx(&ctx_encryption, PRESHARED_KEY_ENCRYPTION);

    /* CBC-MAC IV is always 0 */
    uint8_t iv_cbc_mac[BLOCKSIZE];
    memset(iv_cbc_mac, 0, BLOCKSIZE);
    AES_init_ctx_iv(&ctx_auth, PRESHARED_KEY_AUTH, iv_cbc_mac);
}


static void
loop_function(void * arg)
{
    (void) arg;

    pin_blink(PICO_DEFAULT_LED_PIN, 1, 300);

    /* return to normal-power mode */
    radio->powerUp();

    radio->flush_rx();

    /* Send a request to start */
    uint8_t packet[PACKETSIZE];
    uint8_t pipe;
    packet_request_create(packet);
    radio_send_packets(radio, packet, 1);

    /* Wait up to 4 seconds for a response -- the receiver will reply with an IV */
    memset(packet, 0, PACKETSIZE);
    if (!radio_receive_packet_timeout(radio, packet, &pipe,  4*1e6)) {
        return;
    }

    /* Decode the response */
    uint8_t ptype;
    uint8_t npackets;
    uint8_t IV[DATASIZE];
    packet_decode(&ptype, &npackets, IV, packet);

    /* Verify response */
    if (ptype != PACKETTYPE_IV || npackets != 1) {
        pin_blink(PICO_DEFAULT_LED_PIN, 3, 100);
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
    radio_send_packets(radio, packets_command, npackets);
    
    /* enter low-power mode */
    radio->powerDown();

    pin_blink(PICO_DEFAULT_LED_PIN, 2, 100);
}


int
main()
{
    setup();

    sleepconfig_t sleepconfig = {
        .mode = SM_DORMANT,
        .loop_function = loop_function,
        .arg = NULL,
        .pin_wakeup = PIN_BTN,
        .edge = true,
        .active = true,
    };

    sleep_run(&sleepconfig);
}
