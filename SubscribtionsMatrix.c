/*
*
* SubscribtionsMatrix.c - Subscribtions matrix
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
		SMHandle->next_msg[i].next_tID=0;
		SMHandle->next_msg[i].next_index=0;
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

void NewMsgInTopic(SubscribtMatrixHandle SMHandle, portBASE_TYPE topicID)
{
	int AppID;
	for(AppID=0; AppID<MAX_APPS; AppID++)
	{
		xSemaphoreGive(SMHandle->matrix[topicID][AppID].sem_in_msg);
	}
}

void MsgReadByApp(SubscribtMatrixHandle SMHandle, portBASE_TYPE topicID, portBASE_TYPE AppID, portBASE_TYPE last_rd)
{
	if(AppID <= MAX_APPS) SMHandle->matrix[topicID][AppID].last_read = last_rd;
}
