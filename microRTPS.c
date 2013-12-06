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

void microRTPSInit(microRTPS* mRTPS)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE emptyBufFound;

	for(i=0; i<MAX_TOPICS; i++)
	{
		mRTPS->topicBuffers = 0;
	}

	// for debug purposes
	mRTPS->topicList[0].subscribersCount = 0;
	mRTPS->topicList[0].topicID = 1904;


	emptyBufFound = 0;

	for(i=0; i<MAX_TOPICS; i++)
	{
		if(mRTPS->topicBuffers == 0)
		{
			emptyBufFound=1;
			break;
		}
	}

	mRTPS->topicBuffers = CreateTopicBuffer(mRTPS->topicList[0].topicID, sizeof(unsigned portBASE_TYPE));
	mRTPS->topicList[0].tpbuf_index = i;

}

void microRTPSWrite(microRTPS* mRTPS, void* msgBuf, unsigned portBASE_TYPE topicID)
{

	unsigned portBASE_TYPE subscribersCount;

	subscribersCount = mRTPS->topicList[microRTPS_FindTopicDescByTopicID(mRTPS, topicID);

	socketListElem* elem = mRTPS->socketList;
	// blocks on semaphore if there is no space left
	WriteTopicBuffer(&(mRTPS->topicBuffers[microRTPS_FindTpbufByTopicID(mRTPS, topicID)]), msgBuf, subscribersCount);

	while(elem != 0)
	{
		RTPSsocketNewMessageInTopic(elem->container, topicID);
		elem = SLE_Next(elem);
	}
}

void microRTPSAssertTopicIsSubscribed(microRTPS* mRTPS, unsigned portBASE_TYPE topicID)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE TopicIsSubscribed;

	TopicIsSubscribed = 0;

	for(i=0; i<MAX_TOPICS; i++)
	{
		if(mRTPS->topicList[i].topicID == topicID)
		{
			TopicIsSubscribed = 1;
			break;
		}
	}

	if(TopicIsSubscribed == 0)
	{
		// TODO: send subscribe frame to broker
	}
}

void microRTPSReceivedSubscribeReply(microRTPS* mRTPS, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE j;
	unsigned portBASE_TYPE emptyBufFound;
	unsigned portBASE_TYPE emptySlotFound;

	// find empty topic buffers slot
	// find empty topic list slot
	// write topicID, msgLength
	// TODO: write topic name to topic list

	emptyBufFound = 0;

	for(i=0; i<MAX_TOPICS; i++)
	{
		if(mRTPS->topicBuffers == 0)
		{
			emptyBufFound = 1;
			break;
		}
	}

	// find empty slot (topicID == 0)
	j = microRTPS_FindTpbufByTopicID(mRTPS, 0);

	mRTPS->topicBuffers[i] = CreateTopicBuffer(mRTPS->topicList[j].topicID, mRTPS->topicList[j].msgLength);
	mRTPS->topicList[j].tpbuf_index = i;
}

unsigned portBASE_TYPE microRTPS_FindTpbufByTopicID(microRTPS* mRTPS, unsigned portBASE_TYPE topicID)
{
	unsigned portBASE_TYPE j;
	unsigned portBASE_TYPE SlotFound;

	SlotFound = 0;

	for(j=0; j<MAX_TOPICS; j++)
	{
		if(mRTPS->topicList[j].topicID == topicID)
		{
			return mRTPS->topicList[j].tpbuf_index;
		}
	}
}

unsigned portBASE_TYPE microRTPS_FindTopicDescByTopicID(microRTPS* mRTPS, unsigned portBASE_TYPE topicID)
{
	unsigned portBASE_TYPE j;
	unsigned portBASE_TYPE SlotFound;

	SlotFound = 0;

	for(j=0; j<MAX_TOPICS; j++)
	{
		if(mRTPS->topicList[j].topicID == topicID)
		{
			return j;
		}
	}
}

