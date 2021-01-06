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

#pragma optimize

inherit BASE;

#include <tile.h>
#include <event.h>

#define RIGHT          0
#define DOWN           1

#define T_SIDE         0
#define R_SIDE         1
#define B_SIDE         2
#define L_SIDE         3

#define CLIENT_RADIUS  25

/* --- -- - variables - -- --- */

int		areamap_w, areamap_h;			/* Worldmap size */
mapping areas;

/* --- function prototypes --- */

object	get_area(int, int);
object  *get_areas(int, int, int, int);
object  *get_players(int, int, int, int);
object  *get_livings(int, int, int, int);
object  *get_objects(int, int, int, int);
object  *get_obj(int, int);
int		add_area(int, int, string);
int		area_loaded(string);
void    update_client(object, int, int, int, int);
void    object_enter(object, object);
void    object_leave(object, object);

/* --- -- - functions - -- --- */


void 
create()
{
   areas = ([ ]);
   THOB->create_areamap();
}


int
add_area(int x, int y, string file)
{
		areas[sprintf("%dx%dy", x, y)] = file;
		return 1;
}

void
remove_area(object obj)
{
   map_delete(areas, 
			  sprintf("%dx%dy", obj->query_area_x(), obj->query_area_y()));

}

int
area_loaded(string file)
{
   object o;
   object *ob = all_inventory(THOB);
   if (file[<2..<1] == ".c")
	 file = file[0..<3];

   foreach (o in ob)
	 {
	 if (base_name(o) == file || file_name(o) == file)
	   return 1;
	 }
   
   return 0;
}

object
find_area(string f)
{
   object o;
   object *ob = all_inventory(THOB);
   
   if (f[<2..<1] == ".c")
	 f = f[0..<3];
   
   foreach (o in ob)
	 if (base_name(o) == f || file_name(o) == f)
	   return o;
   return 0;
}

int *
find_area_coords (string filename)
{
   string *k = keys(areas);
   int i, x, y;
   
   for (i = 0; i < sizeof(k); i++)
	 if (areas[k[i]] == filename)
	 {
		sscanf(k[i], "%dx%dy", x, y);
		break;
	 }
   if (i < sizeof(k))
	 return ({ x, y });
   return ({ -1, -1 });
}

int
build_map()
{
   return 0;
}

object
get_area(int x, int y)
{
   object o, *ob;
   string f;
   if (x < 0 || y < 0 || x >= areamap_w || y >= areamap_h)   
	 return 0;

   x = x/100;
   y = y/100;
   
   ob = all_inventory(THOB);
   
   f = sprintf("%dx%dy", x, y);
   if (areas[f])
	 {					/* load area from file */
		if (!area_loaded(areas[f]))
		  {
			 if (file_name(THOB) == "/world/worldmap")
			   o = THOB->build_map (x, y, areas[f]);
			 else
			   o = o = clone_object (areas[f]);
			 
			 o->set_area_coordinates(x, y);
			 o->move(THOB);
			 o->create_gridmap();
		  }
		else
		  return find_area(areas[f]);
	 }
   else
	 {			
		o = THOB->build_map(x, y);      /* This is for worldmap */
		if (!o) /* See build_map() functions above and in /world/worldmap.c */
		  return 0;
		o->set_area_coordinates(x, y);
		o->move(THOB);
		o->create_gridmap();
		add_area(x, y, file_name(o));
	 }
   return o;
}


int
query_tile(int x, int y)
{
   return THOB->get_area(x, y)->query_tile(x-(x/100*100), y-(y/100*100));
}

void
update_tile (int x, int y, string tile)
{
   object o, *u = get_players(x - CLIENT_RADIUS, y - CLIENT_RADIUS, 
							  x + CLIENT_RADIUS, y + CLIENT_RADIUS);
	 foreach (o in u)
	   write_client (o, CMD_TILE, sprintf ("%d %d %c", x, y, tile[0]));
   
   THOB->get_area(x, y)->update_tile(x - x/100*100, y - y/100*100, tile);
}

