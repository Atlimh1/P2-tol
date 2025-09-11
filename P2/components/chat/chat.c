#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <esp_log.h>

#include <string.h>

#include <lownet.h>
#include <serial_io.h>

#include "chat.h"

#define TAG "CHAT"

void chat_init()
{
	if (lownet_register_protocol(LOWNET_PROTOCOL_CHAT, chat_receive) != 0)
		{
			ESP_LOGE(TAG, "Error registering CHAT protocol");
		}
}

void shout_command(char* args)
{
	// ...?
}

void tell_command(char* args)
{
	// ...?
}

void chat_receive(const lownet_frame_t* frame) {
	if (frame->destination == lownet_get_device_id()) {
		// This is a tell message, just for us!

		//  ...?
	} else {
		// This is a broadcast shout message.

		// ...?
	}
}

void chat_shout(const char* message) {
	// ...?
}

void chat_tell(const char* message, uint8_t destination) {
	// ...?
}
