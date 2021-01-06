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

#include <living.h>
#include <hitloc.h>
#include <feeling.h>
#include <security.h>

inherit NATURAL_WEAPONS;
inherit HITLOC;
inherit GRIDOBJECT;
inherit SKILL_SYSTEM;
inherit SPELL_SYSTEM;
inherit BEHAVIOR;
inherit EFFECT;
inherit INTRODUCE;

string race;
int gender;

int alignx, aligny;

int hp;
int sp;
int ep;
int age; /* age in heart beats */
int max_age; /* max age in heart beats */

int aggressiveness;

int weight;
int height;

nosave int casting;
nosave int were_in_battle;

nosave int defence;
nosave int offense;
nosave int fatique;
nosave int damage;
nosave int ethereal;

int position;
nosave string active_cstyle;

nosave object chat_target;

nosave object *near_objects = ({ });

int uncons;
int dead;

int freeze = 0;
int speed = 500000;

int *race_stats;       /* 1-200 */
int *comp_stats;       /* -50 - 50 */

int
set_were_in_battle (int i)
{
   return were_in_battle = i;
}

int
query_were_in_battle ()
{
   return were_in_battle;
}

string
query_name (object o)
{
   string s;
   
   if (!o)
	 return lower_case(THOB->query_stranger_name());
   else if (s = o->test_known (THOB->query_uid()))
	 return unarticle(s) == s ? capitalize(lower_case(s)) : lower_case(s);
   else
	 return lower_case(THOB->query_stranger_name());
}

string
query_cap_name (object o)
{   
   string s;
   
   if (!o)
	 return capitalize(THOB->query_stranger_name());
   else if (s = o->test_known (THOB->query_uid()))
	 return capitalize(s);
   else
	 return capitalize(THOB->query_stranger_name());
}

string
query_short (object o)
{   
   string s;
   
   if (!o)
	 return THOB->query_stranger_short();
   else if (s = o->test_known (THOB->query_uid()))
	 return s;
   else
	 return THOB->query_stranger_short();
}

int
query_age ()
{
   if (!age)
	 age = 60*24*28*8*18/0.1; /* 18 years old */
   
   return age;
}

int
query_max_age ()
{
   if (!max_age)
	 max_age = 60*24*28*8*60/0.1; /* 60 years old */
   
   return max_age;
}

int
set_age (int i)
{
   return age = i;
}

int
set_max_age (int i)
{
   return max_age = i;
}

int
query_ethereal ()
{
   return ethereal;
}

int
set_ethereal (int i)
{
   return ethereal = i;
}

int
query_money ()
{
   object o;
   
   o = present(THOB, "gold coin");
   
   if (!o)
	 return 0;
   else
	 return o->query_value();
}

int
add_money (int i)
{
   object o;
   
   o = present(THOB, "gold coin");
   
   if (i > 0 && !o)
	 {
		o = GENERIC_D->generate ("item", "gold", "gold coin");
		o->add_value (i - 1);
		o->move (THOB, -1, -1);
	 }
   else if (!o)
	 return 0;
   else
	 {
		o->add_value (i);
	 }
   
   if (o)
	 return o->query_value();
   else
	 return 0;
}

int
query_gender ()
{
   return gender;
}

int
set_gender (int i)
{
   if (gender < 0 || gender > 2)
	 return gender = 0;
   
   return gender = i;
}

void
create_gridobject ()
{
   int i;
   race_stats = allocate (A_MAX);
   comp_stats = allocate (A_MAX);
   for(i=0;i<A_MAX;i++)
	 {
		comp_stats[i] = random(101) - 50;
	 }
   aggressiveness = 50;
   uncons = FALSE;
   dead = FALSE;
   position = POS_STANDING;
   height = 180; /* kludge */
   weight = 80;
   magic_style = -1;
   
   personality = allocate(sizeof(personalities));
   THOB->create_living();
   if(base_name(THOB) != file_name(THOB))
	 set_heart_beat(1);
}

int
query_hp ()
{
   return hp;
}

int
query_sp ()
{
   return sp;
}

int
query_ep ()
{
   return ep;
}