string *
query_map(int x0, int y0, int x1, int y1)
{
   string *buf;
   int lx, tx, ty, i, j, x, y, dx = x1-x0+1, dy = y1-y0+1;
   int changed;
   object env;
   
   buf = allocate(dy);
   for (y = 0; y < dy; y++)
	 buf[y] = repeat_string(" ", dx);
   
   env = THOB->get_area(x0, y0);
   y = y0 - y0/100*100;
   ty = y0;
   lx = x0 - x0/100*100;
   
   for (j = 0; j < dy; j++)
	 {
		x = lx;
		tx = x0;
		if (changed)
		  {
			 env = THOB->get_area(tx, ty);
			 changed = 0;
		  }
		
		for (i = 0; i < dx; i++)
		  {
			 buf[j][i] = env->query_tile(x, y);
			 x++;
			 if (x == 100)
			   {
				  tx += 100;
				  env = THOB->get_area(tx, ty);
				  x = 0;
				  changed = 1; 
			   }
		  }
		y++;
		if (y == 100)
		  {
			 ty += 100;
			 changed = 1;
			 y = 0;
		  }
	 }
   return buf;
}

string
query_map_slice(int x, int y, int dir, int lenght)
{
   int lx, ly, i;
   object env = THOB->get_area(x, y);
   string buf = repeat_string(" ", lenght);
   
   lx = x - x/100*100;
   ly = y - y/100*100;
   
   switch (dir)
	 {
	  case RIGHT:
		while (1)
		  {
			 buf[i] = env->query_tile(lx, ly);
			 i++;
			 if (i == lenght)
			   break;
			 lx++;
			 if (lx == 100)
			   {
				  x +=100;
				  env = THOB->get_area(x, y);
				  lx = 0;
			   }
		  }
		break;
	  case DOWN:
		while (1)
		  {
			 buf[i] = env->query_tile(lx, ly);
			 i++;
			 if (i == lenght)
			   break;
			 ly++;
			 if (ly == 100)
			   {
				  y += 100;
				  env = THOB->get_area(x, y);
				  ly = 0;
			   }
		  }
		break;
	  default:
		return 0;            /* Insert error handler here */
	 }
   
   return buf;
}

