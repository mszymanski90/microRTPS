/*
*
* socketList.c - socket list
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

#include "socketList.h"

void SLE_Init(socketListElem* elem, void* container)
{
	elem->container = container;
	elem->next = NULL;
	elem->prev = NULL;
}

void SLE_Push(socketListElem** first, socketListElem* new)
{
	if((*first) != NULL)
	{
		(*first)->prev = new;
		new->next = (*first);
	}
	else
	{
		new->next = NULL;
	}

	(*first) = new;
	new->prev = NULL;
}

socketListElem* SLE_Next(socketListElem* current)
{
	return current->next;
}

socketListElem* SLE_Prev(socketListElem* current)
{
	return current->next;
}

void SLE_Remove(socketListElem* removed)
{
	(removed->next)->prev = (removed->prev);
	(removed->prev)->next = (removed->next);

	removed->prev = NULL;
	removed->next = NULL;
}
