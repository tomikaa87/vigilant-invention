/*
 * radio_protocol.c
 *
 *  Created on: 2018. maj. 5.
 *      Author: tkarpati
 */

#include "radio_protocol.h"

#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE(x)		(sizeof(x) / sizeof(x[0]))

static void print_command(protocol_cmd_t command)
{
	switch (command)
	{
    case PROTO_CMD_NOP:
        printf("NOP");
        break;

	case PROTO_CMD_SHUTTER_1_UP:
		printf("SHUTTER_1_UP");
		break;

	case PROTO_CMD_SHUTTER_1_DOWN:
		printf("SHUTTER_1_DOWN");
		break;

	case PROTO_CMD_SHUTTER_2_UP:
		printf("SHUTTER_2_UP");
		break;

	case PROTO_CMD_SHUTTER_2_DOWN:
		printf("SHUTTER_2_DOWN");
		break;

    case PROTO_CMD_ALL_UP:
        printf("PROTO_CMD_ALL_UP");
        break;

    case PROTO_CMD_ALL_DOWN:
        printf("PROTO_CMD_ALL_DOWN");
        break;

	default:
		printf("UNKNOWN");
	}
}

void protocol_msg_init(protocol_msg_t* msg)
{
	memset(msg->data, 0, sizeof(protocol_msg_t));
	msg->msg_magic = PROTO_MSG_MAGIC;
}

void print_protocol_message(const protocol_msg_t* msg)
{
	printf("  Type: ");

	switch (msg->msg_type)
	{
	case PROTO_MSG_COMMAND:
		printf("COMMAND");
		break;

	case PROTO_MSG_READ_STATUS:
		printf("READ_STATUS");
		break;

	case PROTO_MSG_READ_TEMPERATURE:
		printf("READ_TEMPERATURE");
		break;

	case PROTO_MSG_READ_TEMPERATURE_RESULT:
	    printf("READ_TEMPERATURE_RESULT");
	    break;

	case PROTO_MSG_READ_STATUS_RESULT:
	    printf("READ_STATUS_RESULT");
	    break;

	case PROTO_MSG_RESULT:
		printf("RESULT");
		break;

	default:
		printf("UNKNOWN");
	}

	printf("\r\n  Payload:\r\n");

	switch (msg->msg_type)
	{
	case PROTO_MSG_COMMAND:
	{
		printf("    Command: ");
		print_command(msg->payload.command);
		printf("\r\n");

		break;
	}

	case PROTO_MSG_READ_STATUS_RESULT:
	{
		printf("    Status:\r\n");
		printf("      Firmware version: %s\r\n", msg->payload.status.firmware_ver);
		printf("      Last commands:\r\n");

		for (uint8_t i = 0; i < ARRAY_SIZE(msg->payload.status.last_commands); ++i)
		{
		    printf("        ");
			print_command(msg->payload.status.last_commands[i]);
			printf("\r\n");
		}

		break;
	}

	case PROTO_MSG_READ_TEMPERATURE_RESULT:
		printf("    Temperature: %ld\r\n", msg->payload.temp.temperature);
		break;

	case PROTO_MSG_RESULT:
		printf("    Result code: ");

		switch (msg->payload.result.result_code)
		{
		case PROTO_RESULT_OK:
			printf("OK");
			break;

		case PROTO_RESULT_INVALID_MSG:
			printf("INVALID_MSG");
			break;

		case PROTO_RESULT_UNKNOWN_MSG_TYPE:
			printf("UNKNOWN_MSG_TYPE");
			break;

		case PROTO_RESULT_NO_SUCH_COMMAND:
			printf("NO_SUCH_COMMAND");
			break;

		case PROTO_RESULT_UNSUPPORTED_COMMAND:
		    printf("UNSUPPORTED_COMMAND");
		    break;

		default:
			printf("UNKNOWN");
		}

		printf("\r\n");

		break;

	default:
		printf("    unknown\r\n");
	}

	printf("  Data:");

	for (uint8_t i = 0; i < sizeof(protocol_msg_t); ++i)
	{
		if (i % 8 == 0)
			printf("\r\n   ");

		// tiny_printf doesn't support %02x
		printf(" ");
		if (msg->data[i] < 0x10)
		    printf("0");
        printf("%x", msg->data[i]);
	}

	printf("\r\n");

	printf("  Magic: %s\r\n",
	        (msg->msg_magic == PROTO_MSG_MAGIC) ? "OK" : "INVALID");
}
