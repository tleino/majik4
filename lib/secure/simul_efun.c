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

#include <mcp.h>
#include <list.h>
#include <dialog.h>

int randseed;

void write_client (object, int, mixed);
void open_window (object, int, int, int, int, int, string, string, string);
varargs void message(int, int, int, int, string, mixed, mixed, mixed);
mixed base_name(mixed);
int abs(int);
int round(float);
int d(string);
void debug(string);

string 
wrap (string str, int width, int indent)
{
   string buf = "", *s;
   int columns;
   
   if (THIS)
	 columns = THIS->query_columns();
   
   if (!columns)
	 columns = 78;
   
   columns -= 2;
   
   if ( !indent )
	 return sprintf("%-=*s", width || columns, str);
   
   if (indent)
	 {
		str = sprintf("%-=*s", width || columns, str);
		
		s = explode(str, "\n");
		
		for (int i=0;i<sizeof(s);i++)
		  buf += repeat_string (" ", indent) + s[i] + "\n";
		
		return buf;
	 }
   
   return sprintf("%s%-=*s", str[0..indent - 1], (width || 2) - indent, str[indent..]);
}

void
select_target (object client, string callback)
{
   if (stringp (callback))
	 {	
		callback = replace_string (callback, "\n", "");
		callback = replace_string (callback, " ", "_");
	 }
   else
	 callback = "0";
   
   write_client (client, CMD_TARGET, callback);
}

string
unarticle (string str)
{
   string tmp;
   
   if (!stringp(str) || str == "")
	 return str;
   if (sscanf(str, "the %s", tmp))
	 return tmp;
   else if (sscanf(str, "a %s", tmp))
	 return tmp;
   else if (sscanf(str, "an %s", tmp))
	 return tmp;
   else
	 return str;
}

varargs string
article (string str, object giver, object who)
{
   string first;
   
   if (!str || str == "")
	 str = "bug";
   
   str = unarticle(str);
   
   first = str[0..0];
   
   if (who)
	 {
		if (lower_case(str) != str) /* the item has a proper name */
		  return who->query_cap_name(giver ? giver : 0);
				
		if (who->is_unique())
		  return "the " + str;
	 }
   
   if (member_array(first, ({ "a", "e", "i", "o", "u" })) != -1)
	 return "an " + str;
   else
	 return "a " + str;
}

void
prompt_client (object o, int type, string command, string prompt)
{
   command = replace_string (command, " ", "_");
   prompt = replace_string (prompt, " ", "_");
   
   write_client (o, CMD_PROMPT, sprintf("%d %s %s", type, command, prompt));
}

void
client_open_dialog (object client, int type, mixed callback, mixed title,
					mixed text, mixed *args)
{
   string args_str = "";
   
   if (stringp (title))
	 {
		title = replace_string (title, "\n", "");
		title = replace_string (title, " ", "_");
	 }
   else
	 title = "0";
   
   if (stringp (text))
	 {
		text = replace_string (text, "\n", ";");
		text = replace_string (text, " ", "_");
	 }
   else
	 text = "0";
   
   if (stringp (callback))
	 {
	    callback = replace_string (callback, "\n", "");
		callback = replace_string (callback, " ", "_");
	 }
   else
	 callback = "0";
   
   if (arrayp(args))
	 {
		for (int i=0;i<sizeof(args);i++)
		  {
			 if (arrayp(args[i]))
			   {
				  for (int j=0;j<sizeof(args[i]);j++)
					{
					   args_str += "|" + replace_string (args[i][j], " ", "_");
					}
				  
				  args_str += ";";
			   }
			 else
			   {
				  args_str += ";" + replace_string (args[i], " ", "_");
			   }
		  }
	 }
   else
	 args_str = "0";
   
   write_client (client, CMD_DIALOG, sprintf ("%d %s %s %s %s",
											  type, callback, title, text,
											  args_str));
}

