/*
*
* microRTPS.h - microRTPS
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

#include "microRTPS.h"

void microRTPS_Init(microRTPS* mRTPS)
{
	unsigned portBASE_TYPE i;

	for(i=0; i<MAX_TOPICS; i++)
	{
		mRTPS->TopicBuffers[i] = NULL;
	}

	mRTPS->socketList = NULL;
	MsgQueueInit(&mRTPS->txMsgQueue);

	// TODO: correct max count ?
	mRTPS->txSem = xSemaphoreCreateCounting(TPBUF_LENGTH, 0);
}

void microRTPSRxThread(microRTPS* mRTPS)
{
	MsgAddress msgAddr;
	void **msgBuf;
	unsigned portBASE_TYPE msgLength;
	unsigned portBASE_TYPE topicID;
	unsigned portBASE_TYPE msgID;

	while(1)
	{
		if(xSemaphoreTake(mRTPS->txSem, portMAX_DELAY) == pdTRUE)
		{
			MsgQueueRead(&mRTPS->txMsgQueue, &topicID, &msgID);

			msgLength = GetMsgLengthFromTopicBuffer(mRTPS->TopicBuffers[msgAddr.tpbufID]);
			msgBuf = GetMsgFromTopicBuffer(mRTPS->TopicBuffers[msgAddr.tpbufID], msgAddr.msgID);

			// send function

			MsgDoneReading(mRTPS->TopicBuffers[msgAddr.tpbufID], msgAddr.msgID);
		}
	}
}

unsigned portBASE_TYPE microRTPSWrite(microRTPS* mRTPS, void* msgBuf, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE forTx)
{
	unsigned portBASE_TYPE tpbufID;
	unsigned portBASE_TYPE msgID;

	socketListElem* elem = mRTPS->socketList;

	tpbufID = microRTPS_FindTpbufIndexByTopicID(mRTPS, topicID);

	if(tpbufID >= MAX_TOPICS) return 0;

	msgID = WriteTopicBuffer(mRTPS->TopicBuffers[tpbufID], msgBuf, forTx);

	if(msgID >= TPBUF_LENGTH) return 0;
	if(elem == NULL) return 0;

	while(elem != NULL)
	{
		if(!RTPSsocketNewMessageInTopic(elem->container, topicID, msgID)) return 0;
		elem = SLE_Next(elem);
	}


	if(forTx)
	{
		MsgQueueWrite(&mRTPS->txMsgQueue, tpbufID, msgID);
	}

	return 1;
}

unsigned portBASE_TYPE microRTPSAssertTopicIsSubscribed(microRTPS* mRTPS, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE tpbufID;

	tpbufID = microRTPS_FindTpbufIndexByTopicID(mRTPS, topicID);

	// topicID not in database
	if(tpbufID == MAX_TOPICS)
	{
		// search for empty slot
		for(i=0; i<MAX_TOPICS; i++)
		{
			if(mRTPS->TopicBuffers[i] == NULL)
			{
				/*
				if(mRTPS->TopicBuffers[i]->topicID == 0)
				{
					tpbufID = i;
					break;
				}
				*/

				tpbufID = i;
				break;
			}
		}


		if(tpbufID == MAX_TOPICS)
		{
			// error: no space left
			return MAX_TOPICS;
		}
		else
		{
			mRTPS->TopicBuffers[tpbufID] = CreateTopicBuffer(topicID, msgLength);
		}
	}

	mRTPS->TopicBuffers[tpbufID]->subscribersCount++;
	return tpbufID;
}

void microRTPS_Register(microRTPS* mRTPS, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength)
{
	unsigned portBASE_TYPE i;

	for(i=0; i<MAX_TOPICS; i++)
	{
		if(mRTPS->TopicBuffers[i] == NULL)
		{
			break;
		}
	}

	mRTPS->TopicBuffers[i] = CreateTopicBuffer(topicID, msgLength);
}

unsigned portBASE_TYPE microRTPS_FindTpbufIndexByTopicID(microRTPS* mRTPS, unsigned portBASE_TYPE topicID)
{
	unsigned portBASE_TYPE i;

	for(i=0; i<MAX_TOPICS; i++)
	{
		if(mRTPS->TopicBuffers[i] != NULL)
		{
			if(mRTPS->TopicBuffers[i]->topicID == topicID)
			{
				return i;
			}
		}
	}

	return MAX_TOPICS;
}

