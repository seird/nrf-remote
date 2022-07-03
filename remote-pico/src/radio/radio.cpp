#include <pico/stdlib.h>

#include "radio.h"
#include "../packet/packet.h"


RF24 *
radio_setup(bool radio_number, uint8_t payload_size)
{
    // instantiate an object for the nRF24L01 transceiver
    RF24 * radio = new RF24(CE_PIN, CSN_PIN);

    // Let these addresses be used for the pair
#if USE_RELAY
    uint8_t address[][6] = {"3AAAA", "4AAAA"};
#else
    uint8_t address[][6] = {"1AAAA", "2AAAA"};
#endif
    // It is very helpful to think of an address as a path instead of as
    // an identifying device destination

    // to use different addresses on a pair of radios, we need a variable to
    // uniquely identify which address this radio will use to transmit
    // 0 uses address[0] to transmit, 1 uses address[1] to transmit

    // initialize the transceiver on the SPI bus
    if (!radio->begin()) {
        return NULL;
    }


    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to
    // each other.
    // radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit a float
    radio->setPayloadSize(payload_size);
    
    if (radio->isPVariant()){
        radio->setDataRate(RF24_250KBPS); // RF24_1MBPS default
    }
    radio->setCRCLength(RF24_CRC_16); // RF24_CRC_16 default
    

    // set the TX address of the RX node into the TX pipe
    radio->openWritingPipe(address[radio_number]); // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio->openReadingPipe(1, address[!radio_number]); // using pipe 1

    // additional setup specific to the node's role
    // if (role) {
    //     radio.stopListening(); // put radio in TX mode
    // }
    // else {
    //     radio.startListening(); // put radio in RX mode
    // }

    // For debugging info
    // radio.printDetails();       // (smaller) function that prints raw register values
    // radio.printPrettyDetails(); // (larger) function that prints human readable data

    return radio;
}


void
radio_send_packets(RF24 * radio, uint8_t * packets, uint8_t npackets)
{
    radio->stopListening();
    for (uint8_t i=0; i<npackets; ++i) {
        radio->write(packets + i*PACKETSIZE, PACKETSIZE);
    }
}


bool
radio_receive_packet(RF24 * radio, uint8_t * packet, uint8_t * pipe)
{
    radio->startListening();
    if (radio->available(pipe)) {               // is there a payload? get the pipe number that recieved it
        uint8_t bytes = radio->getPayloadSize(); // get the size of the payload
        radio->read(packet, bytes);              // fetch payload from FIFO
        return true;
    }
    return false;
}


bool
radio_receive_packet_timeout(RF24 * radio, uint8_t * packet, uint8_t * pipe, uint32_t timeout_us)
{
    uint32_t t_start = time_us_32();
    while (true) {
        if (radio_receive_packet(radio, packet, pipe)) {
            return true;
        }

        if ((time_us_32() - t_start) > timeout_us) {
            return false;
        }

        sleep_ms(1);
    }
}


bool
radio_receive_packets_timeout(RF24 * radio, uint8_t * packets, uint8_t npackets, uint8_t * pipe, uint32_t timeout_us)
{
    uint32_t t_start = time_us_32();
    uint32_t timeout = timeout_us;
    for (uint8_t i=0; i<npackets; ++i) {
        uint32_t t_elapsed = time_us_32() - t_start;
        timeout -= t_elapsed;
        timeout = MAX(timeout, 0);
        
        if (!radio_receive_packet_timeout(radio, packets + i*PACKETSIZE, pipe, timeout)) {
            return false;
        }
    }
    return true;
}
