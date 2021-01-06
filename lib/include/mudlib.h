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

#ifndef __MUDLIB_H__
#define __MUDLIB_H__

#include <mcp.h>

#define THIS 			this_player()
#define THOB 			this_object()
#define PREV(x) 		previous_object(x)
#define ENV(x) 			environment(x)
#define MAPENV(x)       environment(environment(x))

#define WORLDMAP		"/world/worldmap.c"
#define GRIDMAP 		"/inherit/gridmap.c"
#define AREAMAP         "/inherit/areamap.c"
#define GRIDOBJECT 		"/inherit/gridobject.c"
#define ITEM			"/inherit/item.c"
#define LIVING          "/inherit/living.c"
#define BASE 			"/inherit/base.c"
#define WEAPON			"/inherit/weapon.c"
#define SHIELD			"/inherit/shield.c"
#define MISSILE_WEAPON	"/inherit/missile_weapon.c"
#define MISSILE			"/inherit/missile.c"
#define ARMOR			"/inherit/armor.c"
#define CLOTHING		"/inherit/clothing.c"
#define BODYPART		"/inherit/bodypart.c"
#define HITLOC          "/inherit/hitloc.c"
#define NATURAL_WEAPONS "/inherit/natweapon.c"
#define EFFECT			"/inherit/effect.c"
#define SKILL_SYSTEM	"/inherit/skillsys.c"
#define SPELL_SYSTEM	"/inherit/spellsys.c"
#define SKILL			"/inherit/skill.c"
#define SPELL			"/inherit/spell.c"
#define COMBAT_STYLE	"/inherit/cstyle.c"
#define MONSTER         "/inherit/monster.c"
#define SHOP            "/inherit/shop.c"
#define BANK            "/inherit/bank.c"
#define BEHAVIOR        "/inherit/behavior.c"
#define CHANNEL         "/inherit/channel.c"
#define HORSEMAN		"/inherit/horseman.c"
#define INTRODUCE       "/inherit/introduce.c"

#define TIME_D          "/daemon/time.c"
#define WORLDGEN_D		"/daemon/worldgen.c"
#define CHANNEL_D		"/daemon/channel.c"
#define MATERIAL_D      "/daemon/material.c"
#define GENERIC_D       "/daemon/generic.c"
#define LOG_D			"/daemon/log.c"
#define RACE_D          "/daemon/race.c"
#define BATTLE_D		"/daemon/battle.c"
#define SKILL_D			"/daemon/skill.c"
#define SPELL_D			"/daemon/spell.c"
#define WEATHER_D       "/daemon/weather.c"
#define OFFLINE_D       "/daemon/offline.c"
#define FEELING_D		"/daemon/feeling.c"

#define SPELL_DIR       "/spell/"
#define SKILL_DIR       "/skill/"

#define VISIBLE_RADIUS  7

#define TRUE			1
#define FALSE			0

#endif /* __MUDLIB_H__ */
