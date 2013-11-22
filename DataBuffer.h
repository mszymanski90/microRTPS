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