void
open_window (object o, int width, int height, int start_x, int start_y, int type, string title, string command, string data)
{
   title = replace_string (title, " ", "_");
   command = replace_string (command, " ", "_");
   data = replace_string (data, " ", "_");
   
   write_client (o, CMD_LIST, sprintf("%d %d %d %d %d %s %s %s",
									  width, height, start_x, start_y,
									  type, title, command, data) + "\n");
}

/* If given a string array, returns a string such as "red, green and blue" 
 * from the string array ({ "red", "green", "blue" }). If given an object
 * array, returns a similar string with the object names. */
string
enumerize(mixed *mixes)
{
   int i, j, size;
   string str = "";
   string last, seclast, laststr;
   mixed mix;
   
   size = sizeof(mixes);
   if(size == 0)
	 return 0;
   
   mix = mixes[size - 1];
   if(stringp(mix))
	 last = mix;
   else
	 last = mix->query_name();
   
   if(size == 1)
	 return last;
   
   if(size > 1)
	 {
		mix = mixes[size - 2];
		if(stringp(mix))
		  seclast = mix;
		else
		  seclast = mix->query_name();
		
		laststr = last +" and "+ seclast;
		if(size == 2)
		  return laststr;
		
		for(i=0,j=size-2;i<j;i++)
		  {
			 mix = mixes[i];
			 if(stringp(mix))
			   str += mixes[i]+", ";
			 else
			   str += mixes[i]->query_name()+", ";
		  }
		return str + laststr;
	 }
}

/* This function uses floats to calculate the error radius for the
 * flying objects so do not try to optimize this by using ints. */
int *
fly(int x1, int y1, int x2, int y2, int maxdistance, float bonuserr, object mapenv)
{
   int i, j;
   float bonus, x, y;
   int dx, dy;
   int tox, toy;
   int right, down;
      
   dx = x2 - x1;
   dy = y2 - y1;
   
   if(!dx && !dy)
	 return 0;
      
   if(x2 > x1)
	 {
		right = 1;
		tox = x1 + maxdistance;
	 }
   else
	 tox = x1 - maxdistance;
   
   if(y2 > y1)
	 {
		down = 1;
		toy = y1 + maxdistance;
	 }
   else
	 toy = y1 - maxdistance;
   
   bonus = bonuserr;
   
   if(abs(dx) >= abs(dy))
	 {
		/* More sideways than up/down */
		if(dx)
		  bonus += to_float(dy) / abs(dx);
		
		y = to_float(y1) + bonus;
		
		if(right)
		  {
			 for(i=x1+1,j=tox;i<j;i++)
			   {
				  if(!mapenv->is_clear(i, round(y)))
					{
					   return ({ i, round(y) });
					}
				  y += bonus;
			   }
			 tox = i - 1;
		  }
		else
		  {
			 for(i=x1-1,j=tox;i>j;i--)
			   {
				  if(!mapenv->is_clear(i, round(y)))
					{
					   return ({ i, round(y) });
					}
				  y += bonus;
			   }
			 tox = i + 1;
		  }
		toy = round(y - bonus);
	 }
   else 
	 {
		/* More up/down than sideways */
		if(dy)
		  bonus += to_float(dx) / abs(dy);
		
		x = to_float(x1) + bonus;
		
		if(down)
		  {
			 for(i=y1+1,j=toy;i<j;i++)
			   {
				  if(!mapenv->is_clear(round(x), i))
					{
					   return ({ round(x), i });
					}
				  x += bonus;
			   }
			 toy = i - 1;
		  }
		else
		  {
			 for(i=y1-1,j=toy;i>j;i--)
			   {
				  if(!mapenv->is_clear(round(x), i))
					{
					   return ({ round(x), i });
					}
				  x += bonus;
			   }
			 toy = i + 1;
		  }
		tox = round(x - bonus);
	 }
   return ({ tox, toy });
}

