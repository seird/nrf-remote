#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>

#include "../pin/pin.h"
#include "../radio/radio.h"
#include "../packet/packet.h"


const uint8_t address_read_node_A[6] = "1AAAA";
const uint8_t address_write_node_A[6] = "2AAAA";
const uint8_t pipe_A = 1;

const uint8_t address_read_node_B[6] = "3AAAA";
const uint8_t address_write_node_B[6] = "4AAAA";
const uint8_t pipe_B = 2;


RF24 * radio = NULL;

bool blink_error = false;
bool blink_radio_failure = false;
bool blink_traffic = false;

int radio_failures = 0;

const uint32_t LOOP_SLEEP_MS = 1000;


static bool
radio_init()
{
    if (!radio->begin()) {
        return false;
    }

    radio->failureDetected = 0; 

    radio->setPayloadSize(sizeof(float));
    radio->setDataRate(RF24_250KBPS); // RF24_1MBPS default
    radio->setCRCLength(RF24_CRC_16); // RF24_CRC_16 default

    radio->openReadingPipe(pipe_A, address_read_node_A);
    radio->openReadingPipe(pipe_B, address_read_node_B);

    radio->startListening(); // set the radio in RX mode
    return true;
}


static bool
setup()
{
    stdio_init_all();

    sleep_ms(100);

    pin_init(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    pin_off(PICO_DEFAULT_LED_PIN);

    /* set up the radio */
    radio = new RF24(CE_PIN, CSN_PIN);
    return radio_init();
}


void
core1(void)
{
    while (true) {
        if (blink_traffic) {
            pin_blink(PICO_DEFAULT_LED_PIN, 1, 50);
            blink_traffic = false;
        }

        if (blink_error) {
            pin_blink(PICO_DEFAULT_LED_PIN, 2, 200);
            blink_error = false;
        }

        if (blink_radio_failure) { // blink fast for 10 seconds
            pin_blink(PICO_DEFAULT_LED_PIN, 100, 100);
            blink_radio_failure = false;
        }

        sleep_ms(10);
    }

}


static void
loop_function(void * arg)
{
    (void) arg;

    uint8_t buffer[radio->getPayloadSize()];
    uint8_t pipe;

    // Check for a new packet
    if (!radio_receive_packet_timeout(radio, buffer, &pipe, 1e5)) {
        return;
    }

    // We are only expecting packets on pipe_A or pipe_B (relay between 2 nodes)
    if (pipe != pipe_A && pipe != pipe_B) {
        blink_error = true;
        return;
    }

    // Pass the packet on to the next node
    // Pipe 1 receives from A --> write to B
    // Pipe 2 receives from B --> write to A
    switch (pipe) {
        case pipe_A:
            radio->openWritingPipe(address_write_node_B);
            break;
        case pipe_B:
            radio->openWritingPipe(address_write_node_A);
            break;
        default:
            blink_error = true;
            return;
    }

    radio_send_packets(radio, buffer, 1);
    blink_traffic = true;
    radio->startListening();
}


int
main()
{
    if (!setup()) {
        /* setup failed, blink the pico LED */
        while (true) {
            pin_blink(PICO_DEFAULT_LED_PIN, 1, 500);
        }
    }

    multicore_launch_core1(&core1);

    while (true) {
        if (radio->failureDetected) {
            ++radio_failures;
            blink_radio_failure = true;
            radio_init();
        }

        loop_function(NULL);

        sleep_ms(LOOP_SLEEP_MS);
    }
}
