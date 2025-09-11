#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <esp_log.h>

#include "ping.h"

#include "serial_io.h"

#define TAG "PING"

void ping_init()
{
	if (lownet_register_protocol(LOWNET_PROTOCOL_PING, ping_receive) != 0)
		{
			ESP_LOGE(TAG, "Error registering PING protocol");
		}
}

typedef struct __attribute__((__packed__))
{
	lownet_time_t timestamp_out;
	lownet_time_t timestamp_back;
	uint8_t origin;
} ping_packet_t;

void ping_command(char* args)
{
	// ...?
}

void ping(uint8_t node) {
	// ...?
}


void ping_receive(const lownet_frame_t* frame) {
	if (frame->length < sizeof(ping_packet_t)) {
		// Malformed frame.  Discard.
		return;
	}

	// ...?
}
