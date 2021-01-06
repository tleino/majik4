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

#include <spellsys.h>

/* Academic, Primal, Religious, Divine */
mapping spelldata = ([
"alteration" : ({ "gnoru", "danzu" "mesiik", "punid" }),
"control" : ({ "safxyy", "icynag", "iijjaa", "ohiaqo" }),
"creation" : ({ "cuuvaa", "ihaono", "tyzka", "kuduf" }),
"destruction" : ({ "yykhuu", "zulaar", "yywzii", "vybryg" }),
"detection" : ({ "dyoop", "noeet", "iiblyy", "hiiwii" }),
"fire" : ({ "ofucii", "xaayyj", "jisuzy", "byvij" }),
"air" : ({ "aajiij", "vozyto", "saku", "sytii" }),
"water" : ({ "muucaa", "bobaxa", "wabeec", "covy" }),
"earth" : ({ "jusaac", "unosaa", "iirzyg", "sahpe" }),
"flesh" : ({ "veepoo", "epax", "iivem", "yyjyyg" }),
"nature" : ({ "atyoog", "liipy", "egfyw", "huuxyy" }),
"plantkind" : ({ "fuoti", "syjke", "guqej", "ryahe" }),
"magic" : ({ "iitov", "vaaiik", "ilva", "omerep" }),
"mind" : ({ "utydu", "upuwo", "udoeer", "lubdiz" }),
"spirit" : ({ "ooseno", "ynvi", "joaza", "qawaby" }),
"image" : ({ "eeqjyj", "yybjuc", "paalee", "xuupyl" }),
"arrow" : ({ "xoqee", "yrecy", "bohoo", "yyxcax" }),
"being" : ({ "jadluu", "tyji", "jiivuu", "ofevu" }),
"essence" : ({ "baaho", "udeeku", "bosar", "puuohy" }),
"key" : ({ "eemce", "akipo", "lafeew", "daacuu" }),
"shield" : ({ "eefimi", "cyyul", "uujtus", "dyyli" }),
"sphere" : ({ "ykuta", "omba", "gigic", "jucko" }),
"weapon" : ({ "xahhi", "reoof", "uthuu", "aaboz" }),
"end" : ({ "fin", "fin", "fin", "fin" }),
]);

string 
query_style_verbal_symbol(int style, string symbol)
{
   if(!style || style < 0 || !symbol || symbol == "")
	 return 0;
   
   return spelldata[symbol][style];
}

int
query_verbal_word_style(string word)
{
   int i, j, ret;
   string *syms = keys(spelldata);
   
   for(i=0,j=sizeof(syms);i<j;i++)
	 {
		ret = member_array(word, spelldata[syms[i]]);
		if(ret != -1)
		  return ret;
	 }
   return -1;
}

string
query_verbal_word_meaning(string word)
{
   int i, j, style;
   string *strs;
   
   style = query_verbal_word_style(word);
   if(style == -1)
	 return 0;
   
   strs = keys(spelldata);
   for(i=0,j=sizeof(strs);i<j;i++)
	 {
		if(member_array(word, spelldata[strs[i]]) != -1)
		  return strs[i];
	 }
   return 0;
}

string *
get_spells()
{
   int i, j;
   string *strs = get_dir(SPELL_DIR), *retstrs = ({ });
   for(i=0,j=sizeof(strs);i<j;i++)
	 {
		if(strs[i][<2..<1] == ".c")
		  retstrs += ({ strs[i] });
	 }
   return retstrs;
}

object
find_spell(string spell)
{
   int i, j;
   string *strs = get_spells();
   object ob;
   
   for(i=0,j=sizeof(strs);i<j;i++)
	 {
		ob = load_object(SPELL_DIR + strs[i]);
		if(member_array(spell, ob->query_casting_symbols()) != -1)
		  return ob;
	 }
   return 0;
}

/* Al, Co, Cr, De, Dt */
string *
query_tech_symbols()
{
   return ({
	  "alteration",
	  "control",
	  "creation",
	  "destruction",
	  "detection"
   });
}

/* Fi, Fl, Ai, Pl, Ea, Ma, Wa, Mi, Im */
string *
query_form_symbols()
{   
   return ({
	  "fire", 
      "air",
	  "water",
	  "earth",
	  "flesh",
	  "nature"
	  "plantkind",
	  "magic",
	  "mind",
	  "spirit", 
	  "image"
   });
}

/* Ar, Be, Es, Ke, Sh, Sp, We */
string *
query_power_symbols()
{
   return ({
	  "arrow",         /* affecting things from distance                   */
	  "being",         
	  "essence",       /* affecting distance weapons                       */
	  "key",           /* affecting keys and locks of all kinds            */
	  "shield",        /* affects protection values, defenses              */
	  "sphere",        /* shapes the spell into spherical shape, affects
						* spheres, balls and all things round. Also, group
						* of targets.                                      */
	  "weapon",        /* affects melee weapons                            */
   });
}

string *
query_all_symbols()
{
   return ({ "end" }) + query_tech_symbols() + query_form_symbols() + query_power_symbols();
}
