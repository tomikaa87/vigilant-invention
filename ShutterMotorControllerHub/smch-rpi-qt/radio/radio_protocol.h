/*
 * radio_protocol.h
 *
 *  Created on: 2018. maj. 5.
 *      Author: tkarpati
 */

#ifndef RADIO_PROTOCOL_H_
#define RADIO_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#ifdef WIN32
#define PROTO_PACKED
#else
#define PROTO_PACKED		__attribute__((__packed__))
#endif

#define PROTO_MSG_MAGIC		('T' << 8 | 'K')

typedef enum
{
	PROTO_MSG_COMMAND = 0,
	PROTO_MSG_READ_STATUS = 1,
	PROTO_MSG_READ_TEMPERATURE = 2,
	PROTO_MSG_READ_TEMPERATURE_RESULT = 253,
	PROTO_MSG_READ_STATUS_RESULT = 254,
	PROTO_MSG_RESULT = 255
} PROTO_PACKED protocol_msg_type_t;

typedef enum
{
    PROTO_CMD_NOP = 0,

    PROTO_CMD_SHUTTER_1_UP = 1,
    PROTO_CMD_SHUTTER_1_DOWN = 2,
    PROTO_CMD_SHUTTER_2_UP = 3,
    PROTO_CMD_SHUTTER_2_DOWN = 4,
    PROTO_CMD_ALL_UP = 5,
    PROTO_CMD_ALL_DOWN = 6
} PROTO_PACKED protocol_cmd_t;

typedef struct
{
	char firmware_ver[16];
	protocol_cmd_t last_commands[10];
} PROTO_PACKED protocol_status_payload_t;

typedef struct
{
	int32_t temperature;
} PROTO_PACKED protocol_temp_payload_t;

typedef enum
{
	PROTO_RESULT_OK = 0,
	PROTO_RESULT_INVALID_MSG = 1,
	PROTO_RESULT_UNKNOWN_MSG_TYPE = 2,

	PROTO_RESULT_NO_SUCH_COMMAND = 3,
	PROTO_RESULT_UNSUPPORTED_COMMAND = 4
} PROTO_PACKED protocol_result_code_t;

typedef struct
{
	protocol_result_code_t result_code;
} PROTO_PACKED protocol_result_t;

typedef union
{
	struct
	{
		uint16_t msg_magic;
		protocol_msg_type_t msg_type;

		union
		{
			protocol_cmd_t command;
			protocol_status_payload_t status;
			protocol_temp_payload_t temp;
			protocol_result_t result;
		} payload;
	} PROTO_PACKED;

	uint8_t data[32];
} PROTO_PACKED protocol_msg_t;

void protocol_msg_init(protocol_msg_t* msg);
void print_protocol_message(const protocol_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* RADIO_PROTOCOL_H_ */
