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
#include <security.h>
#include <living.h>
#include <color.h>
#include <feeling.h>
#include <list.h>

#define RACE "human"
#define CLIENT_VERSION "0.05"
#define SERVER_VERSION 0

inherit LIVING;
inherit CHANNEL;

/* --- -- - variables - -- --- */

// string 	name, 				/* character's name */
string    password;				/* character's password */
int		  security;				/* 0 = mortal */
int       offline;              /* boolean */
string    start;                /* starting location */
mapping   inventory;            
nosave string fail_message;
nosave string *command_buffer;

/* Keyboard "alias" */
mixed   keyboardalias;

/* --- function prototypes --- */

void	setup(string);

/* --- -- - functions - -- --- */

int
set_offline (int i)
{
   if (i)
	 THOB->set_wimpy (85);
   else
	 THOB->set_wimpy (0);
   
   return offline = i;
}

int
query_offline ()
{
   return offline;
}

void
notify_fail (string str)
{
   fail_message = str;
}

void
create ()
{
   living::create();
}

int
flush ()
{
   int i;
   
   if (sizeof (command_buffer))
	 i = sizeof (command_buffer);

   command_buffer = 0;
   return i;
}
   
void
heart_beat ()
{
   int x, y;
   
   speed_count += 100000;
   
   if (speed_count <= THOB->query_speed())
	 return;
   
   // message (CMD_MSG, 0,0,0, "*** "+THOB->query_speed()+" "+THOB->query_were_in_battle()+"", THOB);
   
   speed_count = 0;
   ::heart_beat();
   
   if (!command_buffer || sizeof(command_buffer) == 0)
	 return;
      
   if (sscanf (command_buffer[0], "%d#%d", x, y) == 2) /* CMD_XYPOS */
	 {
		THOB->set_were_in_battle (0);
		
		if (MAPENV(THOB))
		  move(MAPENV(THOB), grid_x+x, grid_y+y);		
	 }
   else /* CMD_COMMAND */
	 THOB->force_us (command_buffer[0]);

   if (sizeof(command_buffer) > 1)
	 command_buffer = command_buffer[1..];
   else
	 command_buffer = 0;
}

void
setup (string str)
{
   set_name (str);
   
   if (!THOB->query_race())
	 {
		/* New character, let's select a race. */
		set_heart_beat (0);
		open_window (THOB, 0,0,0,0, LTYPE_SELECT, "Select which race?", "race", "human;elf;orc;troll;dwarf;");
		return;
	 }
   else
	 set_race(THOB->query_race());
  
   security = 100;
   set_physical (TRUE);
   set_heart_beat (1);
   purge_eq ();
	 
   if (start == "/world/misc/void")
	 start = "/world/worldmap";
   
   if(THOB->query_dead())
	 start = "/world/misc/hell";
   if(THOB->query_freeze())
	 THOB->set_freeze(THOB->query_freeze());
   else if (!start || start == "" || file_size(start + ".c") < 0)
	 start = "/world/worldmap";
   
   write_client(THOB, CMD_SET_LIGHT, TIME_D->query_sun_light());
   
   if (interactive(THOB) || offline == 0)
	 {
		offline = 0;
		message (CMD_OOC, 0, 0, 0, "DEBUG: ^c1$N1^c0 becomes interactive.", users(), THOB, THOB);
	 }
}

void
net_dead ()
{
   message (CMD_OOC, 0, 0, 0, "DEBUG: ^c1$N1^c0 loses the link.", users(), THOB, THOB);
   
   THOB->set_offline (1);
   OFFLINE_D->add_offline (THOB);
   
// if (ENV(THOB))
//   if (MAPENV(THOB))
//	   MAPENV(THOB)->object_exit(THOB);
//
// raw_move("/world/misc/void");
// THOB->quit ("net_dead");
}