int
add_hp (int i)
{
   hp += i;
   write_client (THOB, CMD_PLR_HP, hp);
   if(hp < 0)
	 {
		if (THOB->query_security())
		  {
			 message (CMD_MSG, 0,0,0, "You are an immortal and therefore can't be killed.", THOB);
			 THOB->revive();
			 THOB->heal_hitlocs();
			 THOB->set_position (POS_STANDING);
			 return hp;
		  }
		else if(hp < -(THOB->query_max_hp() * 2))
		  {
			 if(!THOB->query_dead())
			   THOB->die();
		  }
		else
		  {
			 if(!THOB->query_uncons() && !THOB->query_dead())
			   THOB->uncons();
		  }
	 }
   return hp;
}

int
add_sp (int i)
{
   sp += i;
   write_client (THOB, CMD_PLR_SP, sp);
   return sp;
}

int
add_ep (int i)
{
   ep += i;
   write_client (THOB, CMD_PLR_EP, ep);
   if(i < 0 && ep < 0 && !THOB->query_dead() && !THOB->query_uncons())
	 {
		message (CMD_SEE, THOB->query_gridx(), THOB->query_gridy(), VISIBLE_RADIUS, "$n1 {are} overwhelmed by exhaustion.", MAPENV(THOB), THOB);
		THOB->add_hp(ep);
		THOB->flush();
	 }
   return ep;
}

int
query_max_hp ()
{
   return THOB->query_stat(A_STU) * 5 + THOB->query_stat(A_CON) * 2 +
	 THOB->query_stat(A_STR) * 2 + THOB->query_stat(A_POW);
}

int
query_max_sp ()
{
   return THOB->query_stat(A_POW) * 5 + THOB->query_stat(A_INT) * 2 + 
	 THOB->query_stat(A_WIS) * 2 + THOB->query_stat(A_CON);
}

int
query_max_ep ()
{
   return THOB->query_stat(A_CON) * 5 + THOB->query_stat(A_STR) * 2 + 
	 THOB->query_stat(A_POW) * 2 + THOB->query_stat(A_AGI);
}

int
query_race_stat(int i)
{
   return race_stats[i];
}

int *
query_race_stats()
{
   return race_stats;
}

int
query_comp_stat(int i)
{
   return comp_stats[i];
}

int *
query_comp_stats()
{
   return comp_stats;
}

int
query_stat (int i)
{
   int val;
   
   val = THOB->query_race_stat(i) + THOB->query_comp_stat(i) / 10;
   if(val > THOB->query_max_stat(i))
	 val = THOB->query_max_stat(i);
   else if(val < 1)
	 val = 1;
   
   return val;
}

int *
query_stats()
{
   int i, j;
   int *arr = allocate(A_MAX);
   for(i=0,j=A_MAX;i<j;i++)
	 {
		arr[i] = THOB->query_stat(i);
	 }
   return arr;
}

string
query_stat_string()
{
   int *stats = THOB->query_stats();
   return sprintf("%d %d %d %d %d %d %d %d %d", stats[A_STR], stats[A_DEX], 
				  stats[A_STU], stats[A_CON], stats[A_AGI], stats[A_INT], 
				  stats[A_WIS], stats[A_POW], stats[A_CHA]);
}
 
int
query_max_stat(int i)
{
   return RACE_D->query_stats(THOB->query_race())[i] * 2;
}

int *
set_comp_stats(int *values)
{
   comp_stats = values;
   if(THOB->is_player())
	 {
		write_client(THOB, CMD_PLR_STAT, THOB->query_stat_string()); 
	 }
   return comp_stats;
}

int *
set_comp_stat(int type, int value)
{
   comp_stats[type] = value;
   if(THOB->is_player())
	 {
		write_client(THOB, CMD_PLR_STAT, THOB->query_stat_string());
	 }
   return comp_stats;
}

int *
set_race_stats(int *values)
{
   race_stats = values;
   if(THOB->is_player())
	 {
		write_client(THOB, CMD_PLR_STAT, THOB->query_stat_string());
	 }
   return race_stats;
}