int
d (string str)
{
   int t, z, p, ret;
   
   if (sscanf(str, "%dd%d+%d", t, z, p) >= 2)
	 {
		for (int i=0;i<t;i++)
		  ret += random (z) + 1;
		
		ret += p;
		return ret;
	 }
   return 0;
}

void
write_client (object client, int command, mixed data)
{
   int datalen; 
   
   if (intp(data))
	 data = "" + data + "";
   
   data = replace_string (data, "\r", "");
   data = replace_string (data, "\n", "");
   
   data += "\n";
   datalen = strlen (data);
   
   if (client->query_offline())
	 write_file ("/data/player/"+client->query_true_name()[0..0]+"/"+client->query_true_name()+".msg", command + ":" + datalen + ":" + data);
   else if(PREV(0) == client && !ENV(client))
	 write(command + ":" + datalen + ":" + data);
   else
	 tell_object (client, command + ":" + datalen + ":" + data);
}

varargs int
getoid (object o)
{
	int id;
	
	if (!o)
		o = previous_object();
	
	sscanf (file_name(o), "%*s#%d", id);
	
	return id;
}

varargs void
message (int type, int x, int y, int radius, string msg, mixed where, 
		 mixed target, mixed exclude)
{
   string buf = "", str1 = "", str2 = "", str3 = "";
   object subject;
   
   msg = replace_string(msg, "\n", "");
   
   if (where && !arrayp(where) && where->is_areamap())
	 where = where->get_message_targets(x, y, radius);
   
   if (where && !arrayp(where))
	 where = ({ where });
   
  if (objectp(where))
	 where = ({ where });
   
   if (target && !arrayp(target))
	 target = ({ target });
   
   if (arrayp (where))
	 {
		for (int i=0;i<sizeof(where);i++)
		  {
			 if (exclude)
			   {
				  if (arrayp (exclude))
					{
					   if (member_array (where[i], exclude) > -1)
						 continue;
					}
				  
				  if (objectp (exclude))
					{
					   if (where[i] == exclude)
						 continue;
					}
			   }
			 
			 buf = msg;
			 
			 for (int j=1;j<sizeof(target)+1;j++)
			   {
				  
				  if (where[i] == target[j-1])
					{
					   if (strsrch(buf,"$n"+(j)) > -1 || strsrch(buf, "$N"+(j)) > -1 
						   || where[i] == subject)
						 {
							if (where[i] != subject)
							  {
								 subject = where[i];
								 buf = replace_string(buf, "$n"+(j), "you");
								 buf = replace_string(buf, "$N"+(j), "You");
								 buf = replace_string(buf, "}", "");
								 buf = replace_string(buf, "{", "");
							  }
							
							buf = replace_string(buf, "$t"+(j), "yourself");
							buf = replace_string(buf, "$T"+(j), "Yourself");
							
						 }
					   else
						 {
							buf = replace_string(buf, "$t"+(j), "you");
							buf = replace_string(buf, "$T"+(j), "You");
							
							while (sscanf(buf, "%s{%s}%s", str1, str2, str3) == 3)
							  {
								 if (str2 == "hum")
								   buf = str1 + "hums" + str3;
								 if (str2 == "die")
								   buf = str1 + "dies" + str3;
								 else
								   buf = str1 + pluralize(str2) + str3;
							  }
						 }
					   
					   buf = replace_string(buf, "$m"+(j)+"self", "$m"+(j)+"rself");
					   buf = replace_string(buf, "$M"+(j)+"self", "$M"+(j)+"rself");
					   buf = replace_string(buf, "$o"+(j), "your");
					   buf = replace_string(buf, "$O"+(j), "Your");
					   buf = replace_string(buf, "$e"+(j), "you");
					   buf = replace_string(buf, "$E"+(j), "You");
					   buf = replace_string(buf, "$m"+(j), "you");
					   buf = replace_string(buf, "$M"+(j), "You");
					   buf = replace_string(buf, "$s"+(j), "your");
					   buf = replace_string(buf, "$S"+(j), "Your");
					}
				  else
					{
					   if (target[j-1])
						 {
							buf = replace_string(buf, "$n"+(j), target[j-1]->query_name(where[i]) ? target[j-1]->query_name(where[i]) : "(null)");
							buf = replace_string(buf, "$N"+(j), target[j-1]->query_cap_name(where[i]) ? target[j-1]->query_cap_name(where[i]) : "(null)");
							buf = replace_string(buf, "$t"+(j), target[j-1]->query_name(where[i]) ? target[j-1]->query_name(where[i]) : "(null)");
							buf = replace_string(buf, "$T"+(j), target[j-1]->query_cap_name(where[i]) ? target[j-1]->query_cap_name(where[i]) : "(null");
							buf = replace_string(buf, "$o"+(j), target[j-1]->query_name(where[i])+"'s" ? target[j-1]->query_name(where[i])+"'s" : "(null");
							buf = replace_string(buf, "$O"+(j), target[j-1]->query_cap_name(where[i])+"'s" ? target[j-1]->query_cap_name(where[i])+"'s" : "(null)");
							buf = replace_string(buf, "$e"+(j), target[j-1]->query_pronoun() ? target[j-1]->query_pronoun() : "(null)");
							buf = replace_string(buf, "$E"+(j), target[j-1]->query_pronoun() ? capitalize(target[j-1]->query_pronoun()) : "(null)");
							buf = replace_string(buf, "$m"+(j), target[j-1]->query_objective() ? target[j-1]->query_objective() : "(null)");
							buf = replace_string(buf, "$M"+(j), target[j-1]->query_objective() ? capitalize(target[j-1]->query_objective()) : "(null)");
							buf = replace_string(buf, "$s"+(j), target[j-1]->query_possessive() ? target[j-1]->query_possessive() : "(null)");
							buf = replace_string(buf, "$S"+(j), target[j-1]->query_possessive() ? capitalize(target[j-1]->query_possessive()) : "(null)");
							buf = replace_string(buf, "$p"+(j), target[j-1]->query_short(where[i]) ? target[j-1]->query_short(where[i]) : "(null)");
							buf = replace_string(buf, "$P"+(j), target[j-1]->query_short(where[i]) ? capitalize(target[j-1]->query_short(where[i])) : "(null)");
						 }
					   
					   
					   while (sscanf(buf, "%s{%s}%s", str1, str2, str3) == 3)
						 {
							if (str2 == "hum")
							  buf = str1 + "hums" + str3;
							if (str2 == "die")
							  buf = str1 + "dies" + str3;
							else
							  buf = str1 + pluralize(str2) + str3;
						 }
					}
			   }
			 				  
			 if (interactive(where[i]) || where[i]->query_offline())
			   {
				  switch (type)
					{
					 case CMD_SEE:
					 case CMD_HEAR:
					 case CMD_FEEL:
					 case CMD_SMELL:
					 case CMD_TASTE:
					 case CMD_SENSE:
					   if (where[i] && where[i]->query_uncons()) /* All senses are off while unconcious */
						 return;
					   break;
					}
				  write_client(where[i], type, x+" "+y+" "+radius+" "+capitalize(buf));
			   }
		  }
	 }
}

