#ifndef GUARD_LOWNET_H
#define GUARD_LOWNET_H

#include <assert.h>
#include <stdint.h>

#define LOWNET_SERVICE_CORE 1
#define LOWNET_SERVICE_PRIO 10

#define LOWNET_MAX_PROTOCOLS 10

#define LOWNET_FRAME_SIZE 212
#define LOWNET_HEAD_SIZE 8
#define LOWNET_CRC_SIZE 4
#define LOWNET_PAYLOAD_SIZE (LOWNET_FRAME_SIZE - (LOWNET_HEAD_SIZE + LOWNET_CRC_SIZE)) // 200 bytes.

#define LOWNET_TIME_RESOLUTION 256
#define LOWNET_BROADCAST_ADDRESS 0xFF

typedef struct __attribute__((__packed__))
{
	uint8_t magic[2];
	uint8_t source;
	uint8_t destination;
	uint8_t protocol;
	uint8_t length;
	uint8_t padding[2];
	uint8_t payload[LOWNET_PAYLOAD_SIZE];
	uint32_t crc;
} lownet_frame_t;

typedef struct __attribute__((__packed__)) {
	uint32_t seconds; // Seconds since UNIX epoch.
	uint8_t parts; // Milliseconds, 1000/256 resolution.
} lownet_time_t;

static_assert(sizeof(lownet_time_t) == 5);

typedef void (*lownet_recv_fn)(const lownet_frame_t* frame);

// Usage: lownet_register_protocol(PROTO, HANDLER)
// Pre:   PROTO is a protocol identifier which has not been registered
//        HANDLER is the frame handler for PROTO
// Value: 0 if PROTO was successfully registered, non-0 otherwise
int lownet_register_protocol(uint8_t protocol, lownet_recv_fn handler);

void lownet_init(void);
void lownet_send(const lownet_frame_t* frame);

lownet_time_t lownet_get_time();
uint8_t lownet_get_device_id();

#endif
