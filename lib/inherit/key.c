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

inherit ITEM;

int *door;

void
set_door (int x, int y)
{
   if (!door)
	 door = ({ x, y });
   else
	 door += ({ x, y });
}

int
test_door (int x, int y)
{
   if (!door)
	 return 0;
   
   for (int i=0;i<sizeof(door);i++)
	 if (door[i] == x && door[i] == y)
	   return 1;
   
   return 0;
}

int *
query_door ()
{
   return door;
}
