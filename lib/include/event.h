/* Majik 4 server
 * Copyright (C) 1998, 1999  Majik Development Team <majik@majik.netti.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* This file contains defines for events called in receive_event()
 * in monster.c
 */

#ifndef __EVENT_H__
#define __EVENT_H__


#define E_ATTACKED        0

#define E_HELP_REQUEST    1

#define E_OBJECT_ENTER    2
#define E_OBJECT_LEAVE    3

#define E_BATTLE_IN_SIGHT 4

#endif __EVENT_H__
