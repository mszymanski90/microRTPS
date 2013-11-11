/*
*
* SubscribtionsMatrix.h - Subscribtions matrix
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

#ifndef SUBSCRIBTIONSMATRIX_H_
#define SUBSCRIBTIONSMATRIX_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "microRTPS_config.h"

typedef struct sMsgQueueElem
{
	unsigned portBASE_TYPE topicID;
	unsigned portBASE_TYPE index;
	unsigned portBASE_TYPE next_tID;
	unsigned portBASE_TYPE next_index;
} MsgQueueElem;

typedef struct sSubscribtMatrixElem
{
	xSemaphoreHandle sem_in_msg;
	portBASE_TYPE last_read;
} tSubscribtMatrixElem;

typedef struct sSubscribtMatrix
{
	tSubscribtMatrixElem matrix[MAX_TOPICS][MAX_APPS];
	MsgQueueElem next_msg[TPBUF_LENGTH*MAX_TOPICS];
} tSubscribtMatrix;

typedef tSubscribtMatrix* SubscribtMatrixHandle;

void InitSubscriptMatrix(SubscribtMatrixHandle SMHandle);
void DeleteSubscriptMatrix(SubscribtMatrixHandle SMHandle);
void NewMsgInTopic(SubscribtMatrixHandle SMHandle, portBASE_TYPE topicID);
void MsgReadByApp(SubscribtMatrixHandle SMHandle, portBASE_TYPE topicID, portBASE_TYPE AppID, portBASE_TYPE last_rd);

#endif /* SUBSCRIBTIONSMATRIX_H_ */