int
set_race_stat(int type, int value)
{
   race_stats[type] = value;
   if(THOB->is_player())
	 {
		write_client(THOB, CMD_PLR_STAT, THOB->query_stat_string());
	 }
   return race_stats[type];
}

object
query_chat_target()
{
   return chat_target;
}

object
set_chat_target(object ob)
{
   return chat_target = ob;
}

int
set_aggressiveness(int i)
{
   if(i > 100)
	 i = 100;
   return aggressiveness = i;
}

int
query_aggressiveness()
{
   return aggressiveness;
}

int
query_defensiveness()
{
   return (100 - aggressiveness);
}

int
set_position(int i)
{
   if(position == i)
	 return i;
   
   switch(i)
	 {
	  case POS_LYING:
		if(position == POS_STANDING)
		  THOB->add_speed(500000);
		break;
	  default:
		THOB->add_speed(-500000);
		break;
	 }
   return position = i;
}

int
query_position()
{
   return position;
}

int
add_fatique_bonus(int i)
{
   return fatique += i;
}

int
set_fatique_bonus(int i)
{
   return fatique = i;
}

int
query_fatique_bonus()
{
   return fatique;
}

int
add_offense_bonus(int i)
{
   return offense += i;
}

int
set_offense_bonus(int i)
{
   return offense = i;
}

int
query_offense_bonus()
{
   return offense;
}

int
add_defence_bonus(int i)
{
   return defence += i;
}

int
set_defence_bonus(int i)
{
   return defence = i;
}

int
query_defence_bonus()
{
   return defence;
}

int
add_damage_bonus(int i)
{
   return damage += i;
}

int
set_damage_bonus(int i)
{
   return damage = i;
}

int
query_damage_bonus()
{
   return damage;
}

int
set_casting(int i)
{
   if(i == casting)
	 return 0;
   
   if(i)
	 {
		THOB->add_speed(250000);
	 }
   else
	 {
		THOB->add_speed(-250000);
	 }
   return casting = i;
}

int
query_casting()
{
   return casting;
}

string
set_active_combat_style(string str)
{
   int sc;
   object skillob;
   
   if(!str || str == "")
	 return 0;
   
   if(str == active_cstyle)
	 return 0;
   
   skillob = SKILL_D->find_skill(str);
   if(!skillob)
	 return 0;
      
   if(stringp(active_cstyle))
	 {
		skillob = SKILL_D->find_skill(active_cstyle);
		sc = THOB->query_skill_chance(skillob->query_name());
		THOB->add_speed((skillob->query_speed_bonus() * sc / 100) * 5000);
		THOB->add_fatique_bonus(-(skillob->query_fatique_bonus() * (100 - sc)) / 100);
		skillob = SKILL_D->find_skill(str);
	 }
		
   sc = THOB->query_skill_chance(skillob->query_name());
   THOB->add_speed(-(skillob->query_speed_bonus() * sc / 100) * 5000);
   THOB->add_fatique_bonus((skillob->query_fatique_bonus() * (100 - sc)) / 100);
   return active_cstyle = skillob->query_name();
}

string
query_active_combat_style()
{
   return active_cstyle;
}

int
set_weight(int i)
{
   return weight = i;
}

int
query_weight()
{
   if (!weight)
	 weight = 75;
   
   return weight * 1000;
}

int
set_height(int i)
{
   return height = i;
}

int
query_height()
{
   if (!height)
	 height = 175;
   
   return height * 1000;
}

int
query_max_carry()
{
   return THOB->query_stat(A_STR) * 6 * 1000;
}

int
query_carried_weight()
{
   int i, j;
   int carr = 0;
   object *obs = all_inventory(THOB);
   
   for(i=0,j=sizeof(obs);i<j;i++)
	 {
		carr += obs[i]->query_weight();
	 }
   return carr;
}

int
query_weight_penalty()
{
   int carr = THOB->query_carried_weight();
   float nopen = THOB->query_weight() / 10;
   float penalty = (carr / nopen) * 10;
   penalty -= THOB->query_stat(A_STR) / 2;
   if(penalty < 0)
	 penalty = 0;
   return to_int(penalty);
}

int
set_uncons(int i)
{
   return uncons = i;
}

