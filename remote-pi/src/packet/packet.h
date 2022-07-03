#ifndef __PACKET_H__
#define __PACKET_H__


/**
 * Packet: 18 bytes (max allowed size = 32 bytes for nrf240l1+)
 * 
 * _______________________________
 * | PTYPE  | NPACKETS | DATA     |
 * |--------|----------|----------|
 * | 1 byte | 1 bytes  | 16 bytes |
 * |________|__________|__________|
 * 
 */


#define DATASIZE 16 // 16 bytes
#define PACKETSIZE 18 // bytes

#define PACKETTYPE_REQUEST 0x0
#define PACKETTYPE_IV      0x1
#define PACKETTYPE_DATA    0x2

#include <stdint.h>
#include "../crypto/crypto.h"



/**
 * @brief Create a single packet
 * 
 * @param packet 
 * @param ptype 
 * @param npackets 
 * @param data 
 */
void packet_create(uint8_t packet[PACKETSIZE], uint8_t ptype, uint8_t npackets, uint8_t data[DATASIZE]);


/**
 * @brief Decode a single packet
 * 
 * @param ptype 
 * @param npackets 
 * @param data 
 * @param packet 
 */
void packet_decode(uint8_t * ptype, uint8_t * npackets, uint8_t data[DATASIZE], uint8_t packet[PACKETSIZE]);


/**
 * @brief Request a communication sequence with the receiver
 * 
 * @param packet PACKETSIZE bytes
 */
void packet_request_create(uint8_t packet[PACKETSIZE]);


/**
 * @brief Create an IV packet
 * 
 * @param packet PACKETSIZE bytes
 */
void packet_iv_create(uint8_t packet[PACKETSIZE], uint8_t IV[IVSIZE]);


/**
 * @brief Create data packets
 * 
 * @param packet a packet to store the data packets
 * @param buffer_size MUST BE ATLEAST (data_size / DATASIZE) * PACKETSIZE
 * @param data
 * @param data_size MUST BE A MULTIPLE OF DATASIZE (16 bytes)
 */
uint8_t packet_data_create(uint8_t * packet, size_t buffer_size, uint8_t * data, size_t data_size);


#endif // __PACKET_H__
