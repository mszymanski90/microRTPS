/*
*
* microRTPS.h - microRTPS
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

#ifndef MICRORTPS_H_
#define MICRORTPS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "microRTPS_config.h"
#include "TopicBuffer.h"


typedef struct smicroRTPS
{
	TopicBufferHandle topicBuffers[MAX_TOPICS];
	xListItem socketList;
} microRTPS;

/*
 * \brief Initializes microRTPS structure.
 */
void microRTPSInit(microRTPS* mRTPS);

/*
 * \brief Copies message to internal data base and notifies sockets that subscribe this topic.
 */
void microRTPSNewMsgInTopic(microRTPS* mRTPS, void* msgBuf, unsigned portBASE_TYPE topicID);


#endif /* MICRORTPS_H_ */
