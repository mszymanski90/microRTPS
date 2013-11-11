/*
*
* DataBuffer.c - Data Buffer
*
* Copyright (C) 2013         Maciej Szyma�ski <mszymanski90@gmail.com>
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

#include "DataBuffer.h"

TopicBufferHandle CreateTopicBuffer(unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msg_length)
{
	TopicBufferHandle pom;
	portBASE_TYPE i;

	pom = pvPortMalloc(sizeof(TopicBuffer));
	pom->messages = pvPortMalloc(TPBUF_LENGTH * msg_length);
	pom->topicID = topicID;
	pom->sem_space_left = xSemaphoreCreateCounting(TPBUF_LENGTH, 0);
	pom->subscribersCount = 0;
	pom->last_write=0;

	for(i=0; i<TPBUF_LENGTH; i++)
	{
		pom->next_msg[i]=0;
	}

	return pom;
}

portBASE_TYPE DestroyTopicBuffer(TopicBufferHandle TBHandle)
{
	vSemaphoreDelete(TBHandle->sem_space_left);
	vPortFree(TBHandle->messages);
	vPortFree(TBHandle);

	return 0;
}

tMsg ReadTopicBuffer(TopicBufferHandle TBHandle, unsigned portBASE_TYPE* last_read_index)
{
	if(TBHandle->msgPendingReads[*last_read_index]>0) TBHandle->msgPendingReads[*last_read_index]--;
	(*last_read_index)=TBHandle->next_msg[*last_read_index];
	return TBHandle->messages[*last_read_index];
}

void WriteTopicBuffer(TopicBufferHandle TBHandle, tMsg msg)
{
	int i;

	// block if no space in buffer
	xSemaphoreTake(TBHandle->sem_space_left, (portTickType) 0);

	for(i=0; i<TPBUF_LENGTH; i++)
	{
		if(TBHandle->msgPendingReads[i]<=0)
		{
			// all apps subscribing this topic, read this message
			TBHandle->messages[i] = msg;
			TBHandle->msgPendingReads[i] = TBHandle->subscribersCount;
			TBHandle->next_msg[TBHandle->last_write] = i;
			TBHandle->last_write = i;
			TBHandle->next_msg[TBHandle->last_write] = 0;
			break;
		}
	}
}

void DBInit(DataBuffer* DBuffer)
{
	int i;

	for(i=0; i<DB_COUNT; i++)
	{
		DBuffer->tb = 0;
	}

	InitSubscribtMatrix(&DBuffer->sm);
}

/*
 * \brief Writes to topic buffer.
 */
void DBWrite(DataBuffer* DBuffer, unsigned portBASE_TYPE topicID, tMsg msg)
{
	WriteTopicBuffer(DBuffer->tb[topicID], msg);
	NewMsgInTopic(DBuffer->sm, topicID);
}

/*
 * \brief Reads from topic buffer.
 */
tMsg DBRead(DataBuffer* DBuffer, unsigned portBASE_TYPE topicID)
{
	xSemaphoreTake(DBuffer->sm.matrix[]);
	ReadTopicBuffer();
}

