/*
*
* protocol.h - protocol
*
* Copyright (C) 2013         Maciej Szymañski <mszymanski90@gmail.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*/

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>

#define PROTO_ID 0x1dfd
#define PROTO_VERSION 0x0001

#define FRAME_PUB 0x00
#define FRAME_REGISTER 0x01

struct sheader
{
	uint16_t protocolID;
	uint16_t version;
	uint32_t length;
	uint8_t frameType;
};
typedef struct header header_t;

struct spublish_frame
{
	header_t header;
	uint32_t topicID;
};
typedef struct spublish_frame publish_frame_header;

struct sregister_frame
{
	header_t header;
	uint32_t topicID;
	uint8_t topicName[32];
	uint32_t msgLength;
};
typedef struct sregister_frame register_frame;

#endif /* PROTOCOL_H_ */
