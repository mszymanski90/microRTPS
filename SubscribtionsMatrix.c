/*
*
* SubscribtionsMatrix.c - Subscriptions matrix
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

void CreateSubscriptMatrix(SubscriptMatrixHandle SMHandle)
{
	int i;
	int j;

	for(i=0; i<MAX_TOPICS; i++)
	{
		for(j=0; j<MAX_APPS; j++)
		{
			xSemaphoreCreateCounting(SMHandle->matrix[i][j].sem_in_msg);
		}
	}
}

void DeleteSubscriptMatrix(SubscriptMatrixHandle SMHandle)
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

void NewMsgForApp(SubscriptMatrixHandle SMHandle, portBASE_TYPE topicID, portBASE_TYPE AppID)
{
	if(AppID <= MAX_APPS) xSemaphoreGive(SMHandle->matrix[topicID][AppID].sem_in_msg);
}
