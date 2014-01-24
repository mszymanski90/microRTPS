/*
 * msgStruct.h
 *
 *  Created on: 23 sty 2014
 *      Author: Maciej
 */

#ifndef MSGSTRUCT_H_
#define MSGSTRUCT_H_

typedef struct sMsg1
{
	unsigned portBASE_TYPE num1;
	signed portCHAR num2;
} Msg1;

typedef struct sMsg2
{
	unsigned portBASE_TYPE num1;
	signed portCHAR num2;
	signed portCHAR num3;
} Msg2;

#endif /* MSGSTRUCT_H_ */
