/*
*
* TopicBuffer.h - topic buffer
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

#ifndef TOPICBUFFER_H_
#define TOPICBUFFER_H_

#include "microRTPS_config.h"
#include "protocol.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

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
	unsigned portBASE_TYPE msg_length;
	unsigned portBASE_TYPE subscribersCount;
	unsigned portBASE_TYPE publishersCount;

	tMsg messages;
	unsigned portBASE_TYPE msgPendingActions[TPBUF_LENGTH];


	xSemaphoreHandle sem_space_left;
	xSemaphoreHandle mutex;
} TopicBuffer;

typedef TopicBuffer* TopicBufferHandle;

/*
 * \brief Creates TopicBuffer and returns a handle to it.
 */
TopicBufferHandle CreateTopicBuffer(unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msg_length);

portBASE_TYPE DestroyTopicBuffer(TopicBufferHandle TBHandle);

/*
 * \brief Reads frame from Topic Buffer
 */
void GetFrameFromTopicBuffer(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index, tMsg* msgBuf);

/*
 * \brief Reads message from Topic Buffer
 */
void GetMsgFromTopicBuffer(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index, tMsg* msgBuf);

unsigned portBASE_TYPE GetMsgLengthFromTopicBuffer(TopicBufferHandle TBHandle);

/*
 * \brief Returns TopicID.
 */
unsigned portBASE_TYPE GetTopicIDFromTopicBuffer(TopicBufferHandle TBHandle);

/*
 * \brief Decrements msgPendingReads, Gives semaphore for writing.
 */
unsigned portBASE_TYPE MsgDoneReading(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index);

/*
 * \brief Writes to Topic Buffer.
 */
unsigned portBASE_TYPE WriteTopicBuffer(TopicBufferHandle TBHandle, tMsg msg, unsigned portBASE_TYPE forTx);

/*
 * \brief Increments subscribers count
 */
void TB_IncrementSubsCount(TopicBufferHandle TBHandle);

/*
 * \brief Decrements subscribers count
 */
void TB_DecrementSubsCount(TopicBufferHandle TBHandle);

#endif /* TOPICBUFFER_H_ */
