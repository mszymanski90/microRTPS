/*
*
* simulator.c - net stack simulator
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

#include "simulator.h"

void lSimInit(Simulator* sim)
{
	Msg1* pmsg1;
	Msg2* pmsg2;
	publish_frame_header* ppub_header;
	int i=0;

	sim->received_register = 0;
	sim->buffer_released = 1;

	sim->reg_frame_buf.header.protocolID = PROTO_ID;
	sim->reg_frame_buf.header.version = PROTO_VERSION;
	sim->reg_frame_buf.header.length = 0;
	sim->reg_frame_buf.header.frameType = FRAME_REGISTER;
	sim->reg_frame_buf.topicID = 0;

	for(i=0; i<32; i++)
	{
		sim->reg_frame_buf.topicName[i] = 0;
	}

	sim->reg_frame_buf.msgLength = 0;

	ppub_header = (publish_frame_header*) sim->tx_buffer1;
	pmsg1 = (Msg1*) sim->tx_buffer1 + sizeof(publish_frame_header);

	ppub_header->header.protocolID = PROTO_ID;
	ppub_header->header.version = PROTO_VERSION;
	ppub_header->header.length = sizeof(publish_frame_header) + sizeof(Msg1);
	ppub_header->header.frameType = FRAME_PUB;
	ppub_header->topicID = 1;
	pmsg1->num1 = 5;
	pmsg1->num2 = 13;

	ppub_header = (publish_frame_header*) sim->tx_buffer2;
	pmsg2 = (Msg2*) sim->tx_buffer2 + sizeof(publish_frame_header);

	ppub_header->header.protocolID = PROTO_ID;
	ppub_header->header.version = PROTO_VERSION;
	ppub_header->header.length = sizeof(publish_frame_header) + sizeof(Msg2);
	ppub_header->header.frameType = FRAME_PUB;
	ppub_header->topicID = 2;
	pmsg2->num1 = 7;
	pmsg2->num2 = 13;
	pmsg2->num2 = 4;
}

int32_t lSimRecvfrom(Simulator* sim, void** msgBuf, size_t xLen, uint32_t ulFlags)
{
	const portTickType xDelay = 3000 / portTICK_RATE_MS;
	int32_t frame_length;

	vTaskDelay(xDelay);

	if(ulFlags == RTNET_ZERO_COPY)
	{
		if(sim->buffer_released)
		{
			// if simulator received register frame earlier
			// respond with register frame, else send
			/*
			if(sim->received_register)
			{
				if(xLen >= sizeof(register_frame))
				{
					*msgBuf = &(sim->reg_frame_buf);
					frame_length = sizeof(register_frame);
				}
			}
			*/
			//else
			{
				// decide topic
				if(sim->toggle == 1)
				{
					//if(xLen >= sizeof(publish_frame_header) + sizeof(Msg1))
					{
						*msgBuf = sim->tx_buffer1;
						frame_length = sizeof(publish_frame_header) + sizeof(Msg1);
					}

					sim->toggle = 0;
				}
				else
				{
					//if(xLen >= sizeof(publish_frame_header) + sizeof(Msg2))
					{
						*msgBuf = sim->tx_buffer2;
						frame_length = sizeof(publish_frame_header) + sizeof(Msg2);
					}

					sim->toggle = 1;
				}

			}

			return frame_length;
		}
		else return -1;

	}
}

int32_t lSimSendto(Simulator* sim, void* pvBuf, size_t xLen, uint32_t ulFlags, xRTnetSockAddr_t *pxDestAddr)
{
	publish_frame_header* ppub_header;
	Msg1* pmsg1;
	Msg2* pmsg2;

	if(pxDestAddr->sin_addr == RTNET_IP_BROADCAST)
	{
		vSimCopyBuffer((char*) pvBuf, (char*) sim->rx_buffer, xLen);

		ppub_header = (publish_frame_header*) pvBuf;

		if(ppub_header->topicID == 1)
		{
			pmsg1 = (Msg1*) (pvBuf + sizeof(publish_frame_header));

			if(pmsg1->num1!=5) return 0;
			if(pmsg1->num2!=13) return 0;

			return 1;
		}
		else if(ppub_header->topicID == 2)
		{
			pmsg2 = (Msg2*) (pvBuf + sizeof(publish_frame_header));

			if(pmsg2->num1!=7) return 0;
			if(pmsg2->num2!=13) return 0;
			if(pmsg2->num3!=4) return 0;

			return 1;
		}
		else return 0;
	}
	else return 0;
}

void vSimReleaseUdpDataBuffer(Simulator* sim, void* msgBuf)
{

}

void vSimCopyBuffer(char* fromBuf, char* toBuf, size_t xLen)
{
	int i;

	for(i=0; i<xLen; i++)
	{
		*(toBuf + i) = *(fromBuf + i);
	}
}
