//
//  packet.h
//  SerialInput
//
//  Created by Rogiel Sulzbach on 6/15/16.
//  Copyright © 2016 Rogiel. All rights reserved.
//

#ifndef packet_h
#define packet_h

typedef uint32_t frequency_t;

struct measurement_t {
	uint8_t id;
	frequency_t frequency;
} __attribute__((packed));

/**
 * The level sensor comm protocol packet structure
 */
struct packet_payload {
	/**
	 * A sync packet that indicates the start of a sequence
	 */
	uint8_t sync;

	measurement_t level;
	measurement_t reference;
	measurement_t environment;
} __attribute__((packed));

union packet_t {
	packet_payload 	payload;
	uint8_t 		buffer[sizeof(packet_payload)];
} __attribute__((packed));

#endif /* packet_h */