void
object_move (object obj, object penv, int tx, int ty)
{
   int x, y, cx, cy, dx, dy, sx, sy, lcx, lcy, ltx, lty;
   object cenv, tenv, o, *u;                 
   
   cx = obj->query_gridx();
   cy = obj->query_gridy();
   dx = abs(tx-cx);
   dy = abs(ty-cy);
   lcx = cx - cx/100*100;
   lcy = cy - cy/100*100;
   ltx = tx - tx/100*100;
   lty = ty - ty/100*100;
   
   tenv = ENV(obj);

   /* Update the player and living arrays in gridmaps */
   
   if (penv)
	 {
		cenv = ENV(penv);
		penv->remove_obj(lcx, lcy, obj);
		tenv->add_obj(ltx, lty, obj);
		
		if (penv != tenv)
		  {
			 if (obj->is_living())
			   {
				  penv->remove_living(obj);
				  tenv->add_living(obj);
			   }

			 if (interactive(obj))
			   {
				  penv->remove_player(obj);
				  tenv->add_player(obj);
			   }
		  }
	 }
   else
	 {
		tenv->add_obj(ltx, lty, obj);
		if (obj->is_living())
		  tenv->add_living(obj);
		if (interactive(obj))
		  tenv->add_player(obj);
	 }
   
   obj->set_gridpos(tx, ty);

   /* Send the upper left coords of the map to the client and disable map updating */
   
   if (interactive(obj))
	 {
		x = tx - CLIENT_RADIUS;
		if (x < 0)
		  x = 0;
		if (x > areamap_w)
		  x = areamap_w - (CLIENT_RADIUS * 2);
		y = ty - CLIENT_RADIUS;
		if (y < 0)
		  y = 0;
		if (y > areamap_h)
		  y = areamap_h - (CLIENT_RADIUS * 2);
		
		write_client(obj, CMD_LOCK_MAP, "");
		write_client(obj, CMD_MAP_COORD, sprintf("%d#%d", x , y));
	 }
   
   if ((dx > 20) || (dy > 20) || (cenv != ENV(tenv)))
	 {
		/* We want to send the entire map instead of a slice(s) */
		
		if (penv && penv->is_gridmap())    /* Leave from prev. env */
		  {
			 x = cx - CLIENT_RADIUS;
			 if (x < 0)
			   x = 0;
			 if (x > ENV(penv)->query_area_w() - 1 - CLIENT_RADIUS*2)
			   x = ENV(penv)->query_area_w() - 1 - (CLIENT_RADIUS * 2);
			 y = cy - CLIENT_RADIUS;
			 if (y < 0)
			   y = 0;
			 if (y > ENV(penv)->query_area_h() - 1 - CLIENT_RADIUS*2)
			   y = ENV(penv)->query_area_h() - 1 - (CLIENT_RADIUS * 2);
			 u = ENV(penv)->get_livings(x, y, x+(CLIENT_RADIUS * 2), y+(CLIENT_RADIUS * 2));
			 
			 foreach (o in u)
			   object_leave(obj, o);
			 object_leave(obj,obj);
		  }
		
		x = tx - CLIENT_RADIUS;
		if (x < 0)
		  x = 0;
		if (x > areamap_w - 1 - (CLIENT_RADIUS * 2))
		  x = areamap_w - 1 -(CLIENT_RADIUS * 2);
		y = ty - CLIENT_RADIUS;
		if (y < 0)
		  y = 0;
		if (y > areamap_h - 1 -(CLIENT_RADIUS * 2))
		  y = areamap_h - 1 -(CLIENT_RADIUS * 2);

		u = get_livings(x, y, x+(CLIENT_RADIUS * 2), y+(CLIENT_RADIUS * 2));

		/* Update near objects in the new env */
		 
		 foreach (o in u)
		  {
			 object_enter(obj, o);
		  }
		
		/* Update the client */ 
		
		if (interactive(obj))  /* FIXME: Add light level setting? */
		  {
			 write_client(obj, CMD_MAP, implode(query_map(x, y, x+(CLIENT_RADIUS * 2), y+(CLIENT_RADIUS * 2)), ";")+";");
			 update_client(obj, x, y, x+(CLIENT_RADIUS * 2), y+(CLIENT_RADIUS * 2));
//			 object_enter(obj, obj);
		  }
		else if (obj->is_living()) /* Not an interactive, still needs some info */
		  {
			 obj->clear_near_objects();
			 foreach(o in u)
			   {
				  object_enter(o, obj);
			   }
		  }
		
	 }
   
   else   /* We're sending slices */
	 {
		if (cy < CLIENT_RADIUS)
		  y = 0;
		else if (cy > areamap_h - 1 - CLIENT_RADIUS*2)
		  y = areamap_h - 1 - CLIENT_RADIUS*2;
		else
		  y = cy - CLIENT_RADIUS;
		
		/* Moving to west */
		while ((cx < tx) && (cx > 24))
		  {
			 cx++;
			 if ((x = cx+CLIENT_RADIUS) > areamap_w-1)
			   break;

			 if (interactive(obj)) /* Update the client */
			   {
				  write_client(obj, CMD_MAP_SLICE, sprintf("%d %s", R_SIDE, 
								    query_map_slice( x, y, DOWN, (CLIENT_RADIUS * 2 + 1))));
			 
				  /* Update the objects enterng the radius */
				  u = get_objects(x, y, x, y+(CLIENT_RADIUS * 2));
				  foreach( o in u)
					{
					   object_enter(o, obj);        
					   if (o->is_living())
						 object_enter(obj, o);
					}
				  
				  /* Update the objects leaving the radius */
				  u = get_objects(x-(CLIENT_RADIUS * 2)-1, y, x-(CLIENT_RADIUS * 2)-1, y+(CLIENT_RADIUS * 2));
				  foreach( o in u)
					{
					   object_leave(o, obj);        
					   if (o->is_living())
						 object_leave(obj, o);
					   
					}
			   }
			 
			 else /* So we're not an interactive */
			   {
				  u = get_livings(x, y, x, y+(CLIENT_RADIUS * 2));
				  if (obj->is_living()) /* A living mover wants info */
					{
					   foreach( o in u)
						 {
							object_enter(o, obj);
						 }
					}
				  
				  foreach( o in u)
					{
					   object_enter(obj, o);
					}
				  
				  u = get_livings(x-(CLIENT_RADIUS * 2)-1, y, x-(CLIENT_RADIUS * 2)-1, y+(CLIENT_RADIUS * 2));
				  foreach( o in u)
					{
					   object_leave(obj, o);
					}
				  
			   }
		  }
		
		/* Moving to east */
		while ((cx > tx) && cx < (areamap_w - CLIENT_RADIUS))
		  {
			 cx--;
			 if ((x = cx - CLIENT_RADIUS) < 0)
			   break;

			 if (interactive(obj))
			   {
				  write_client(obj, CMD_MAP_SLICE, sprintf("%d %s", L_SIDE, 
							   query_map_slice( x, y, DOWN, (CLIENT_RADIUS * 2 + 1))));
				  
				  u = get_objects(x, y, x, y+(CLIENT_RADIUS * 2));
				  foreach( o in u)
					{
					   object_enter(o, obj);
					   if (o->is_living())
						 object_enter(obj, o);
					}
				  
				  u = get_objects(x+(CLIENT_RADIUS * 2)+1, y, x+(CLIENT_RADIUS * 2)+1, y+(CLIENT_RADIUS * 2));
				  foreach( o in u)
					{
					   object_leave(o, obj);
					   if (o->is_living())          
						 object_leave(obj, o);
					}
			   }
			 else
			   {
				  u = get_livings(x, y, x, y+(CLIENT_RADIUS * 2));
				  if (obj->is_living())
					{
					   foreach( o in u)
						 {
							object_enter(o, obj);
						 }
					}
				  
				  foreach( o in u)
					{
					   object_enter(obj, o);
					}
				  
				  u = get_livings(x+(CLIENT_RADIUS * 2)+1, y, x+(CLIENT_RADIUS * 2)+1, y+(CLIENT_RADIUS * 2));
				  foreach( o in u)
					{
					   object_leave(obj, o);
					}
			   }
		  }

		if (cx < CLIENT_RADIUS)
		  x = 0;
		else if (cx > areamap_w - 1 - CLIENT_RADIUS*2)
		  x = areamap_w - 1 - CLIENT_RADIUS*2;
		else
		  x = cx - CLIENT_RADIUS;
		
		/* Moving to south */
		while ((cy < ty) && (cy > 24))
		  {
			 cy++;
			 if ((y = cy+CLIENT_RADIUS) > areamap_h-1)
			   break;

			 if (interactive(obj))
			   {
				  write_client(obj, CMD_MAP_SLICE, sprintf("%d %s", B_SIDE, 
					 query_map_slice( x, y, RIGHT, (CLIENT_RADIUS * 2 + 1))));

				  u = get_objects(x, y, x+(CLIENT_RADIUS * 2), y);
				  foreach( o in u)
					{
					   object_enter(o, obj);
					   if (o->is_living())
						 object_enter(obj, o);
					}

				  u = get_objects(x, y-(CLIENT_RADIUS * 2)-1, x+(CLIENT_RADIUS * 2), y-(CLIENT_RADIUS * 2)-1);
				  foreach( o in u)
					{
					   object_leave(o, obj);
					   if (o->is_living())          
						 object_leave(obj, o);
					}
			   }
			 else
			   { 
				  u = get_livings(x, y, x+(CLIENT_RADIUS * 2), y);
				
				  if (obj->is_living())
					{
					   foreach( o in u)
						 {
							object_enter(o, obj);
						 }
					}
				  
				  foreach( o in u)
					{
					   object_enter(obj, o);
					}
				  
				  u = get_livings(x, y-(CLIENT_RADIUS * 2)-1, x+(CLIENT_RADIUS * 2), y-(CLIENT_RADIUS * 2)-1);
				  foreach( o in u)
					{
					   object_leave(obj, o);
					}
			   }
		  }
		
		/* Moving to north */
		while ((cy > ty) && (cy < areamap_h -CLIENT_RADIUS))
		  {
			 cy--;
			 if ((y = cy - CLIENT_RADIUS) < 0)
			   break;
			 
			 if (interactive(obj))
			   {
				  write_client(obj, CMD_MAP_SLICE, sprintf("%d %s", T_SIDE, 
							   query_map_slice( x, y, RIGHT, (CLIENT_RADIUS * 2 + 1))));

				  u = get_objects(x, y, x+(CLIENT_RADIUS * 2), y);
				  foreach( o in u)
					{
					   object_enter(o, obj);
					   if (o->is_living())
						 object_enter(obj, o);
					}
			 
				  u = get_objects(x, y+(CLIENT_RADIUS * 2)+1, x+(CLIENT_RADIUS * 2), y+(CLIENT_RADIUS * 2)+1);
				  foreach( o in u)
					{
					   object_leave(o, obj);
					   if (o->is_living())          
						 object_leave(obj, o);
					}
			   }
			 else
			   {
				  u = get_livings(x, y, x+(CLIENT_RADIUS * 2), y);
				  if (obj->is_living())
					{
					   foreach( o in u)
						 {
							object_enter(o, obj);
						 }
					}
				  
				  foreach( o in u)
					{
					   object_enter(obj, o);
					}
				  
				  u = get_livings(x, y+(CLIENT_RADIUS * 2)+1, x+(CLIENT_RADIUS * 2), y+(CLIENT_RADIUS * 2)+1);
				  foreach( o in u)
					{
					   object_leave(obj, o);
					}
			   }
		  }
	
		if (cx > tx)
		  {
			 x = tx - CLIENT_RADIUS;
			 sx = cx + CLIENT_RADIUS;
		  }
		
		else
		  {  
			 x = cx - CLIENT_RADIUS;
			 sx = tx + CLIENT_RADIUS;
		  }
		
		if (x < 0)
		  x = 0;
		if (x > areamap_w)
		  x = areamap_w;
		if (sx < 0)
		  sx = 0;
		if (sx > areamap_w)
		  sx = areamap_w;
		
		if (cy > ty)
		  {
			 y = ty - CLIENT_RADIUS;
			 sy = cy + CLIENT_RADIUS;
		  }
		else
		  {
			 y = cy - CLIENT_RADIUS;
			 sy = ty + CLIENT_RADIUS;
		  }
		
		if (y < 0)
		  y = 0;
		if (y > areamap_h)
		  y = areamap_h;
		if (sy < 0)
		  sy = 0;
		if (sy > areamap_h)
		  sy = areamap_h;
		
		/* Updating the clients that stayed in the radius during the whole process */
		u = get_players(x, y, sx, sy);
		foreach(o in u)
		  {
			 write_client(o, CMD_MOVE_OBJECT,
						  sprintf("%d %d %d", getoid(obj), tx, ty)
						  );
		  }
		
	 }
   
   /* Enable client map updating and send client the player coordinates */
   if (interactive(obj))
	 {
		write_client(obj, CMD_RELEASE_MAP, "");
		write_client(obj, CMD_XYPOS, sprintf("%d#%d", tx, ty));
	 }
   
}

