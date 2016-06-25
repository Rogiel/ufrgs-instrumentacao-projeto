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
 * The type used to represent a number of cycles
 */
typedef uint32_t cicles_t;

/**
 * A struture containing the measurement ID and cicle count
 */
struct measurement_t {
    /**
     * The measurement ID
     */
	uint8_t id;

    /**
     * The number of cicles that happened between the last two oscilator low borders
     */
    cicles_t cicleCount;
} __attribute__((packed));

/**
 * The level sensor communication protocol packet structure
 */
struct packet_payload {
	/**
	 * A sync packet that indicates the start of a sequence
	 */
	uint8_t sync;

    /**
     * A measurement payload representing the level sensor
     */
	measurement_t level;

    /**
     * A measurement payload representing the reference sensor
     */
	measurement_t reference;

    /**
     * A measurement payload representing the environment sensor
     */
	measurement_t environment;
} __attribute__((packed));

union packet_t {
	packet_payload 	payload;
	uint8_t 		buffer[sizeof(packet_payload)];
} __attribute__((packed));

#endif /* packet_h */
