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

int
main (string str)
{
   object o;
   
   if (!str)
	 {
		message (CMD_MSG, 0,0,0, "Usage: cc <filename>", PREV(0));
		return 1;
	 }
   
   o = find_object (str);
   
   if (o)
	 {
		destruct (o);
	    message (CMD_MSG, 0,0,0, str + " destructed.", PREV(0));
	 }
   		
   o = load_object (str);
   
   if (o)
	 message (CMD_MSG, 0,0,0, str + " compiled and loaded.", PREV(0));
   else
	 message (CMD_MSG, 0,0,0, str + " failed.", PREV(0));
				   
   return 1;
}
