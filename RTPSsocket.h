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
#include "microRTPS.h"
#include "msgFIFO.h"
#include "socketList.h"

typedef struct sSubscribtion
{
	unsigned portBASE_TYPE topicID;
	unsigned portBASE_TYPE tpbufID;
} Subscribtion;

typedef struct sRTPSsocket
{
	microRTPS* mRTPS;
	socketListElem listItem;
	xSemaphoreHandle semNewMsg;
	unsigned portBASE_TYPE inProcedure;

	Subscribtion subscribedTopics[MAX_TOPICS];
	MsgQueue msgQueue;
	MsgAddress addr;
} RTPSsocket;

/*
 * \brief Searches subscribed topics for matching topicID, new message in socket if success.
 */
void RTPSsocketNewMessageInTopic(RTPSsocket* socket, unsigned portBASE_TYPE topicID);

void RTPSsocketInit(RTPSsocket* socket, microRTPS* mRTPS);
unsigned portBASE_TYPE RTPSsocketReceive(RTPSsocket* socket, void* msgBuf, unsigned portBASE_TYPE* topicID);
unsigned portBASE_TYPE RTPSsocketPublish(RTPSsocket* socket, void* msgBuf, unsigned portBASE_TYPE topicID);

/*
 * \brief Adds topic to list of topics subscribed by socket (if TID is known).
 *
 * Note that topicID must be known by the system.
 * \retval 0 success
 * \retval 1 error
 */
unsigned portBASE_TYPE RTPSsocketSubscribeByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength);
unsigned portBASE_TYPE RTPSsocketUnsubscribeByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID);

unsigned portBASE_TYPE RTPSsocketRegisterTopic(RTPSsocket* socket, unsigned portBASE_TYPE name);

#endif /* RTPSSOCKET_H_ */
