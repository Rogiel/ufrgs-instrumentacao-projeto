//
//  packet.h
//  SerialInput
//
//  Created by Rogiel Sulzbach on 6/15/16.
//  Copyright © 2016 Rogiel. All rights reserved.
//

#ifndef packet_h
#define packet_h

/**
 * The level sensor comm protocol packet structure
 */
struct packet_payload {
	/**
	 * A sync packet that indicates the start of a sequence
	 */
	uint8_t sync;
	uint32_t integer;
} __attribute__((packed));

union packet_t {
	packet_payload 	payload;
	uint8_t 		buffer[sizeof(packet_payload)];
};

#endif /* packet_h */
