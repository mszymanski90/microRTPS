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
	portBASE_TYPE i;

	pom = pvPortMalloc(sizeof(TopicBuffer));
	pom->messages = pvPortMalloc(TPBUF_LENGTH * msg_length);
	pom->topicID = topicID;
	pom->sem_space_left = xSemaphoreCreateCounting(TPBUF_LENGTH, TPBUF_LENGTH);
	pom->subscribersCount = 0;

	return pom;
}

portBASE_TYPE DestroyTopicBuffer(TopicBufferHandle TBHandle)
{
	vSemaphoreDelete(TBHandle->sem_space_left);
	vPortFree(TBHandle->messages);
	vPortFree(TBHandle);

	return 0;
}

tMsg GetMsgFromTopicBuffer(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index)
{
	unsigned portBASE_TYPE msgLength;
	msgLength = TBHandle->msg_length;
	return TBHandle->messages + msg_index*msgLength;
}

unsigned portBASE_TYPE GetMsgLengthFromTopicBuffer(TopicBufferHandle TBHandle)
{
	return TBHandle->msg_length;
}

unsigned portBASE_TYPE GetTopicIDFromTopicBuffer(TopicBufferHandle TBHandle)
{
	return TBHandle->topicID;
}

void MsgDoneReading(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index)
{
	if(TBHandle->msgPendingActions[msg_index]>0) TBHandle->msgPendingActions[msg_index]--;

	// all subscribing apps have read this message; this slot is now avaible for writing
	if(TBHandle->msgPendingActions[msg_index]==0) xSemaphoreGive(TBHandle->sem_space_left);
}

unsigned portBASE_TYPE WriteTopicBuffer(TopicBufferHandle TBHandle, tMsg msg, unsigned portBASE_TYPE forTx)
{
	unsigned portBASE_TYPE i;
	unsigned portBASE_TYPE j;

	unsigned portCHAR *TPbuf;
	unsigned portCHAR *msgbuf;

	unsigned portBASE_TYPE* m;

	// block if no space in buffer
	xSemaphoreTake(TBHandle->sem_space_left, portMAX_DELAY);

	for(i=0; i<TPBUF_LENGTH; i++)
	{
		if(TBHandle->msgPendingActions[i]<=0)
		{
			if(TBHandle->messages == NULL) return TPBUF_LENGTH;

			TPbuf = (unsigned portCHAR *) (TBHandle->messages + i*(TBHandle->msg_length));
			msgbuf = (unsigned portCHAR *) msg;

			for(j=0; j<TBHandle->msg_length; j++)
			{
				*(TPbuf + j*(sizeof(unsigned portCHAR))) = *(msgbuf + j*(sizeof(unsigned portCHAR)));
			}

			m= (unsigned portBASE_TYPE*) (TBHandle->messages + i*(TBHandle->msg_length));
			if(*m != 1) return TPBUF_LENGTH;

			TBHandle->msgPendingActions[i] = TBHandle->subscribersCount;

			// if message is to be transmitted, then there is one read more
			if(forTx) TBHandle->msgPendingActions[i]++;

			return i;
		}
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
	TBHandle->subscribersCount--;
}
