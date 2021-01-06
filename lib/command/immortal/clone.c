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
   int *dx;
   
   if (!str)
	 {
		PREV(0)->notify_fail ("Usage: clone <absolute path>");
		return 0;
	 }
   
   if (str[<2..<1] != ".c")
	 str += ".c";
   
   if (file_size (str) < 0)
	 {
		message (CMD_MSG, 0,0,0, str + ": No such file or directory.",
				 PREV(0));
		return 1;
	 }
   
   o = new (str);
   
   if (!o)
	 {
		message (CMD_MSG, 0,0,0, str + ": Could not clone.",
				 PREV(0));
		return 1;
	 }
   else
	 {
		if (o->query_physical())
		  {
			 dx = MAPENV(PREV(0))->get_near_location(PREV(0)->query_gridx(),
												  PREV(0)->query_gridy(),
												  2);
			 
			 if (dx[0] == 0 && dx[1] == 0)
			   {
				  message (CMD_MSG, 0,0,0, "There is no room for $p2.", PREV(0), o);
				  destruct (o);
				  return 1;
			   }
		  }
		else
		  {
			 dx = allocate(2);
			 
			 dx[0] = PREV(0)->query_gridx();
			 dx[1] = PREV(0)->query_gridy();
		  }
		
		o->move (file_name (MAPENV(PREV(0))), dx[0], dx[1]);
		
		message (CMD_SEE, PREV(0)->query_gridx(), PREV(0)->query_gridy(),
				 VISIBLE_RADIUS, "$N1 {snap} $s1 fingers and $p2 arrives in a puff of logic.",
				 MAPENV(PREV(0)), ({ PREV(0), o }));
		
		return 1;
	 }
   
   return 1;
}
