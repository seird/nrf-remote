#include <stdio.h>
#include <pico/stdlib.h>


#include "../button/button.h"
#include "../pin/pin.h"
#include "../radio/radio.h"
#include "../packet/packet.h"
#include "../crypto/crypto.h"
#include "../sleep/sleep.h"
#include "../commands.h"
#include "../crypto/aes.h"
#include "../key.h"
#include "pins.h"


RF24 * radio = NULL;
struct AES_ctx ctx_encryption;
struct AES_ctx ctx_auth;


static void
setup()
{
    stdio_init_all();

    pin_init(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    pin_off(PICO_DEFAULT_LED_PIN);

    pin_init(PIN_TURN_ON, GPIO_OUT);
    pin_off(PIN_TURN_ON);

    radio = radio_setup(0, PACKETSIZE);
    radio->startListening(); // set the radio in RX mode

    AES_init_ctx(&ctx_encryption, PRESHARED_KEY_ENCRYPTION);
    
    /* CBC-MAC IV is always 0 */
    uint8_t iv_cbc_mac[BLOCKSIZE];
    memset(iv_cbc_mac, 0, BLOCKSIZE);
    AES_init_ctx_iv(&ctx_auth, PRESHARED_KEY_AUTH, iv_cbc_mac);
}


static void
pc_turn_on()
{
    pin_on(PICO_DEFAULT_LED_PIN);
    pin_on(PIN_TURN_ON);
    sleep_ms(1000);
    pin_off(PIN_TURN_ON);
    pin_off(PICO_DEFAULT_LED_PIN);
}


static void
pc_turn_off()
{
    //
}


static void
pc_reset()
{
    //
}


static void
loop_function(void * arg)
{
    (void) arg;

    /* Check for a packet */
    uint8_t packet[PACKETSIZE];
    uint8_t pipe;
    if (!radio_receive_packet(radio, packet, &pipe)) {
        return;
    }

    /* Decode the response */
    uint8_t ptype;
    uint8_t npackets;
    packet_decode(&ptype, &npackets, NULL, packet);

    /* Verify response -- We are only expecting "request to start" packets */
    if (ptype != PACKETTYPE_REQUEST || npackets != 1) {
        radio->flush_rx();
        return;
    }

    /* Reply with an IV / nonce */
    memset(packet, 0, PACKETSIZE);
    uint8_t IV[IVSIZE];
    packet_iv_create(packet, IV);
    radio_send_packets(radio, packet, 1);

    /* Wait up to 2 seconds for a response -- the transmitter will reply with an encrypted command + mac = 2 packets */
    uint8_t cmd_npackets = 2;
    uint8_t packet_command[cmd_npackets*PACKETSIZE];
    memset(packet_command, 0, cmd_npackets*PACKETSIZE);
    if (!radio_receive_packets_timeout(radio, packet_command, cmd_npackets, &pipe, 2*1e6)) {
        pin_blink(PICO_DEFAULT_LED_PIN, 1, 100);
        radio->flush_rx();
        return;
    }

    /* Decode the packets */
    uint8_t data[cmd_npackets*DATASIZE];
    for (uint8_t i=0; i<cmd_npackets; ++i) {
        packet_decode(&ptype, &npackets, data + i*DATASIZE, packet_command + i*PACKETSIZE);
        /* Verify response */
        if (ptype != PACKETTYPE_DATA || npackets != (cmd_npackets-i)) {
            pin_blink(PICO_DEFAULT_LED_PIN, 2, 100);
            radio->flush_rx();
            return;
        }
    }

    /* Verify the CBC-MAC */
    uint8_t * received_mac = data + DATASIZE;
    uint8_t computed_mac[BLOCKSIZE];
    cbc_mac(computed_mac, data, DATASIZE, &ctx_auth);
    if (memcmp(received_mac, computed_mac, BLOCKSIZE) != 0) {
        pin_blink(PICO_DEFAULT_LED_PIN, 3, 100);
        radio->flush_rx();
        return;
    }

    /* Ciphertext is OK -> Decrypt the data and execute the command */
    uint32_t command;
    if (decrypt_command(&command, data, &ctx_encryption, IV)) {
        switch (command) {
            case CMD_TURN_ON:
                pc_turn_on();
                break;
            case CMD_TURN_OFF:
                pc_turn_off();
                break;
            case CMD_RESET:
                pc_reset();
                break;
        }
    }
}


int
main()
{
    setup();

    sleepconfig_t sleepconfig = {
        .mode = SM_DEFAULT, // TODO: test SM_SLEEP
        .hours = 0,
        .minutes = 0,
        .seconds = 1,
        .loop_function = loop_function,
        .arg = NULL,
    };

    sleep_run(&sleepconfig);

    return 0;
}