int
query_uncons()
{
   return uncons;
}

int
set_dead(int i)
{
   return dead = i;
}

int
query_dead()
{
   return dead;
}

int
add_object(object ob, int toggle)
{
   string wstr;
   string *locs;
   
   if(!ob)
	 return -1;
   
   ob->move(THOB,-1,-1);
   if(!toggle)
	 return 0;
      
   if(ob->is_weapon())
	 {
		locs = THOB->query_possible_wield_hitlocs(ob);
		if(sizeof(locs) < 1)
		  return 0;
		wstr = locs[random(sizeof(locs))];
		locs = allocate(2);
		if(sscanf(wstr, "%s and %s", locs[0], locs[1]) != 2)
		  locs = ({ wstr });
		THOB->wield(ob, locs);
	 }
   else if(ob->is_clothing())
	 {
		THOB->wear(ob);
	 }
   return 1;
}

void
revive ()
{
   THOB->set_dead(0);
   THOB->set_uncons(0);
   THOB->set_freeze(0);
   THOB->add_hp(THOB->query_max_hp() - THOB->query_hp());
   THOB->add_sp(THOB->query_max_sp() - THOB->query_sp());
   THOB->add_ep(THOB->query_max_ep() - THOB->query_ep());
   THOB->stop_bleed();
}

void
die ()
{
   /* int *xy; */
   string *parts;
   
   if (THOB->query_security())
	 {
		message (CMD_MSG, 0,0,0, "You are an immortal and therefore can't be killed.", THOB);
		THOB->revive();
		THOB->heal_hitlocs();
		THOB->set_position (POS_STANDING);
		return;
	 }
   
   if(dead)
	 return;
   
   THOB->flush();
   parts = THOB->query_existing_hitlocs();
   THOB->force_us("drop all");
   THOB->clone_bodypart(parts);
   /*
    xy = "/world/misc/hell"->get_near_location(48, 48, 5);
    foreach(string entry in parts)
	{
	   THOB->set_hitloc_cond(entry, HITLOC_SEVERED);
	}
	THOB->move("/world/misc/hell", xy[0], xy[1]);
	THOB->stop_bleed();
	THOB->set_dead(1);
	THOB->set_uncons(0);
	THOB->set_freeze(0);
	THOB->set_position(POS_STANDING);
	*/
   message (CMD_SEE, THOB->query_gridx(), THOB->query_gridy(), VISIBLE_RADIUS, "$n1 {die}.", MAPENV(THOB), THOB);
   if (interactive (THOB) || THOB->query_offline())
	 {
		if (THOB->query_offline())
		  OFFLINE_D->remove_offline (THOB->query_true_name(), TRUE);
		else
		  {
			 if (interactive (THOB))
			   write_client (THOB, CMD_QUIT, "You have been killed.");
			 			   
			 if(THOB->query_offline())
			   OFFLINE_D->remove_offline (THOB->query_true_name(), TRUE);
			 else
			   {
				  dead = 1;
				  THOB->save_me();
				  // rm ("/data/player/"+THOB->query_true_name()[0..0]+"/"+THOB->query_true_name()+".o");
				  THOB->remove();
			   }
		  }
	 }
   else
	 THOB->remove();
}

void
recover ()
{
   THOB->flush();
   message (CMD_SEE, THOB->query_gridx(), THOB->query_gridy(), VISIBLE_RADIUS, "$n1 {regain} consciousness.", MAPENV(THOB), THOB);
   THOB->set_uncons(0);
   THOB->set_freeze(0);
}

void
uncons ()
{
   if(uncons)
	 return;
   
   THOB->flush();
   message (CMD_SEE, THOB->query_gridx(), THOB->query_gridy(), VISIBLE_RADIUS, "$n1 {fall} unconscious.", MAPENV(THOB), THOB);
   THOB->set_position(POS_LYING);
   THOB->set_uncons(1);
   if(THOB->query_hp() > 0)
	 THOB->set_freeze(10);
   else
	 THOB->set_freeze(-1);
}

