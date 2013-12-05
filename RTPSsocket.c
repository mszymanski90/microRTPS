/*
*
* RTPSsocket.c - microRTPS socket
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

#include "RTPSsocket.h"

void RTPSsocketNewMessageInTopic(RTPSsocket* socket, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgID)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE topicIsSubscribed;
	MsgAddress msg_addr;

	topicIsSubscribed = 0;

	for(i=0; i<MAX_TOPICS; i++)
	{
		if(socket->subscribedTopics[i] == topicID)
		{
			topicIsSubscribed = 1;
			break;
		}
	}

	if(topicIsSubscribed)
	{
		// TODO: make msg_addr an argument of this function
		msg_addr.msgID = msgID;
		msg_addr.topicID = topicID;
		// push this message into message queue
		MsgQueueWrite(socket->msgQueue, msg_addr);
	}
}

void RTPSsocketInit(RTPSsocket* socket)
{

}

unsigned portBASE_TYPE RTPSsocketReceive(RTPSsocket* socket, void* msgBuf, portBASE_TYPE* topicID)
{
	MsgDoneReading(socket->mRTPS->topicBuffers[socket->addr.topicID], socket->addr.msgID);
	xSemaphoreTake(socket->semNewMsg);
	socket->addr = MsgQueueRead(socket->msgQueue);

	topicID = socket->addr.topicID;
	msgBuf = GetMsgFromTopicBuffer(socket->mRTPS->topicBuffers[socket->addr.topicID], socket->addr.msgID);

	return 0;
}

/* publishes from this socket */
unsigned portBASE_TYPE RTPSsocketPublish(RTPSsocket* socket, void* msgBuf, unsigned portBASE_TYPE topicID)
{

}

unsigned portBASE_TYPE RTPSsocketSubscribeByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID)
{
	// TODO: remember number of subscribents, use this number to set pending reads in topic buffer

}

unsigned portBASE_TYPE RTPSsocketUnsubscribeByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID)
{

}