void
object_leave(object obj, object client)
{
   if (obj == client)
	 return;
   if (interactive(client))
	 {
		write_client(client, CMD_DEL_OBJECT,
					 sprintf("%d", getoid(obj))
					 );
	 }
   client->remove_near_object(obj);
   client->receive_event(E_OBJECT_LEAVE, obj);
}

void
object_enter(object obj, object client)
{

   if (interactive(client))
	 {
		write_client(client, CMD_ADD_OBJECT,
					 sprintf("%d", getoid(obj))
					 );
		if (client == obj)
		  write_client(client, CMD_OID_OWN, sprintf("%d", getoid(obj)));
		
		write_client(client, CMD_INFO_OBJECT,
					 sprintf("%d %d %d %d %d %d %s",
							 getoid(obj),
							 obj->query_gridchar(),
							 obj->query_gridcolor(),
							 obj->query_gridlight(),
							 obj->query_physical(),
							 obj->query_invis(),
							 obj->query_true_name()
							 )
					 );
		write_client(client, CMD_MOVE_OBJECT,
					 sprintf("%d %d %d", 
							 getoid(obj), 
							 obj->query_gridx(), 
							 obj->query_gridy()
							 )
					 );
	 }
   else
	 {
		if (obj == client)
		  return;
	 }
   client->add_near_object(obj);
   client->receive_event(E_OBJECT_ENTER, obj);
}

