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

#include "rtnet.h"

#include "microRTPS_config.h"
#include "TopicBuffer.h"
#include "socketList.h"
#include "msgFIFO.h"
#include "protocol.h"

typedef struct sTID_name
{
	unsigned portCHAR name[MAX_TOPIC_NAME_LENGTH];
	unsigned portBASE_TYPE topicID;
	unsigned portBASE_TYPE msgLength;
	unsigned portBASE_TYPE registered;
} TID_name;

typedef struct smicroRTPS
{
	TopicBufferHandle TopicBuffers[MAX_TOPICS];
	socketListElem* socketList;
	MsgQueue txMsgQueue;
	xSemaphoreHandle txSem;

	xRTnetSocket_t RTnetSocket;
	xRTnetSockAddr_t broadcastAddr;

	unsigned portBASE_TYPE isMaster;
	TID_name* topicNameTable;
} microRTPS;

/*
 * \brief Initializes microRTPS structure.
 */
void microRTPS_Init(microRTPS* mRTPS, unsigned portBASE_TYPE isMaster);

void microRTPSRxTask(void *pvParameters);
void microRTPSTxTask(void *pvParameters);

/*
 * \brief Copies message to internal data base and notifies sockets that subscribe this topic. Queues msg for Tx if forTx is true.
 */
unsigned portBASE_TYPE microRTPSWrite(microRTPS* mRTPS, void* msgBuf, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE forTx);

/*
 * \brief Checks if topic is subscribed (if buffer marked with topicID exists).
 */
unsigned portBASE_TYPE microRTPSAssertBufferExists(microRTPS* mRTPS, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength);

void microRTPS_Register(microRTPS* mRTPS, unsigned portBASE_TYPE topicID, unsigned portBASE_TYPE msgLength);

void microRTPS_RegisterFrame(microRTPS* mRTPS, void* msgBuf);

/*
 * \brief Returns index of topicBuffer that contains corresponding topicID.
 */
unsigned portBASE_TYPE microRTPS_FindTpbufIndexByTopicID(microRTPS* mRTPS, unsigned portBASE_TYPE topicID);

void microRTPSCopyName(unsigned portCHAR* from, unsigned portCHAR* to);

#endif /* MICRORTPS_H_ */
