#ifndef __RADIO_H__
#define __RADIO_H__

/*
    https://nrf24.github.io/RF24/
    https://nrf24.github.io/RF24/md_docs_pico_sdk.html
*/

#include <stdint.h>
#include <RF24/RF24.h>         // RF24 radio object


#define CE_PIN 22
#define CSN_PIN 0
#define IRQ_PIN 6


void radio_send_packets(RF24 * radio, uint8_t * packets, uint8_t npackets);

bool radio_receive_packet(RF24 * radio, uint8_t * packet);
bool radio_receive_packet_timeout(RF24 * radio, uint8_t * packet, uint32_t timeout_us);
bool radio_receive_packets_timeout(RF24 * radio, uint8_t * packets, uint8_t npackets, uint32_t timeout_us);

RF24 * radio_setup(bool radio_number, uint8_t payload_size);


#endif // __RADIO_H__
