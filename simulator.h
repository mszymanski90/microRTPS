/*
*
* simulator.h - net stack simulator
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

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <stdint.h>
#include <FreeRTOS.h>
#include "protocol.h"
#include "msgStruct.h"

#define RTNET_ZERO_COPY      0x00000001
#define RTNET_IP_BROADCAST   0xFFFFFFFFUL
#define RTNET_IPPROTO_UDP    17

struct xRTNET_SOCKADDR
{
	uint32_t sin_addr;
	uint16_t sin_port;
};
typedef struct xRTNET_SOCKADDR xRTnetSockAddr_t;

struct sSimulator
{
	int32_t buffer_released;
	int32_t received_register;
	int32_t toggle;

	register_frame reg_frame_buf;
	char rx_buffer[48];
	char tx_buffer1[64];
	char tx_buffer2[64];
};
typedef struct sSimulator Simulator;

void lSimInit(Simulator* sim);
int32_t lSimRecvfrom(Simulator* sim, void** msgBuf, size_t xLen, uint32_t ulFlags);
int32_t lSimSendto(Simulator* sim, void* pvBuf, size_t xLen, uint32_t ulFlags, xRTnetSockAddr_t *pxDestAddr);
void vSimReleaseUdpDataBuffer(Simulator* sim, void* msgBuf);

void vSimCopyBuffer(char* fromBuf, char* toBuf, size_t xLen);

#endif /* SIMULATOR_H_ */
