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

void microRTPS_Init(microRTPS* mRTPS, unsigned portBASE_TYPE isMaster, Simulator* sim)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE j;

	mRTPS->isMaster = isMaster;
	mRTPS->sim = sim;

	if(mRTPS->isMaster)
	{
		mRTPS->topicNameTable = pvPortMalloc(NAME_TABLE_LENGTH * sizeof(TID_name));

		for(i=0; i<NAME_TABLE_LENGTH; i++)
		{
			// all empty
			mRTPS->topicNameTable->topicID = 0;
			mRTPS->topicNameTable->msgLength = 0;
			mRTPS->topicNameTable->registered = 1;

			for(j=0; j<MAX_TOPIC_NAME_LENGTH; j++)
			{
				mRTPS->topicNameTable->name[j] = 0;
			}
		}
	}

	for(i=0; i<MAX_TOPICS; i++)
	{
		mRTPS->TopicBuffers[i] = NULL;
	}

	mRTPS->socketList = NULL;
	MsgQueueInit(&mRTPS->txMsgQueue);

	mRTPS->txSem = xSemaphoreCreateCounting(TPBUF_LENGTH, 0);

	//mRTPS->RTnetSocket = xRTnetSocket(RTNET_AF_INET, RTNET_SOCK_DGRAM, RTNET_IPPROTO_UDP);
	// TODO: make broadcast address a parameter
	mRTPS->broadcastAddr.sin_addr = RTNET_IP_BROADCAST;
	// TODO: make port a parameter
	mRTPS->broadcastAddr.sin_port = RTNET_IPPROTO_UDP;
	//xRTnetBind(mRTPS->RTnetSocket, &(mRTPS->broadcastAddr), 0);
}

void microRTPSRxTask(void *pvParameters)
{
	microRTPS* mRTPS;
	void* msgBuf;
	header_t* pFrameHeader;
	publish_frame_header* ppub_header;
	unsigned portBASE_TYPE topicID;
	int32_t frameLength;

	mRTPS = (microRTPS*) pvParameters;

	while(1)
	{
		frameLength = lSimRecvfrom(mRTPS->sim, (void**) &msgBuf, MAX_BUFFER_SIZE, RTNET_ZERO_COPY );

		pFrameHeader = (header_t*) msgBuf;
		if(pFrameHeader->protocolID == PROTO_ID) // protocol OK
			if(pFrameHeader->version == PROTO_VERSION) // version OK
				{
					switch(pFrameHeader->frameType)
					{
						case FRAME_PUB:
						{
							ppub_header = (publish_frame_header*) msgBuf;
							topicID = ppub_header->topicID;
							microRTPSWrite(mRTPS,
									(msgBuf+sizeof(publish_frame_header)),
									(unsigned portBASE_TYPE) topicID,
									0);
							vSimReleaseUdpDataBuffer(mRTPS->sim, msgBuf);
							break;
						}
						case FRAME_REGISTER:
						{
							microRTPS_RegisterFrame(mRTPS, msgBuf);
							break;
						}
						default:
						{
							break;
						}
					}
				}
	}

}

