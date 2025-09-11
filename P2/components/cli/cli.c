#include "cli.h"

#include <string.h>
#include <stdio.h>

#include <utility.h>
#include <serial_io.h>

command_fun_t find_command(const char* command, const command_t* commands, size_t n)
{
	/*
		Loop Invariant:
		0 <= i < n
		forall x | 0 <= x < i : commands[i].name != command
	 */
	for (size_t i = 0; i < n; ++i)
		if (!strcmp(command, commands[i].name))
			return commands[i].fun;

	return NULL;
}

void id_command(char*)
{
	// id + null
	char buffer[ID_WIDTH + 1];
	format_id(buffer, lownet_get_device_id());
	serial_write_line(buffer);
}

void date_command(char*)
{
	// ...?
}
