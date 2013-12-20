/*
*
* microRTPS_config.h - configuration file
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

#ifndef MICRORTPS_CONFIG_H_
#define MICRORTPS_CONFIG_H_

// user settings
#define MAX_TOPICS 4
// recommended minimum is 2
#define TPBUF_LENGTH 4
#define MAX_APPS 2

//
#define MSG_QUEUE_LENGTH TPBUF_LENGTH*MAX_TOPICS

#endif /* MICRORTPS_CONFIG_H_ */