varargs void
quit (string reason, int no_save)
{
   message (CMD_OOC, 0, 0, 0, "DEBUG: ^c1$N1^c0 is no longer interactive.", users(), THOB, THOB);
   
   if (!no_save)
	 THOB->save_me ();
      
   if (reason)
	 write_client (THOB, CMD_QUIT, reason);
   else
	 write_client (THOB, CMD_QUIT, "No reason was given by server.");
   
   OFFLINE_D->add_offline (THOB);
}

int
remove()
{  
   if (ENV(THOB))
	 if (MAPENV(THOB))
	   MAPENV(THOB)->object_exit(THOB);
   
   destruct (THOB);
   return 1;
}

void
save_me ()
{
   object o, *all;
      
   all = all_inventory (THOB);
   
   inventory = ([ ]);
   
   foreach (o in all)
	 {
		/* Actually, we need to save generic items too. If we burn out
		 * a torch, quit and re-enter, without saving it is back to
		 * condition that it where before burning it out.
		 * 
		 *   += Namhas
		 */
		
		/* if(o->is_unique())
		 *   inventory[file_name(o)] = o->save_map();
		 * else
		 *   inventory[getoid(o)] = o->query_generate();
		 */
		
		inventory[file_name(o)] = o->save_map();
	 }
   
   if (file_size ("/data") != -2)
	 mkdir ("/data");
   
   if (file_size ("/data/player") != -2)
	 mkdir ("/data/player");
   
   if (file_size ("/data/player/" + name[0..0]) != -2)
	 mkdir ("/data/player/" + name[0..0]);
   
   if (!ENV(THOB))
	 start = "/world/worldmap";
   else
	 start = base_name(MAPENV(THOB));
   
   save_object ("/data/player/" + name[0..0] + "/" + name);
   inventory = ([ ]);
}

void
restore_me (string str)
{
   int i, j;
   object ob;
   string *k;
   
   restore_object ("/data/player/" + str[0..0] + "/" + str);

   if (inventory)
	 {
		k = keys (inventory);
		
		for (i=0,j=sizeof(k);i<j;i++)
		  {
			 /* OIDs are always of type int while filenames are of type string */

			 if (inventory[k[i]]["generate"])
			   {				  
				  ob = GENERIC_D->generate (restore_variable(inventory[k[i]]["generate"])[0],
											restore_variable(inventory[k[i]]["generate"])[1],
											restore_variable(inventory[k[i]]["generate"])[2]);
				  
				  ob->restore_map (inventory[k[i]]);
			   }
			 else
			   {
				  ob = clone_object(base_name(k[i]));
				  ob->restore_map(inventory[k[i]]);
				  
				  ob->create_item();
			   }
			 
			 ob->move(THOB,-1,-1);
		  }
	 }
   
   inventory = ([ ]);
   
   if (interactive(THOB))
	 offline = 0;

   /* If no alias is defined, define a default one */
   if(!keyboardalias)
     {
		log_file("log",file_name(this_object())+"Default alias is loaded\n");
		keyboardalias = ([
'>' : "down",
'<' : "up",
'o' : "open",
'c' : "close",
'q' : "quit",
'8' : "0#-1",
'2' : "0#1",
'4' : "-1#0",
'6' : "1#0",
'7' : "-1#-1",
'9' : "1#-1",
'1' : "-1#1",
'3' : "1#1",
'y' : "-1#-1",
'k' : "0#-1",
'u' : "1#-1",
'h' : "-1#0",
'l' : "1#0",
'b' : "-1#1",
'j' : "0#1",
'n' : "1#1",
'L' : "noop",
'd' : "drop",
',' : "get",
'i' : "i",
'g' : "give",
'?' : "help",
'5' : "noop",
'w' : "wield",
'W' : "wear",
'r' : "remove",
'C' : "chat",
'S' : "skills",
't' : "throw",
's' : "shoot",
'a' : "apply",
'N' : "name",
]);
     }
   
}

/*
string
set_name (string str)
{
   return name = str;
}
*/

string
set_password (string str)
{
   return password = str;
}
   
string
write_prompt ()
{
	return "";
}

