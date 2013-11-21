/*
*
* SubscribtionsMatrix.c - Subscribtions matrix
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

#include "SubscribtionsMatrix.h"

void InitSubscriptMatrix(SubscribtMatrixHandle SMHandle)
{
	int i;
	int j;

	for(i=0; i<MAX_TOPICS; i++)
	{
		for(j=0; j<MAX_APPS; j++)
		{
			SMHandle->matrix[i][j].sem_in_msg = xSemaphoreCreateCounting(MAX_TOPICS, 0);
			SMHandle->matrix[i][j].last_read = 0;
		}
	}

	for(i=0; i<(TPBUF_LENGTH*MAX_TOPICS); i++)
	{
		SMHandle->next_msg[i].topicID=0;
		SMHandle->next_msg[i].index=0;
		SMHandle->next_msg[i].point_next=0;
	}
}

void DeleteSubscriptMatrix(SubscribtMatrixHandle SMHandle)
{
	int i;
	int j;

	for(i=0; i<MAX_TOPICS; i++)
	{
		for(j=0; j<MAX_APPS; j++)
		{
			vSemaphoreDelete(SMHandle->matrix[i][j].sem_in_msg);
		}
	}
}

void NewMsgInTopic(SubscribtMatrixHandle SMHandle, unsigned portBASE_TYPE topicID)
{
	xSemaphoreGive(SMHandle->matrix[topicID][AppID].sem_in_msg);

	// add messages to app queues
}

void MsgReadByApp(SubscribtMatrixHandle SMHandle, unsigned portBASE_TYPE AppID)
{
	// inform topic buffer that message is read
	SMHandle->last_read = SMHandle->next_msg[AppID][SMHandle->last_read].point_next;
}

void GetNextMsg(SubscribtMatrixHandle SMHandle,
		portBASE_TYPE AppID,
		unsigned portBASE_TYPE* topicID,
		unsigned portBASE_TYPE* msgID)
{
	xSemaphoreTake(SMHandle->app_semaphores[AppID], (portTickType) 0);

	// zwrocic wskaznik na nastepna wiadomosc
	*topicID = SMHandle->next_msg[AppID][SMHandle->last_read].topicID;
	*msgID = SMHandle->next_msg[AppID][SMHandle->last_read].index;
}
