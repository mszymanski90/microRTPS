/*
*
* sources/src/DataBuffer.c - Data Buffer
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

#include "DataBuffer.h"

TopicBufferHandle CreateTopicBuffer(unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msg_length)
{
	TopicBufferHandle pom;

	//pom = rt_malloc(sizeof(TopicBuffer));
	//pom->messages = rt_malloc(TPBUF_LENGTH * msg_length);
	pom->topicID = topicID;
	pom->sem = xCreateSemaphoreCounting(TPBUF_LENGTH, 0);
	pom->subscribersCount = 0;

	return pom;
}

portBASE_TYPE DestroyTopicBuffer(TopicBufferHandle TBHandle)
{

}

tMsg ReadTopicBuffer(TopicBufferHandle TBHandle, unsigned portBASE_TYPE msg_index)
{
	if(TBHandle->msgPendingReads[msg_index]>0) TBHandle->msgPendingReads[msg_index]--;
}

void WriteTopicBuffer(TopicBufferHandle TBHandle, tMsg msg)
{
	int i;

	// block if no space in buffer avaible
	xSemaphoreTake(TBHandle->sem, (portTickType) 0);

	for(i=0; i<TPBUF_LENGTH; i++)
	{
		if(TBHandle->msgPendingReads[i]<=0)
		{
			// all apps subscribing this topic, read this message
			TBHandle->messages[i] = msg;
			TBHandle->msgPendingReads[i] = TBHandle->subscribersCount;
			break;
		}
	}
}

void DBInit(void)
{
	sem = xSemaphoreCreateCounting();
}

/*
 * \brief Writes to topic buffer.
 */
tMsg DBWrite(DataBuffer* DBuffer, unsigned portBASE_TYPE topicID, tMsg msg)
{

}

/*
 * \brief Reads from topic buffer.
 */
tMsg DBRead(DataBuffer* DBuffer, unsigned portBASE_TYPE topicID)
{

}

TopicBuffer CreateMsgBuffer(DataBuffer* DBuffer, unsigned portBASE_TYPE msg_length)
{
	return malloc(TPBUF_LENGTH * msg_length);
}