string
process_input (string str)
{
   int command, datalen, *dxy;
   int log_command, log_datalen;
   mixed data, log_data;
   string *file;
   object env;
   write_file("/log/log", str+"\n");
   if (str && sscanf (str, "%d:%d:%s", command, datalen, data) == 3)
	 { 
		switch (command)
		  {
		   case CMD_START:
			 if (!data || (data != CLIENT_VERSION && data != "-1"))
			   THOB->quit ("Your client version is incompatible with the server. The current version is "+CLIENT_VERSION+". Please upgrade your client.");
			 
			 if (data != "-1") /* Yes, we are the client. */
			   THOB->set_offline (0);
			 			 
			 if (!THOB->query_race())
			   return "";
			   
			 if (!grid_x && !grid_y)
			   {  
				  /* Hilltop intersection */
				  grid_x = 2627;
				  grid_y = 5527;
				  
				  env = find_object(start) ? find_object(start) : load_object(start);
				  				  
				  if (env)
					dxy = env->get_near_location(grid_x, grid_y, 5);
				  
				  move(start, dxy[0], dxy[1]);
			   }
			 else
			   {
				  env = find_object(start) ? find_object(start) : load_object(start);
				  
				  if (env)
					{
					   if (!env->is_clear(grid_x, grid_y))
						 {
							dxy = env->get_near_location(grid_x, grid_y, 5);
							move (start, dxy[0], dxy[1]);
						 }
					   else
						 move (start, grid_x, grid_y);
					}
				  else
					move (start, grid_x, grid_y);
			   }
			 
			 if (file_size ("/data/player/"+THOB->query_true_name()[0..0]+"/"+THOB->query_true_name()+".msg") > 0)
			   {
				  file = explode (read_file ("/data/player/"+THOB->query_true_name()[0..0]+"/"+THOB->query_true_name()+".msg"), "\n");
				  
				  for (int i=0;i<sizeof(file);i++)
					{
					   if (sscanf (file[i], "%d:%d:%s", log_command, log_datalen, log_data) == 3)
						 {							
							if (log_command == CMD_REBOOT || log_command == CMD_SHUTDOWN || log_command == CMD_CRASH || log_command == CMD_QUIT)
							  {
								 message (CMD_MSG, 0, 0, 0, "** " + log_data, THOB);
								 continue;
							  }
							
							tell_object (THOB, file[i] + "\n");
						 }
					}
				  
				  rm ("/data/player/"+THOB->query_true_name()[0..0]+"/"+THOB->query_true_name()+".msg");
			   }
				  
			 break;
		   case CMD_XYPOS:
		   case CMD_COMMAND:
			 if (data && strlen (data))
			   {
				  /* List of the commands with no delay */
				  if (data == "blook" || data == "vlook" || data == "i")
				    THOB->force_us (data);
				  else if (strlen(data) > 3 && data[0..3] == "race")
					call_other ("/command/mortal/race", "main", data[5..]);
				  else if (command_buffer)
					command_buffer += ({ data });
				  else
					command_buffer = ({ data });
			   }
			 break;
		   case CMD_CHAR:
		     if(data && strlen(data))
		       {
			  string cmd;
			  if(!command_buffer)
			    command_buffer = ({ });
			  cmd = keyboardalias[data[0]];
			  if(cmd)
			    {
			       command_buffer += ({ cmd });
			       log_file("log","CHAR: "+data[0]+"cmd "+cmd+"\n");
			    }
			  else
			    {
			       log_file("log",sprintf("CHAR: %c unknown\n",data[0]));
			    }
			  break;
		       }
		     break;
		   case CMD_MAP:
//			 if (data && stringp(data) && ENV(THIS))
//			   {
//				  if (!arrayp(ENV(THIS)->query_gridmap()))
					ENV(THIS)->set_gridmap(explode(data, ";"));
//			   }
			 break;
		  }
	 }
   return str;
}

nomask int
query_security ()
{
   return security;
}

int
is_player ()
{
   return 1;
}
