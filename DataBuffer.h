/*
*
* DataBuffer.h - Data Buffer
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
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "microRTPS_config.h"
#include "SubscribtionsMatrix.h"


typedef void* tMsg;

/*
 * \brief Buffer for messages.
 *
 * Topic Buffer provides an interface
 * to reading and writing from message buffer.
 */
typedef struct sTopicBuffer
{
	unsigned portBASE_TYPE topicID;
	tMsg* messages;
	unsigned portBASE_TYPE msgPendingReads[TPBUF_LENGTH];
	unsigned portBASE_TYPE subscribersCount;

	xSemaphoreHandle sem_space_left;
} TopicBuffer;

typedef TopicBuffer* TopicBufferHandle;

/*
 * \brief Creates TopicBuffer and returns a handle to it.
 */
TopicBufferHandle CreateTopicBuffer(unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msg_length);

portBASE_TYPE DestroyTopicBuffer(TopicBufferHandle TBHandle);

/*
 * \brief Reads from Topic Buffer
 *
 * last_read_index- index of last read message in buffer,
 */
tMsg GetMsgFromTopicBuffer(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index);

/*
 * \brief Decrements msgPendingReads, Gives semaphore for writing.
 */
void MsgDoneReading(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index);

/*
 * \brief Writes to Topic Buffer.
 */
void WriteTopicBuffer(TopicBufferHandle TBHandle, tMsg msg);

typedef struct sDataBuffer
{
	TopicBufferHandle tb[MAX_TOPICS];
	tSubscribtMatrix sm;
} DataBuffer;

void DBInit(DataBuffer* DBuffer);
void DBWrite(DataBuffer* DBuffer, unsigned portBASE_TYPE topicID, tMsg msg);
tMsg DBRead(DataBuffer* DBuffer, unsigned portBASE_TYPE topicID);
void NewTopic(DataBuffer* DBuffer, unsigned portBASE_TYPE msg_length);

#endif /* DATABUFFER_H_ */
