/*
*
* TopicBuffer.c - topic buffer
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

#include "TopicBuffer.h"

TopicBufferHandle CreateTopicBuffer(unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msg_length)
{
	TopicBufferHandle pom;
	unsigned portBASE_TYPE i;

	pom = NULL;
	pom = (TopicBufferHandle) pvPortMalloc(sizeof(TopicBuffer));

	pom->messages = NULL;
	pom->messages = pvPortMalloc(TPBUF_LENGTH * (sizeof(publish_frame_header) + msg_length));
	pom->topicID = topicID;
	pom->sem_space_left = xSemaphoreCreateCounting(TPBUF_LENGTH, TPBUF_LENGTH);
	pom->mutex = xSemaphoreCreateMutex();
	pom->subscribersCount = 0;
	pom->publishersCount = 0;
	pom->msg_length = msg_length;

	for(i=0; i<TPBUF_LENGTH; i++)
	{
		pom->msgPendingActions[i] = 0;
	}

	if(pom->sem_space_left != NULL && pom->mutex != NULL && pom->messages != NULL) return pom;
	else return NULL;
}

portBASE_TYPE DestroyTopicBuffer(TopicBufferHandle TBHandle)
{
	vSemaphoreDelete(TBHandle->sem_space_left);
	vPortFree(TBHandle->messages);
	vPortFree(TBHandle);

	return 0;
}

void GetFrameFromTopicBuffer(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index, tMsg* msgBuf)
{
	if(xSemaphoreTake(TBHandle->mutex, portMAX_DELAY) == pdTRUE)
	{
		*msgBuf = TBHandle->messages + msg_index * (sizeof(publish_frame_header) + TBHandle->msg_length);

		xSemaphoreGive(TBHandle->mutex);
	}
}

void GetMsgFromTopicBuffer(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index, tMsg* msgBuf)
{
	if(xSemaphoreTake(TBHandle->mutex, portMAX_DELAY) == pdTRUE)
	{
		*msgBuf = TBHandle->messages + msg_index * (sizeof(publish_frame_header) + TBHandle->msg_length) + sizeof(publish_frame_header);

		xSemaphoreGive(TBHandle->mutex);
	}
}

unsigned portBASE_TYPE GetMsgLengthFromTopicBuffer(TopicBufferHandle TBHandle)
{
	return TBHandle->msg_length;
}

unsigned portBASE_TYPE GetTopicIDFromTopicBuffer(TopicBufferHandle TBHandle)
{
	return TBHandle->topicID;
}

unsigned portBASE_TYPE MsgDoneReading(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index)
{
	if(xSemaphoreTake(TBHandle->mutex, portMAX_DELAY) == pdTRUE)
	{
		if(TBHandle->msgPendingActions[msg_index]>0) TBHandle->msgPendingActions[msg_index]--;

		// all subscribing apps have read this message; this slot is now avaible for writing
		if(TBHandle->msgPendingActions[msg_index]==0)
		{
				if(xSemaphoreGive(TBHandle->sem_space_left) != pdTRUE)
				{
					xSemaphoreGive(TBHandle->mutex);
					return 0;
				}
		}

		xSemaphoreGive(TBHandle->mutex);
		return 1;
	}
	else return 0;
}

unsigned portBASE_TYPE WriteTopicBuffer(TopicBufferHandle TBHandle, tMsg msg, unsigned portBASE_TYPE forTx)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE j;

	void* buffer_addr;



	// block if no space in buffer
	while(xSemaphoreTake(TBHandle->sem_space_left, portMAX_DELAY) != pdTRUE)
	{;}

	// make sure no one else is writing or reading from tpbuf
	if(xSemaphoreTake(TBHandle->mutex, portMAX_DELAY) == pdTRUE)
	{
		// debug delete this later
		if(TBHandle->messages == NULL)
		{
			xSemaphoreGive(TBHandle->mutex);
			return TPBUF_LENGTH;
		}

		for(i=0; i<TPBUF_LENGTH; i++)
		{
			if(TBHandle->msgPendingActions[i]<=0)
			{
				buffer_addr = (TBHandle->messages + i*(TBHandle->msg_length + sizeof(publish_frame_header)) + sizeof(publish_frame_header));

				for(j=0; j<TBHandle->msg_length; j++)
				{
					*((char*) (buffer_addr + j*(sizeof(char)))) = *((char*) (msg + j*(sizeof(char))));
				}

				TBHandle->msgPendingActions[i] = TBHandle->subscribersCount;

				// if message is to be transmitted, then there is one read more
				if(forTx) TBHandle->msgPendingActions[i]++;

				xSemaphoreGive(TBHandle->mutex);

				return i;
			}
		}

		xSemaphoreGive(TBHandle->mutex);
	}

	// error ??
	return TPBUF_LENGTH;
}

void TB_IncrementSubsCount(TopicBufferHandle TBHandle)
{
	TBHandle->subscribersCount++;
}

void TB_DecrementSubsCount(TopicBufferHandle TBHandle)
{
	// if subscribersCount && publishersCount both reach zero, delete buffer
	TBHandle->subscribersCount--;
}
