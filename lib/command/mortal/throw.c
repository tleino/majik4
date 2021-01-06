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

#include <list.h>
#include <living.h>
#include <battle.h>
#include <weapon.h>
#include <tile.h>

int
collide(object thrower, object ob, int x, int y)
{
   int i, j;
   int phys;
   object *coll;
   int damage;
   int damtype, *damtypes;
   mixed *battarr;
   
   coll = MAPENV(thrower)->get_obj(x, y);
   phys = -1;
   if(sizeof(coll) > 0)
	 {
	  	for(i=0,j=sizeof(coll);i<j;i++)
		  {
			 if(coll[i]->query_physical() && !coll[i]->query_ethereal())
			   {
				  phys = i;
				  break;
			   }
		  }
	 }
   
   if(phys == -1)
	 {
		switch(MAPENV(thrower)->query_tile(x, y))
		  {
		   case TILE_L_CON_TREE:
		   case TILE_L_DEC_TREE:
		   case TILE_L_JUN_TREE:
			 message (CMD_SEE, x, y, VISIBLE_RADIUS, "$p1 soars through the air and hits a large tree.", MAPENV(thrower), ob);
			 break;
		   case TILE_CLOSED_DOOR:
			 message (CMD_SEE, x, y, VISIBLE_RADIUS, "$p1 soars through the air and hits a closed door.", MAPENV(thrower), ob);
			 break;
		   default:
			 message (CMD_SEE, x, y, VISIBLE_RADIUS, "$p1 soars through the air and hits a wall.", MAPENV(thrower), ob);
			 break;
		  }
	 }
   else
	 {
		if(coll[0]->is_living())
		  {
			 damtypes = ob->query_damage_types();
			 if(sizeof(damtypes) > 0)
			   damtype = damtypes[random(sizeof(damtypes))];
			 else
			   damtype = DT_IMPACT;
			 
			 if(ob->query_range() & RANG_THROWN))
			   damage = ob->query_type_damage(damtype);
			 else
			   damage = 10;
			   			 
			 battarr = allocate(WD_MAX);
			 battarr[WD_WEAPON] = ob;
			 battarr[WD_DAMTYPE] = damtype;
			 battarr[WD_DAMAGE] = damage;
			 battarr[WD_RANGE] = RANG_THROWN;
			 battarr[WD_SKILL] = ob->query_skill() ? ob->query_skill() : "throwing";
			 if(BATTLE_D->hit(thrower, coll[0], battarr) == HIT_MISS)
			   return 0;
		  }
		else
		  {
			 message (CMD_SEE, x, y, VISIBLE_RADIUS, "$p1 soars through the air and hits $n2.", MAPENV(thrower), ({ ob, coll[0] }) );
		  }
	 }
   return 1;
}
    
int
main(string str)
{
   int x, y, tx, ty, stren;
   int sc;
   int *xy;
   float err;
   object itemob;
   object *inv;
   string item;
   string buf = "";
      
   if(!str || str == "")
	 {
		inv = all_inventory(PREV(0));
		if(sizeof(inv) < 1)
		  {
			 message (CMD_MSG, 0,0,0, "You don't have anything you could throw.", PREV(0));
			 return 1;
		  }
		foreach(object entry in inv)
		  buf += entry->query_short()+";";
		open_window(PREV(0), 0,0,0,0, LTYPE_SELECT, "Throw what?", "throw", buf);
		return 1;
	 }
   
   if(sscanf(str, "%s to %d %d", item, x, y) != 3)
	 {
		select_target(PREV(0), "throw "+str+" to");
		return 1;
	 }
   
   tx = PREV(0)->query_gridx();
   ty = PREV(0)->query_gridy();
        
   itemob = present(PREV(0), item, -1, -1);
   if(!itemob)
	 {
		message (CMD_MSG, 0,0,0, "You don't have "+item+".", PREV(0));
		return 1;
	 }
   
   sc = PREV(0)->use_skill(itemob->query_skill() ? itemob->query_skill() : "throwing", 100, -75, 0);
   if(sc < 0)
	 {
		sc = random(abs(sc));
		sc = d("1d100") > 50 ? sc : -sc;
		err = to_float(sc) / 100.0;
	 }
   
   stren = PREV(0)->query_stat(A_STR);
   xy = fly(tx, ty, x, y, stren, err, MAPENV(PREV(0)));
   if(!xy)
	 {
		message (CMD_MSG, 0,0,0, "You can't throw yourself.", PREV(0));
		return 1;
	 }
   
   message (CMD_SEE, tx, ty, VISIBLE_RADIUS, "$n1 {throw} $n2.", MAPENV(PREV(0)), ({ PREV(0), itemob }) );
   if(itemob->is_wielded())
	 PREV(0)->remove_weapon(itemob);
   if(itemob->is_worn())
	 PREV(0)->strip(itemob);
   
   if(stren > VISIBLE_RADIUS)
	 inv = MAPENV(PREV(0))->get_message_targets(tx, ty, stren);
   else
	 inv = MAPENV(PREV(0))->get_message_targets(tx, ty, VISIBLE_RADIUS);
   foreach(object entry in inv)
	 write_client (entry, CMD_ANIMATE, sprintf("%d %d %d %d %d %d", to_int(itemob->query_gridchar()), itemob->query_gridcolor(), tx, ty, xy[0], xy[1]));
   if(!MAPENV(PREV(0))->is_clear(xy[0], xy[1]))
	 {
		collide(PREV(0), itemob, xy[0], xy[1]);
		if(tx > xy[0])
		  xy[0]++;
		else if(tx < xy[0])
		  xy[0]--;
		if(ty > xy[1])
		  xy[1]++;
		else if(ty < xy[1])
		  xy[1]--;
		itemob->move(MAPENV(PREV(0)), xy[0], xy[1]);
		PREV(0)->update_inv();
		return 1;
	 }
   
   itemob->move(MAPENV(PREV(0)), xy[0], xy[1]);
   message (CMD_SEE, x, y, VISIBLE_RADIUS, "$p1 hits the ground.", MAPENV(PREV(0)), itemob);
   PREV(0)->update_inv();
   return 1;
}
