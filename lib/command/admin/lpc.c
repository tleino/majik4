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
  string file, err;
  mixed ret;
  object ob;

  if (!str)
    {
      return 1;
    }

  file = "/_lpc.c";
  if(ob = find_object(file)) destruct(ob);
  rm (file);

  write_file (file, "mixed _lpc() { return " + str + "; }\n");

  err = catch(ret = (mixed) call_other(file, "_lpc"));
  if (err) 
    err = replace_string (err, "\n", " ");
   
  if(ob = find_object(file)) destruct(ob);
  
  message (CMD_MSG, 0,0,0, sprintf("= %O ", ret), THIS);
  rm (file);
  return 1;
}