void
set_random_seed(int i)
{
   randseed = i;
}

int
randomi(int max)
{
   if (!randseed)
	 randseed = 1;
   randseed = (randseed * 9301 + 49297) % 233280;
   return to_int( max * (randseed / (233280 * -1.0)) );
}

object
get_location(int x, int y)
{
   object o, *u = children("/world/world");
   
   foreach ( o in u)
	 {
		if (o->query_xpos() == x && o->query_ypos() == y)
		  return o;
	 }
   
   o = clone_object("/world/world");
   o->set_xpos(x);
   o->set_ypos(y);
   o->generate_map();
   return o;
}

object
find_player (string str)
{
   object *obs = objects();
   
   if (!str)
	 return 0;
   
   foreach (object entry in obs)
	 {
		if (entry->query_true_name() && entry->is_player() && 
			lower_case(entry->query_true_name()) == lower_case(str))
		  return entry;
	 }
   
   return 0;
}

object
find_living (string str)
{
   object *obs = objects();
   
   if (!str)
	 return 0;
   
   foreach (object entry in obs)
	 {
		if (entry->query_true_name() && entry->is_living() && 
			lower_case(entry->query_true_name()) == lower_case(str))
		  return entry;
	 }   
   return 0;
}

mixed
base_name (mixed what)
{   
   string base;
   int tmp, uid;
   if (objectp(what))
	 tmp=sscanf(file_name(what),"%s#%d",base,uid);
   else if (stringp(what))
	 tmp=sscanf(what,"%s#%d",base,uid);
   else
	 tmp=what;
   
   if (tmp==2)
	 return base;
   else if (stringp(what))
	 return what;
   else if (objectp(what))
	 return file_name(what);
   else
	 return what;
}

