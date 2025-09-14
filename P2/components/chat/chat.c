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

vvoid shout_command(char* args)
{
    if (!args || strlen(args) == 0) {
        serial_write_line("Error: No message provided");
        return;
    }
    
    chat_shout(args);
}

void tell_command(char* args)
{
    if (!args || strlen(args) == 0) {
        serial_write_line("Error: No arguments provided");
        return;
    }
    
    // Parse the input: "ID MSG" or handle "@ID MSG" format
    char* token = strtok(args, " ");
    if (!token) {
        serial_write_line("Error: Invalid format. Use: /tell ID MSG");
        return;
    }
    
    // Convert hex ID to decimal
    uint8_t dest_id;
    if (token[0] == '0' && token[1] == 'x') {
        dest_id = (uint8_t)hex_to_dec(token + 2);
    } else {
        dest_id = (uint8_t)hex_to_dec(token);
    }
    
    if (dest_id == 0) {
        serial_write_line("Error: Invalid node ID");
        return;
    }
    
    // Get the rest of the message
    char* message = strtok(NULL, "");
    if (!message || strlen(message) == 0) {
        serial_write_line("Error: No message provided");
        return;
    }
    
    chat_tell(message, dest_id);
}

void chat_receive(const lownet_frame_t* frame) 
{
    // Validate frame length
    if (frame->length == 0 || frame->length > LOWNET_PAYLOAD_SIZE) {
        return;
    }
    
    // Create a buffer for the message (add null terminator)
    char message[LOWNET_PAYLOAD_SIZE + 1];
    memset(message, 0, sizeof(message));
    
    // Copy payload and ensure only printable characters
    int msg_len = 0;
    for (int i = 0; i < frame->length && i < LOWNET_PAYLOAD_SIZE; i++) {
        if (util_printable(frame->payload[i])) {
            message[msg_len++] = frame->payload[i];
        }
    }
    
    // Format output message
    char output[MSG_BUFFER_LENGTH];
    memset(output, 0, sizeof(output));
    
    if (frame->destination == lownet_get_device_id()) {
        // This is a tell message, just for us!
        char sender_id[ID_WIDTH + 1];
        format_id(sender_id, frame->source);
        snprintf(output, sizeof(output), "[TELL from %s]: %s", sender_id, message);
    } else {
        // This is a broadcast shout message.
        char sender_id[ID_WIDTH + 1];
        format_id(sender_id, frame->source);
        snprintf(output, sizeof(output), "[SHOUT from %s]: %s", sender_id, message);
    }
    
    serial_write_line(output);
}

void chat_shout(const char* message) 
{
    if (!message || strlen(message) == 0) {
        return;
    }
    
    lownet_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    
    frame.destination = LOWNET_BROADCAST_ADDRESS; // 0xFF for broadcast
    frame.protocol = LOWNET_PROTOCOL_CHAT;
    
    // Copy message to payload, checking for printable characters
    int payload_len = 0;
    int msg_len = strlen(message);
    
    for (int i = 0; i < msg_len && payload_len < LOWNET_PAYLOAD_SIZE; i++) {
        if (util_printable(message[i])) {
            frame.payload[payload_len++] = message[i];
        }
    }
    
    frame.length = payload_len;
    lownet_send(&frame);
}

void chat_tell(const char* message, uint8_t destination) 
{
    if (!message || strlen(message) == 0 || destination == 0) {
        return;
    }
    
    lownet_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    
    frame.destination = destination;
    frame.protocol = LOWNET_PROTOCOL_CHAT;
    
    // Copy message to payload, checking for printable characters
    int payload_len = 0;
    int msg_len = strlen(message);
    
    for (int i = 0; i < msg_len && payload_len < LOWNET_PAYLOAD_SIZE; i++) {
        if (util_printable(message[i])) {
            frame.payload[payload_len++] = message[i];
        }
    }
    
    frame.length = payload_len;
    lownet_send(&frame);
}
