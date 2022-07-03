#include <string.h>

#include "../crypto/crypto.h"
#include "packet.h"


void
packet_create(uint8_t packet[PACKETSIZE], uint8_t ptype, uint8_t npackets, uint8_t data[DATASIZE])
{
    size_t offset = 0;

    /* packet type */
    *(packet + offset) = ptype;
    offset += sizeof(ptype);

    /* number of packets */
    *((uint8_t *) (packet + offset)) = npackets;
    offset += sizeof(uint8_t);

    /* data */
    if (data) {
        memcpy(packet + offset, data, DATASIZE);
    } else {
        memset(packet + offset, 0, DATASIZE);
    }
}


void
packet_decode(uint8_t * ptype, uint8_t * npackets, uint8_t data[DATASIZE], uint8_t packet[PACKETSIZE])
{
    size_t offset = 0;

    /* packet type */
    *ptype = packet[offset];
    offset += sizeof(*ptype);

    /* number of packets */
    *npackets = *((uint8_t *)(packet + offset));
    offset += sizeof(*npackets);

    /* data */
    if (data) {
        memcpy(data, packet + offset, DATASIZE);
    }
}


void
packet_request_create(uint8_t packet[PACKETSIZE])
{
    packet_create(packet, PACKETTYPE_REQUEST, 1, NULL);
}


void
packet_iv_create(uint8_t packet[PACKETSIZE], uint8_t IV[IVSIZE])
{
    packet_create(packet, PACKETTYPE_IV, 1, IV);
}


uint8_t
packet_data_create(uint8_t * packet, size_t buffer_size, uint8_t * data, size_t data_size)
{
    /* compute the number of required packets */
    uint8_t npackets = data_size / DATASIZE;

    for (uint8_t i=0; i<npackets; ++i) {
        packet_create(packet + i * PACKETSIZE, PACKETTYPE_DATA, npackets - i, data + i * DATASIZE);
    }

    return npackets;
}
