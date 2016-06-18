//
//  packet.h
//  SerialInput
//
//  Created by Rogiel Sulzbach on 6/15/16.
//  Copyright © 2016 Rogiel. All rights reserved.
//

#ifndef packet_h
#define packet_h

typedef float frequency_t;

/**
 * The level sensor comm protocol packet structure
 */
struct packet_payload {
	/**
	 * A sync packet that indicates the start of a sequence
	 */
	uint8_t sync;

	frequency_t level;
	frequency_t reference;
	frequency_t environment;
} __attribute__((packed));

union packet_t {
	packet_payload 	payload;
	uint8_t 		buffer[sizeof(packet_payload)];
};

#endif /* packet_h */
