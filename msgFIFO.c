/*
*
* msgFIFO.h - message FIFO queue
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

#include "msgFIFO.h"

void MsgQueueInit(MsgQueue* msgQueue)
{
	msgQueue->full=0;
	msgQueue->in=0;
	msgQueue->out=0;
}

MsgAddress MsgQueueRead(MsgQueue* msgQueue)
{
	msgQueue->out++;
	if(msgQueue->out >= MSG_QUEUE_LENGTH) msgQueue->out = 0;
	if(msgQueue->in == msgQueue->out) msgQueue->full = 0;

	return msgQueue->queue[msgQueue->out - 1];
}

void MsgQueueWrite(MsgQueue* msgQueue, MsgAddress queueElem)
{
	if(!msgQueue->full)
	{
		msgQueue->queue[msgQueue->in] = queueElem;
		msgQueue->in++;

		if(msgQueue->in >= MSG_QUEUE_LENGTH) msgQueue->in = 0;
		if(msgQueue->in == msgQueue->out) msgQueue->full = 1;
	}
}

