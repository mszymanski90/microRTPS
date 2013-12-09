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
		mRTPS->rxTopicBuffers = 0;
	}

	// for debug purposes
	mRTPS->topicList[0].subscribersCount = 0;
	mRTPS->topicList[0].topicID = 1904;


	emptyBufFound = 0;

	for(i=0; i<MAX_TOPICS; i++)
	{
		if(mRTPS->rxTopicBuffers == 0)
		{
			emptyBufFound=1;
			break;
		}
	}

	mRTPS->rxTopicBuffers = CreateTopicBuffer(mRTPS->topicList[0].topicID, sizeof(unsigned portBASE_TYPE));
	mRTPS->topicList[0].tpbuf_index = i;
}

void microRTPSWrite(microRTPS* mRTPS, void* msgBuf, unsigned portBASE_TYPE topicID)
{

	unsigned portBASE_TYPE msgID;

	socketListElem* elem = mRTPS->socketList;

	msgID = microRTPSWriteTpbufByTID(mRTPS, topicID, msgBuf);

	while(elem != 0)
	{
		RTPSsocketNewMessageInTopic(elem->container, topicID, msgID);
		elem = SLE_Next(elem);
	}
}

unsigned portBASE_TYPE microRTPSWriteTpbufByTID(microRTPS* mRTPS, unsigned portBASE_TYPE topicID, tMsg msg)
{
	return WriteTopicBuffer(&(mRTPS->rxTopicBuffers[microRTPS_FindTpbufByTopicID(mRTPS, topicID)]), msg);
}

unsigned portBASE_TYPE microRTPSAssertTopicIsSubscribed(microRTPS* mRTPS, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE tpbufID;

	tpbufID = microRTPS_FindTpbufByTopicID(mRTPS, topicID);

	// topicID not in database
	if(tpbufID == MAX_TOPICS)
	{
		// search for empty slot
		for(i=0; i<MAX_TOPICS; i++)
		{
			if(mRTPS->rxTopicBuffers[i] != 0)
			{
				if(mRTPS->rxTopicBuffers[i].topicID == 0)
				{
					tpbufID = i;
					break;
				}
			}
		}


		if(tpbufID == MAX_TOPICS)
		{
			// error: no space left
			return MAX_TOPICS;
		}
		else
		{
			mRTPS->rxTopicBuffers[tpbufID] = CreateTopicBuffer(topicID, msgLength);
		}
	}

	mRTPS->rxTopicBuffers[tpbufID].subscribersCount++;
	return tpbufID;
}

void microRTPSRegister(microRTPS* mRTPS, unsigned portBASE_TYPE name)
{
	// send REGISTER frame
}

unsigned portBASE_TYPE microRTPS_FindTpbufIndexByTopicID(microRTPS* mRTPS, unsigned portBASE_TYPE topicID)
{
	unsigned portBASE_TYPE i;

	for(i=0; i<MAX_TOPICS; i++)
	{
		if(mRTPS->rxTopicBuffers[i].topicID == topicID)
		{
			return i;
		}
	}

	return MAX_TOPICS;
}

