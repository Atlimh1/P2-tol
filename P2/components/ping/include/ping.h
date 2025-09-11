#ifndef GUARD_APP_PING_H
#define GUARD_APP_PING_H

#include <stdint.h>

#include "lownet.h"

#define LOWNET_PROTOCOL_PING 0x03

void ping_init();

// Usage: ping_command(ID)
// Pre:   ID is a valid node id.
// Post:  A ping has been sent to the node identified by ID.
void ping_command(char* args);


// Usage: ping(NODE)
// Pre:   NODE is a node id
// Post: A ping has been sent to the node identified by NODE
void ping(uint8_t node);

void ping_receive(const lownet_frame_t* frame);

#endif