void microRTPSTxTask(void *pvParameters)
{
	microRTPS* mRTPS;
	void* msgBuf;
	publish_frame_header* ppub_header;

	unsigned portBASE_TYPE msgLength;
	unsigned portBASE_TYPE topicID;
	unsigned portBASE_TYPE tpbufID;
	unsigned portBASE_TYPE msgID;

	mRTPS = (microRTPS*) pvParameters;

	while(1)
	{
		if(xSemaphoreTake(mRTPS->txSem, portMAX_DELAY) == pdTRUE)
		{
			MsgQueueRead(&mRTPS->txMsgQueue, &tpbufID, &msgID);

			topicID = GetTopicIDFromTopicBuffer(mRTPS->TopicBuffers[tpbufID]);
			msgLength = GetMsgLengthFromTopicBuffer(mRTPS->TopicBuffers[tpbufID]);
			GetFrameFromTopicBuffer(mRTPS->TopicBuffers[tpbufID], msgID, &msgBuf);

			ppub_header = (publish_frame_header *) msgBuf;

			ppub_header->header.protocolID = PROTO_ID;
			ppub_header->header.version = PROTO_VERSION;
			ppub_header->header.length = msgLength;
			ppub_header->header.frameType = FRAME_PUB;
			ppub_header->topicID = topicID;


			if(!lSimSendto(mRTPS->sim, msgBuf, sizeof(publish_frame_header)+msgLength, 0, &mRTPS->broadcastAddr))
			{
				while(1){;}
			}

			MsgDoneReading(mRTPS->TopicBuffers[tpbufID], msgID);
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
		if(!RTPSsocketNewMessageInTopic(elem->container, topicID, msgID))
		{
			return 0;
		}
		elem = SLE_Next(elem);
	}


	if(forTx)
	{
		MsgQueueWrite(&mRTPS->txMsgQueue, tpbufID, msgID);
		xSemaphoreGive(mRTPS->txSem);
	}

	return 1;
}

unsigned portBASE_TYPE microRTPSAssertBufferExists(microRTPS* mRTPS, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength)
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
				break;
			}
		}


		if(i == MAX_TOPICS)
		{
			// error: no space left
			return MAX_TOPICS;
		}
		else
		{
			tpbufID = i;
			mRTPS->TopicBuffers[tpbufID] = CreateTopicBuffer(topicID, msgLength);
		}
	}

	if(mRTPS->TopicBuffers[tpbufID] == NULL) return MAX_TOPICS; // error

	// do that after this function
	//mRTPS->TopicBuffers[tpbufID]->subscribersCount++;

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

void microRTPS_RegisterFrame(microRTPS* mRTPS, void* msgBuf)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE j;
	unsigned portBASE_TYPE match;
	unsigned portBASE_TYPE assigned;
	register_frame* preg_frame;
	register_frame reg_frame;

	preg_frame = (register_frame*) msgBuf;

	if(mRTPS->isMaster)
	{
		if(preg_frame->topicID == 0)
		{
			for(i=0; i<NAME_TABLE_LENGTH; i++)
			{
				if(mRTPS->topicNameTable[i].registered)
				{
					match = 1;
					for(j=0; j<MAX_TOPIC_NAME_LENGTH; j++)
					{
						if(preg_frame->topicName[j] != mRTPS->topicNameTable[i].name[j])
						{
							match = 0;
							j = MAX_TOPIC_NAME_LENGTH;
						}
					}

					if(match == 1)
					{
						assigned = i;
						mRTPS->topicNameTable[i].registered++;
					}
				}
			}

			if(match == 0)
			{
				for(i=0; i<NAME_TABLE_LENGTH; i++)
				{
					if(mRTPS->topicNameTable[i].registered)
					{
						assigned = i;
						mRTPS->topicNameTable[i].registered = 1;
						mRTPS->topicNameTable[i].topicID = preg_frame->topicID;
						mRTPS->topicNameTable[i].msgLength = preg_frame->msgLength;
						microRTPSCopyName(preg_frame , &(mRTPS->topicNameTable[i]));
						break;
					}
				}
			}

			reg_frame.header.protocolID = PROTO_ID;
			reg_frame.header.version = PROTO_VERSION;
			reg_frame.header.length = 0;
			reg_frame.header.frameType = FRAME_REGISTER;
			reg_frame.topicID = mRTPS->topicNameTable[assigned].topicID;
			reg_frame.msgLength = mRTPS->topicNameTable[assigned].msgLength;
			microRTPSCopyName(preg_frame , &(reg_frame.topicName));

			// TODO: send only to master
			lSimSendto(mRTPS->sim, &reg_frame, sizeof(reg_frame), 0, &mRTPS->broadcastAddr);
		}
	}
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

void microRTPSCopyName(unsigned portCHAR* from, unsigned portCHAR* to)
{
	unsigned portBASE_TYPE i;

	for(i=0; i<NAME_TABLE_LENGTH; i++)
	{
		from[i] = to[i];
	}
}

