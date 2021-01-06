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

mapping stat_bonus_map = ([
"str" : A_STR,
"dex" : A_DEX,
"stu" : A_STU,
"con" : A_CON,
"agi" : A_AGI,
"int" : A_INT,
"wis" : A_WIS,
"pow" : A_POW,
"cha" : A_CHA
]);

object find_skill(string);
string *query_skill_parents(string);

mapping
query_stat_bonus_map()
{
   return stat_bonus_map;
}

int
query_stat_bonus(string skill, object creature)
{
   int i, j;
   int stat;
   object skillob;
   mapping bonusmap;
   string *bonusstats;
      
   if(!creature)
	 return 0;
   
   skillob = find_skill(skill);
   if(!skillob)
	 return 0;
   
   bonusmap = skillob->query_bonus_stats();
   bonusstats = keys(bonusmap);
   if(sizeof(bonusstats) < 1)
	 return 0;
   
   for(i=0,j=sizeof(bonusstats);i<j;i++)
	 {
		stat += creature->query_stat(stat_bonus_map[bonusstats[i]]) / bonusmap[bonusstats[i]];
	 }
   return (stat / j) / 2;
}

string *
get_skills()
{
   string *fnames = get_dir(SKILL_DIR);
   string *actuals = ({ });
   foreach(string entry in fnames)
	 {
		if(entry[<2..<1] == ".c")
		  actuals += ({ entry });
	 }
   return actuals;
}

object
find_skill(string str)
{
   string skillfile;
   
   if(!str || str == "")
	 return 0;
   
   skillfile = replace_string(str, " ", "_");
   return load_object(SKILL_DIR + skillfile);
}

string *
query_skill_children(string str)
{
   int i, j;
   object skillob;
   string *skills = get_skills();
   string *children = ({ });
   
   for(i=0,j=sizeof(skills);i<j;i++)
	 {
		skillob = find_skill(skills[i]);
		if(skillob->query_parent() == str && member_array(skillob->query_name(), children) == -1)
		  children += ({ skillob->query_name() });
	 }
   return children;
}

string *
query_skill_parents(string str)
{
   string skill = str;
   object skillob = find_skill(skill);
   string *parents = ({ });
   
   if(!skillob)
	 return 0;
   
   while(skillob->query_parent())
	 {
		skill = skillob->query_parent();
		parents += ({ skill });
		skillob = find_skill(skill);
	 }
   return parents;
}

void
print_skill_tree(object who, string start, int depth)
{
   int i, j;
   string buf = "";
   string *children;
   
   children = THOB->query_skill_children(start);
     
   for(i=0;i<depth;i++)
	 buf += "-";
   buf += start;
   message (CMD_MSG, 0,0,0, buf, who);
   
   for(i=0,j=sizeof(children);i<j;i++)
	 THOB->print_skill_tree(who, children[i], depth+1);
}

void
destruct_skillobs()
{
   object *obs = objects();
   
   foreach(object entry in obs)
	 {
		if(entry->is_skill())
		  destruct(entry);
	 }
}