void
heart_beat ()
{
   age++;
   
   if (freeze > 0)
	 {
		freeze--;
		if(interactive(THOB))
		  write_client (THOB, CMD_FREEZE, freeze);
	 }
   
   if(!(age % 10))
	 {
		THOB->update_spell_queue();
	 }
   		
    if(!(age % 20) && (THOB->query_bleed_value())) 
	 {
	    THOB->bleed();
	 }
   
   if(!(age % 25) && (THOB->query_bleed_value()))
	 {
		THOB->update_bleed();
	 }
   
   if (!(age % 15))
	 {
		if (THOB->query_hp() < THOB->query_max_hp())
		  {
			 THOB->add_hp (3);
			 if( (THOB->query_hp() >= 0) && (THOB->query_uncons() == 1) )
			   THOB->recover();
		  }
		
		if (THOB->query_ep() < THOB->query_max_ep())
		  THOB->add_ep (3);
		
		if (THOB->query_sp() < THOB->query_max_sp())
		  THOB->add_sp (1);
	 }
   THOB->effect_beat();
   behavior::heart_beat();
}

string
query_position_string()
{
   switch(THOB->query_position())
	 {
	  case POS_SITTING:
		return "sitting";
		break;
		
	  case POS_STANDING:
		return "standing";
		break;
		
	  case POS_LYING:
		return "lying";
		break;
	 }
}

string
query_pronoun()
{
   return gender == 2 ? "she" : gender ? "he" : "it";
}

string
query_objective()
{
   return gender == 2 ? "her" : gender ? "him" : "it";
}

string
query_possessive()
{
   return gender == 2 ? "her" : gender ? "his" : "its";
}

int
force_us(string str)
{
   string verb, args, fail_message;
   int ret, x, y, tempx, tempy;
   object *all, o;
   
   THOB->set_were_in_battle (0);
   
   if (str && stringp (str))
	 {
		if (sscanf(str, "%s %s", verb, args) != 2)
		  {
			 verb = str;
			 args = "";
		  }
		verb = lower_case(verb);
		
		if (CHANNEL_D->channel(verb, args))
		  return 1;
		
		if (ENV(THOB))     
		  ret = ENV(THOB)->test_command (THOB->query_gridx(),
										 THOB->query_gridy(),
										 verb, args);
		
		if (!ret && THOB->query_security() >= SEC_GOD)
		  if (file_size ("/command/admin/" + verb + ".c") > -1)
			ret = call_other ("/command/admin/" + verb + ".c", "main", args);
		
		if (!ret && THOB->query_security() >= SEC_LESSER_GOD)
		  if (file_size ("/command/immortal/" + verb + ".c") > -1)
			ret = call_other ("/command/immortal/" + verb + ".c", "main", args);
		
		if (!ret)
		  {
			 /* find command from our inventory */
			 
			 all = all_inventory(THOB);
			 if(sizeof(all) > 0)
			   {
				  foreach (o in all)
					{
					   ret = o->test_command (verb, args);
					   if (ret)
						 break;
					}
			   }
			 
			 /* from the grid we are standing */
			 
			 if (!ret)
			   {
				  all = MAPENV(THOB)->get_obj(x, y);
				  if(sizeof(all) > 0)
					{
					   foreach (o in all)
						 {
							if (!o->query_physical() && o != THOB)
							  ret = o->test_command (verb, args);
							
							if (ret)
							  break;
						 }
					}
			   }
			 
			 /* from around the grid we are standing */
			 if (!ret)
			   {							
				  x = THOB->query_gridx();
				  y = THOB->query_gridy();
				  tempx = MAPENV(THOB)->query_area_w();
				  tempy = MAPENV(THOB)->query_area_h();
				  
				  for (int i=y-1;i<y+2;i++)
					{
					   if ((y < 0) || (y >= tempy))
						 continue;
					   for (int j=x-1;j<x+2;j++)
						 {
							if ((x < 0) || (x >= tempx))
							  continue;
							all = MAPENV(THOB)->get_obj(j, i);
							if(sizeof(all) > 0)
							  {
								 foreach (o in all)
								   {
									  if (o)
										if (o->query_physical() && o != THOB)
										  ret = o->test_command (verb, args);										   
									  
									  if (ret)
										break;
								   }
							  }
							
							if (ret)
							  break;
						 }
					   
					   if (ret)
						 break;
					}
			   }	
		  }
		
		if (!ret && file_size ("/command/mortal/" + verb + ".c") > -1)
		  ret = call_other ("/command/mortal/" + verb + ".c", "main", args);
		
		if (ret == 0 && fail_message)
		  message (CMD_MSG, 0,0,0, fail_message, THOB);
		else if(THOB->query_somatic_meaning(verb))
		  {
			 message (CMD_SEE, THOB->query_gridx(), THOB->query_gridy(), VISIBLE_RADIUS, "$n1 {perform} a complicated magical gesture.", MAPENV(THOB), THOB);
			 THOB->add_spell_component(THOB->query_somatic_meaning(verb), 2);
		  }
		else if(FEELING_D->query_feeling(verb))
		  {
			 verb = FEELING_D->query_feeling(verb)[FEEL_STRING];
			 if(member_array(args, FEELING_D->query_adverbs()) != -1)
			   verb += " "+args;
			 verb += ".";
			 message (CMD_SEE, THOB->query_gridx(), THOB->query_gridy(), VISIBLE_RADIUS, verb, MAPENV(THOB), THOB);
		  }
		else if (ret == 0)
		  message (CMD_MSG, 0,0,0, "No such command available.", THOB);
	 }
}

