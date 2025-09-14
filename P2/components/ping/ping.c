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
    if (!args || strlen(args) == 0) {
        serial_write_line("Error: No node ID provided");
        return;
    }
    
    // Parse node ID (handle hex format)
    uint8_t dest_id;
    if (args[0] == '0' && args[1] == 'x') {
        dest_id = (uint8_t)hex_to_dec(args + 2);
    } else {
        dest_id = (uint8_t)hex_to_dec(args);
    }
    
    if (dest_id == 0 && strcmp(args, "0") != 0) {
        serial_write_line("Error: Invalid node ID");
        return;
    }
    
    ping(dest_id);
    
    // Print informational message
    char buffer[50];
    char id_str[ID_WIDTH + 1];
    format_id(id_str, dest_id);
    snprintf(buffer, sizeof(buffer), "Ping sent to %s", id_str);
    serial_write_line(buffer);
}

void ping(uint8_t node) 
{
    lownet_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    
    ping_packet_t ping_packet;
    memset(&ping_packet, 0, sizeof(ping_packet));
    
    // Set up the ping packet
    ping_packet.timestamp_out = lownet_get_time();
    ping_packet.timestamp_back.seconds = 0;  // Will be filled by responder
    ping_packet.timestamp_back.parts = 0;
    ping_packet.origin = lownet_get_device_id();
    
    // Set up the frame
    frame.destination = node;
    frame.protocol = LOWNET_PROTOCOL_PING;
    frame.length = sizeof(ping_packet_t);
    
    memcpy(frame.payload, &ping_packet, sizeof(ping_packet_t));
    
    lownet_send(&frame);
}


void ping_receive(const lownet_frame_t* frame) 
{
    if (frame->length < sizeof(ping_packet_t)) {
        // Malformed frame. Discard.
        return;
    }
    
    ping_packet_t ping_packet;
    memcpy(&ping_packet, frame->payload, sizeof(ping_packet_t));
    
    uint8_t my_id = lownet_get_device_id();
    
    if (ping_packet.origin == my_id) {
        // This is a response to our ping (pong)
        lownet_time_t current_time = lownet_get_time();
        
        // Calculate round-trip time
        lownet_time_t rtt = time_diff(&ping_packet.timestamp_out, &current_time);
        
        char buffer[100];
        char sender_id[ID_WIDTH + 1];
        char time_str[TIME_WIDTH + 1];
        
        format_id(sender_id, frame->source);
        format_time(time_str, &rtt);
        
        snprintf(buffer, sizeof(buffer), "Pong from %s, RTT: %s", sender_id, time_str);
        serial_write_line(buffer);
        
    } else {
        // This is a ping request, send a pong response
        lownet_frame_t response_frame;
        memset(&response_frame, 0, sizeof(response_frame));
        
        ping_packet_t response_packet = ping_packet;  // Copy original packet
        response_packet.timestamp_back = lownet_get_time();  // Add our timestamp
        
        // Set up response frame
        response_frame.destination = ping_packet.origin;  // Send back to origin
        response_frame.protocol = LOWNET_PROTOCOL_PING;
        response_frame.length = sizeof(ping_packet_t);
        
        memcpy(response_frame.payload, &response_packet, sizeof(ping_packet_t));
        
        lownet_send(&response_frame);
        
        // Print informational message
        char buffer[50];
        char sender_id[ID_WIDTH + 1];
        format_id(sender_id, frame->source);
        snprintf(buffer, sizeof(buffer), "Ping received from %s, pong sent", sender_id);
        serial_write_line(buffer);
    }
}
