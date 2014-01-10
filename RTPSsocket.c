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

unsigned portBASE_TYPE RTPSsocketNewMessageInTopic(RTPSsocket* socket, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgID)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE topicIsSubscribed;

	unsigned portBASE_TYPE msgID2;
	unsigned portBASE_TYPE tpbufID;

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
		// push this message into message queue
		MsgQueueWrite(&socket->msgQueue, socket->subscribedTopics[i].tpbufID, msgID);

		if(xSemaphoreGive(socket->semNewMsg) != pdTRUE) return 0;
		else return 1;
	}
	else return 1;
}

void RTPSsocketInit(RTPSsocket* socket, microRTPS* mRTPS)
{
	unsigned portBASE_TYPE i;
	socket->semNewMsg = xSemaphoreCreateCounting(MSG_QUEUE_LENGTH, 0);
	socket->mRTPS = mRTPS;
	socket->last_read.msgID=0;
	socket->last_read.tpbufID=0;
	socket->startup=1;

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
	unsigned portBASE_TYPE tpbufID;
	unsigned portBASE_TYPE msgID;

	*topicID=0;
	*msgBuf=NULL;

	if(socket->startup==1)
	{
		socket->startup = 0;
	}
	else
	{
		if(socket->last_read.tpbufID < MAX_TOPICS)
		{
			if(!MsgDoneReading(socket->mRTPS->TopicBuffers[socket->last_read.tpbufID], socket->last_read.msgID))
			{
				return 0;
			}
		}
		else return 0;
	}

	while(xSemaphoreTake(socket->semNewMsg, portMAX_DELAY) != pdTRUE)
	{;}

	MsgQueueRead(&socket->msgQueue, &tpbufID, &msgID);

	if(tpbufID < MAX_TOPICS && msgID < TPBUF_LENGTH)
	{
		*topicID = GetTopicIDFromTopicBuffer(socket->mRTPS->TopicBuffers[tpbufID]);
		GetMsgFromTopicBuffer(socket->mRTPS->TopicBuffers[tpbufID], msgID, msgBuf);

		socket->last_read.tpbufID = tpbufID;
		socket->last_read.msgID = msgID;
	}
	else
	{
		socket->last_read.tpbufID = MAX_TOPICS;
		return 0;
	}

	return 1;
}

unsigned portBASE_TYPE RTPSsocketPublish(RTPSsocket* socket, void* msgBuf, unsigned portBASE_TYPE topicID)
{
	microRTPSWrite(socket->mRTPS, msgBuf, topicID, 1);
}

unsigned portBASE_TYPE RTPSsocketSubscribeByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE space;
	unsigned portBASE_TYPE tpbufID;

	for(i=0; i<MAX_TOPICS; i++)
	{
		if(socket->subscribedTopics[i].topicID == 0)
		{
			space = i;
		}

		if(socket->subscribedTopics[i].topicID == topicID)
		{
			return 0; // topic already subscribed by this socket
		}
	}

	tpbufID = microRTPSAssertBufferExists(socket->mRTPS, topicID, msgLength);
	socket->mRTPS->TopicBuffers[tpbufID]->subscribersCount++;

	if(tpbufID < MAX_TOPICS && space < MAX_TOPICS)
	{
		socket->subscribedTopics[space].topicID = topicID;
		socket->subscribedTopics[space].tpbufID = tpbufID;
		return 1;
	}
	else return 0;
}

unsigned portBASE_TYPE RTPSsocketUnsubscribeByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID)
{
	unsigned portBASE_TYPE i;

	for(i=0; i<MAX_TOPICS; i++)
	{
		if(socket->subscribedTopics[i].topicID == topicID)
		{
			TB_DecrementSubsCount(socket->mRTPS->TopicBuffers[socket->subscribedTopics[i].tpbufID]);

			socket->subscribedTopics[i].topicID = 0;
			socket->subscribedTopics[i].tpbufID = MAX_TOPICS;
		}
	}

}

unsigned portBASE_TYPE RTPSsocketRegisterPublisherByTID(RTPSsocket* socket, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength)
{
	unsigned portBASE_TYPE tpbufID;

	tpbufID = microRTPSAssertBufferExists(socket->mRTPS, topicID, msgLength);
	socket->mRTPS->TopicBuffers[tpbufID]->publishersCount++;
}

