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
		if(socket->subscribedTopics[i].topicID == topicID)
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
		MsgQueueWrite(&socket->msgQueue, msg_addr);

		xSemaphoreGive(socket->semNewMsg);
	}
}

void RTPSsocketInit(RTPSsocket* socket, microRTPS* mRTPS)
{
	unsigned portBASE_TYPE i;
	socket->semNewMsg = xSemaphoreCreateCounting(MSG_QUEUE_LENGTH, 0);
	socket->mRTPS = mRTPS;

	for(i=0; i<MAX_TOPICS; i++)
	{
		socket->subscribedTopics[i].topicID=0;
		socket->subscribedTopics[i].tpbufID=MAX_TOPICS;
	}

	MsgQueueInit(&(socket->msgQueue));

	SLE_Init(&(socket->listItem), socket);
	SLE_Push(&socket->mRTPS->socketList, &socket->listItem);
}

unsigned portBASE_TYPE RTPSsocketReceive(RTPSsocket* socket, void** msgBuf, unsigned portBASE_TYPE* topicID)
{
	MsgDoneReading(socket->mRTPS->TopicBuffers[socket->addr.topicID], socket->addr.msgID);

	xSemaphoreTake(socket->semNewMsg, portMAX_DELAY);
	socket->addr = MsgQueueRead(&socket->msgQueue);

	*topicID = socket->addr.topicID;
	*msgBuf = GetMsgFromTopicBuffer(socket->mRTPS->TopicBuffers[socket->addr.topicID], socket->addr.msgID);

	return 1;
}

unsigned portBASE_TYPE RTPSsocketPublish(RTPSsocket* socket, void* msgBuf, unsigned portBASE_TYPE topicID)
{
	unsigned portBASE_TYPE tpbufID;
	unsigned portBASE_TYPE msgID;
	tpbufID = microRTPS_FindTpbufIndexByTopicID(socket->mRTPS, topicID);


	microRTPSWrite(socket->mRTPS, msgBuf, topicID, 1);
}

unsigned portBASE_TYPE RTPSsocketSubscribeByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE tpbufID;

	tpbufID = microRTPSAssertTopicIsSubscribed(socket->mRTPS, topicID, msgLength);

	if(tpbufID < MAX_TOPICS)
	{
		for(i=0; i<MAX_TOPICS; i++)
		{
			if(socket->subscribedTopics[i].topicID == 0)
			{
				socket->subscribedTopics[i].topicID = topicID;
				socket->subscribedTopics[i].tpbufID = tpbufID;
				break;
			}
		}

		return 1;
	}
	else return 0;
}

unsigned portBASE_TYPE RTPSsocketUnsubscribeByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID)
{
	// TODO: search for matching topicID in subscribedTopics
	// TODO: insert 0 in place of found match
	// TODO: decrement subscribers count in tpbuf
}

unsigned portBASE_TYPE RTPSsocketRegisterTopic(RTPSsocket* socket, unsigned portBASE_TYPE name)
{

}