void
log_file (string file, string line)
{
   /* TODO: This should include some system to limit certain log files
	*       to certain size, since we don't want some 100 mb log files,
	*       don't we?
	* 
	* PROPOSAL: Actually the filename for the log file should be determined
	*           by a previously defined integer, such as LOG_SYSTEM and thus,
	*           we would have slightly higher security and make it possible
	*           to receive message if certain log file has been updated more
	*           easily.
	*/
   
   write_file ("/log/" + file, line);
}

/* present() function eats coordinates and environment of the place where
 * we try to present() the object. If the object is not found then 0 is
 * returned, otherwise object is returned. Use -1 as a third argument if
 * you need to check the whole gridmap.
 * If no name is given all objects inside the radius will be returned.
 */

varargs mixed
present (object environment, string name, int x, int y, int radius)
{
   object o, *a;
   int x0, y0, i, j;
   
   if (!environment)
	 return 0;
   
   if (environment->is_areamap() && (x != -1))
	 {
		   a = ({ });
		   x0 = x - radius;
		   if (x0 < 0)
			 x0 = 0;
		   y0 = y - radius;
		   if (y0 < 0)
			 y0 = 0;
		   x += radius;
		   i = environment->query_area_w();
		   if (x > i)
			 x = i;
		   y += radius;
		   j = environment->query_area_h();
		   if (y > j)
			 y = j;

		if (radius < 20)
		  {
			 for (i = x0 ; i <= x ; i++)  
			   for (j = y0 ; j <= y ; j++)
				 a += environment->get_obj(i, j);
		  }
		else
		  {
			 a = environment->get_objects(x0, y0, x, y); 
		  }
	 }
   else
	 {	 
		a = all_inventory (environment);
	 }
   
   if (!a || sizeof(a) == 0)
	 return 0;

   if (!name || name == "")
	 return a;
   
   foreach (o in a)
	 {
		if (THIS)
		  {
			 if ((lower_case(o->query_name(THIS)) == lower_case(name)) ||
				 (lower_case(o->query_short(THIS)) == lower_case(name)))
			   return o;
		  }
		else
		  {
			 if ((lower_case(o->query_name()) == lower_case(name)) ||
				 (lower_case(o->query_short()) == lower_case(name)))
			   return o;
		  }
	 }
   return 0;
}

int
max(int i, int j)   /* Greater int of two */
{
   return i > j ? i : j;
}

int 
abs(int i)
{
   return i > 0 ? i : -i;
}

int 
round(float f)
{
   return to_int(f + 0.5);
}

void
debug(string str)
{
   message(CMD_MSG, 0,0,0, str+"", users());
}

int
distance(int x0, int y0, int x1, int y1)
{
   return max(abs(x0-x1), abs(y0-y1));
}
