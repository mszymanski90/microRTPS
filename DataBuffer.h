/*
*
* sources/inc/DataBuffer.h - Data Buffer
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

#ifndef DATABUFFER_H_
#define DATABUFFER_H_

#include "FreeRTOS.h"

#define DB_COUNT 4
#define TPBUF_LENGTH 4

typedef void* tMsg;

typedef struct sTopicBuffer
{
	unsigned portBASE_TYPE topicID;
	tMsg* messages;
	unsigned portBASE_TYPE msgPendingReads;
	unsigned portBASE_TYPE subscribersCount;

	xSemaphoreHandle sem;
} TopicBuffer;

typedef TopicBuffer* TopicBufferHandle;

TopicBufferHandle CreateTopicBuffer(unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msg_length);
portBASE_TYPE DestroyTopicBuffer(TopicBufferHandle TBHandle);
void ReadTopicBuffer(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index);
void WriteTopicBuffer(TopicBufferHandle TBHandle, tMsg msg);

typedef struct sDataBuffer
{
	TopicBuffer tb[DB_COUNT];
} DataBuffer;

void DBInit(void);
tMsg DBWrite(DataBuffer* DBuffer, unsigned portBASE_TYPE topicID, tMsg msg);
tMsg DBRead(DataBuffer* DBuffer, unsigned portBASE_TYPE topicID);
void CreateTopicBuffer(DataBuffer* DBuffer, unsigned portBASE_TYPE msg_length);

#endif /* DATABUFFER_H_ */