void
object_exit(object obj)
{
   int x, y;
   object o, *u;
   
   x = obj->query_gridx();
   y = obj->query_gridy();
   
   u = get_livings(x-CLIENT_RADIUS, y-CLIENT_RADIUS, x+CLIENT_RADIUS, y+CLIENT_RADIUS);

   foreach( o in u)
	 {
		  object_leave(obj, o);
	 }
   
   ENV(obj)->remove_obj(x - x/100*100, y - y/100*100, obj);
   if (obj->is_living())
	 ENV(obj)->remove_living(obj);
   if (interactive(obj))
	 ENV(obj)->remove_player(obj);
}

void  
object_update (object obj)
{
   int x, y;
   object o, *u;
   
   x = obj->query_gridx();
   y = obj->query_gridy();
	 
   u = get_players(x-CLIENT_RADIUS, y-CLIENT_RADIUS, x+CLIENT_RADIUS, y+CLIENT_RADIUS);
   
   foreach (o in u)
	 {
		write_client(o, CMD_INFO_OBJECT,
					 sprintf("%d %d %d %d %d %d %s",
							 getoid(obj),
							 obj->query_gridchar(),
							 obj->query_gridcolor(),
							 obj->query_gridlight(),
							 obj->query_physical(),
							 obj->query_invis(),
							 obj->query_true_name()
							 )
					 );
	 }
}