int
set_speed(int i)
{
   speed = i;
   if(speed < MAX_SPEED)
	 speed = MAX_SPEED;
   if (interactive(THOB))
	 write_client (THOB, CMD_SPEED, speed);
   return speed;
}

int
add_speed (int i)
{
   speed += i;
   if(speed < MAX_SPEED)
	 speed = MAX_SPEED;
   if (interactive(THOB))
	 write_client (THOB, CMD_SPEED, speed);
   return speed;
}

int
query_speed (int i)
{
   if (were_in_battle)
	 return speed * 4; /* 0.5s * 4 == 2.0s */
   else
	 return speed;
}

int
add_freeze (int i)
{
   freeze += i;
   if (interactive(THOB))
	 write_client (THOB, CMD_FREEZE, freeze);
   THOB->flush();
   return freeze;
}

int
query_freeze (int i)
{
   return freeze;
}

int
set_freeze (int i)
{
   write_client (THOB, CMD_FREEZE, i);
   THOB->flush();
   return freeze = i;
}

string
set_race(string str)
{
   if(!str || !stringp(str))
	 return 0;
   
   if(race == str)
	 {
		if(THOB->is_player())
		  {
			 write_client(THOB, CMD_PLR_STAT, THOB->query_stat_string());
			 write_client(THOB, CMD_PLR_HP, hp);
			 write_client(THOB, CMD_PLR_SP, sp);
			 write_client(THOB, CMD_PLR_EP, ep);
		  }
		return 0;
	 }
   race = str;
   THOB->set_bodytype(RACE_D->query_bodytype(str));
   THOB->set_gridchar(RACE_D->query_gridchar(str));
   THOB->set_gridcolor(RACE_D->query_gridcolor(str));
   THOB->set_race_stats(RACE_D->query_stats(str));
   THOB->set_gender(RACE_D->query_genders(str)[random(sizeof(RACE_D->query_genders(str)))]);
   
   hp = THOB->query_max_hp();
   sp = THOB->query_max_sp();
   ep = THOB->query_max_ep();
   
   if(THOB->is_player())
	 {
		write_client(THOB, CMD_PLR_HP, hp);
		write_client(THOB, CMD_PLR_SP, sp);
		write_client(THOB, CMD_PLR_EP, ep);
	 }
   
   THOB->init_bodyparts();
   THOB->init_hitlocs();
   THOB->init_skills();
   THOB->init_natural_weapons();
   return race;
}

string
query_race()
{
   return race;
}

int
is_living()
{
   return TRUE;
}

void 
add_near_object(object obj)
{
   near_objects += ({ obj });
}

void 
remove_near_object(object obj)
{
   near_objects -= ({ obj });
}

object 
*query_near_objects()
{
   return near_objects;
}

void
clear_near_objects()
{
   near_objects = ({ });
}
