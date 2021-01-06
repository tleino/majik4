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

#include "mcp.h"

object
connect ()
{
	return clone_object ("/secure/login.c");
}

void
crash (string error, object command_giver, object current_object)
{
	object o, *u = users();
	
   log_file ("crash", ctime(time()) + " :: "+
			 sprintf("error=%s command_giver=%O current_object=%O\n",
					 error,command_giver, current_object));
   
	foreach (o in u)
		write_client (o, CMD_CRASH, 0);
}

string *
epilog ()
{
   /* Files to be reloaded belongs to the array below */
   return ({ "/daemon/time.c", "/daemon/weather.c", "/daemon/offline.c" });
}

void
preload (string file)
{
   call_other (file, "???");
}

void
error_handler (mapping err, int caught)
{
   string str;
   object o, *u = users();
   
   str = err["error"];
   
   if (!str)
	 return;
   
   str = replace_string (str, "\n", " ");
   str = str[1..];
   
   /* Echo it to ALL */
   
   foreach (o in u)
	 {
		message (CMD_MSG, 0,0,0, "** " + str + "", o);
		message (CMD_MSG, 0,0,0, "-- " + sprintf ("%s:%d OBJ(%O)", err["file"], err["line"], err["object"]) + "", o);
	 }
   
   log_file ("runtime.log", "** " + str + "\n");
   log_file ("runtime.log", "-- " + sprintf ("%s:%d OBJ(%O)", err["file"], err["line"], err["object"]) + "\n");
}