object * /* Get players in an area, because we have relatively few players users() should be quite fast */
get_players(int x0, int y0, int x1, int y1)
{
   int x, y;
   object o, *u, *players = ({});
   
   u = users();
   
   foreach (o in u)
	 {
		x = o->query_gridx();
		y = o->query_gridy();
		if ((x > x1) || (x < x0) || (y > y1) || (y < y0))
		  continue;
		players += ({ o });
	 }
   return players;
   
/*   Could this be faster with good get_area() and lots of ppl?
	object o, ob, *envs, *plr, *players = ({});
   int x, y;
   
   envs = get_areas(x0, y0, x1, y1);
   
   foreach (o in envs)
	 {
		plr = o->get_players();
		foreach(ob in plr)
		  {
			 x = ob->query_gridx();
			 y = ob->query_gridy();
			 if ((x > x1) || (x < x0) || (y > y1) || (y < y0))
			   continue;
			 players += ({ob});
		  }
	 }
   
   return players;

*/

}

object * /* Get all living in an area */
get_livings(int x0, int y0, int x1, int y1)
{
   object o, ob, *envs, *plr, *livings = ({});
   int x, y;

   if (x0 < 0)        
	 x0 = 0;
   if (y0 < 0)
	 y0 = 0;
   if (x1 > areamap_w-1)
	 x1 = areamap_w-1;
   if (y1 > areamap_h-1)
	 y1 = areamap_h-1;

   envs = get_areas(x0, y0, x1, y1);
   
   foreach (o in envs)
	 {
		plr = o->get_livings();
		foreach(ob in plr)
		  {
			 x = ob->query_gridx();
			 y = ob->query_gridy();
			 if ((x > x1) || (x < x0) || (y > y1) || (y < y0))
			   continue;
			 livings += ({ob});
		  }
	 }
   
   return livings;
   
}

object * /* Get objects in a sinlge grid */
get_obj(int x, int y)
{
   return THOB->get_area(x, y)->get_obj(x-x/100*100, y - y/100*100);
}


object * /* Get objects in an area */
get_objects(int x0, int y0, int x1, int y1)
{
   object o, ob, *envs, *u, *objects = ({});
   int x, y;
   
   envs = get_areas(x0, y0, x1, y1);
   foreach (o in envs)
	 {
		u = all_inventory(o);
		foreach(ob in u)
		  {
			 x = ob->query_gridx();
			 y = ob->query_gridy();
			 if ((x > x1) || (x < x0) || (y > y1) || (y < y0))
			   continue;
			 objects += ({ob});
		  }
	 }
   return objects;
}
			
object * /* Get gridmap objects that form the given area */
get_areas(int x0, int y0, int x1, int y1)
{
   int i, j;
   object *areas =  ({});

   x0 /= 100;
   y0 /= 100;
   x1 /= 100;
   y1 /= 100;
   
   for (j = y0; j <= y1; j++)
	 for (i = x0; i <= x1; i++)
	   areas += ({ get_area(i*100, j*100) });
   
   return areas;
}

