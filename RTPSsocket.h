/*
*
* RTPSsocket.h - microRTPS socket
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

#ifndef RTPSSOCKET_H_
#define RTPSSOCKET_H_

#include "FreeRTOS.h"
#include "list.h"

#include "microRTPS_config.h"

typedef struct sRTPSsocket
{
	// microRTPS* mRTPS;
	xListItem socket_list;
	xSemaphoreHandle semNewMsg;
	unsigned portBASE_TYPE subscribedTopics[MAX_TOPICS];
	msgFIFO msgQueue;
} RTPSsocket;

void RTPSsocketInit(RTPSsocket* socket);
unsigned portBASE_TYPE RTPSsocketRead(RTPSsocket* socket, void* msgBuf, portBASE_TYPE* topicID);
unsigned portBASE_TYPE RTPSsocketWrite(RTPSsocket* socket, void* msgBuf, portBASE_TYPE* topicID);
unsigned portBASE_TYPE RTPSsocketSubscribeByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID);
unsigned portBASE_TYPE RTPSsocketUnsubscribeByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID);

#endif /* RTPSSOCKET_H_ */
