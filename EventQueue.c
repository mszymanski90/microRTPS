/*
*
* sources/inc/EventQueue.c - Event Queue
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

#include "EventQueue.h"

/*
 * \brief Initializes EventQueue.
 */
void initEventQueue(EventQueue eq, unsigned portBASE_TYPE QueueLength)
{
	eq = xQueueCreate(sizeof());
}

/*
 * \brief Adds new event to end of queue.
 */
void newEvent(EventQueue EQueue, Event event);

/*
 * \brief Returns adress of event on top of the queue.
 */
Event* topEvent(EventQueue EQueue);

/*
 * \brief Erases event after service.
 */
void topEventServiced(EventQueue EQueue);