void
update_client(object client, int x0, int y0, int x1, int y1)
{
   object *inv, o;
   
   inv = get_objects(x0, y0, x1, y1);

   /* Clear object list */
   write_client(client, CMD_DEL_OBJECTS, "");
   
   foreach (o in inv)
	 {
		/* Add object to list */
		write_client(client, CMD_ADD_OBJECT,
					 sprintf("%d", getoid(o))
					 );
		        if (client == o)
		  write_client(client, CMD_OID_OWN, sprintf("%d", getoid(o)));
		/* And update object info */
		write_client(client, CMD_INFO_OBJECT,
					 sprintf("%d %d %d %d %d %d %s",
							 getoid(o),
							 o->query_gridchar(),
							 o->query_gridcolor(),
							 o->query_gridlight(),
							 o->query_physical(),
							 o->query_invis(),
							 o->query_true_name()
							 )
					 );
		write_client(client, CMD_MOVE_OBJECT,
					 sprintf("%d %d %d",
							 getoid(o),
							 o->query_gridx(),
							 o->query_gridy()
							 )
					 );
	 }
   
}

int
is_areamap()
{
   return 1;
}

void
set_area_dimensions(int x, int y)
{
   areamap_w = x;
   areamap_h = y;
}

int
query_area_w()
{
   return areamap_w;
}

int
query_area_h()
{
   return areamap_h;
}

int /* Can the grid be entered */
is_clear (int x, int y)
{
   object o, *u;
   
   if (x < 0 || x > areamap_w || y < 0 || y > areamap_h)
	 return FALSE;
   
   if (member_array(query_tile(x, y), blocking_terrains) != -1)
	 return FALSE;
   
   u = get_obj(x, y);
   
	 foreach (o in u)
	 {
		if (o)
		  if (o->query_physical())
			return FALSE;
	 }
      return TRUE;
}

int /* Get a clear grid from near area, should be used with radius < 10 (maybe less?) */
*get_near_location(int x, int y, int radius)
{
   int i, j, rmax = radius+1;
   int *dirs = ({ 0,0 });
      if (radius > 10)
	      radius = 10;
   for (i=-radius; i < rmax ; i++)
	 for (j=-radius; j < rmax ; j++)
	   if (i || j)
		 if (is_clear(x+i, y+j))
		   dirs += ({ x+i, y+j });
      if (sizeof(dirs) == 2)
	         return dirs;
   i = random(sizeof(dirs)/2-2)*2;
   return ({ dirs[i+2], dirs[i+3] });
}


int /* Get a clear grid further away */
*get_far_location(int x, int y, int radius)
{
   int maxcount = radius*radius;
   do
	 {
		maxcount--;
		x += random(radius*2+1)-radius;
		y += random(radius*2+1)-radius;
	 }
   while (!is_clear(x, y) && maxcount > 0);
   
   if (maxcount == 0)
	 return ({ 0, 0 });
   else
	 return ({ x, y });
}

varargs object * /* Get livings from an area */
get_targets(int x, int y, int radius)
{
   if (!radius)
	 radius = CLIENT_RADIUS;
   return get_livings(x - radius, y - radius, 
					  x + radius, y + radius);
}

varargs object * /* Get targets for messages */
get_message_targets(int x, int y, int radius)
{
   if (!radius)
	 radius = CLIENT_RADIUS;
   return get_players(x - radius, y - radius,
					   x + radius, y + radius);
}

int /* Get a random clear grid from surroundgin grids */
*get_valid_dir(int x, int y)
{
   int i, j;
   int *dirs = ({ 0, 0 });
   for (i=-1; i < 2 ;i++)
	 for (j=-1; j < 2 ;j++)
	   if (i || j)
		 if (is_clear(x+i, y+j))
		   dirs += ({ i,j });
      i = random(sizeof(dirs)/2)*2;
      return ({ dirs[i]+x, dirs[i+1]+y });
}

string
find_stair(int x, int y)
{
   return THOB->get_area(x, y)->find_stair(x - x/100*100, y - y/100*100);
}

void
add_door (int x, int y, string material, int state, int lock)
{
   get_area(x, y)->add_door(x - x/100*100, y - y/100*100, material, state, lock);
   
}

void
update_door (int x, int y, string material, int state, int lock)
{
   get_area(x, y)->update_door(x - x/100*100, y - y/100*100, material, state, lock);
}
   
string
query_door(int x, int y)
{
   return get_area(x, y)->query_door( x - x/100*100, y - y/100*100);
}
