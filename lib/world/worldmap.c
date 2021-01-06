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

inherit AREAMAP;

#include <tile.h>
#include <terrain.h>

string *worldmap;
mixed *map_array;

void set_worldmap(string *);

#define 	T_CHAR		0
#define		T_FLAGS		1
#define		T_PROB		2
#define		T_FNUM		3
#define		T_FPARAM1	4
#define		T_FPARAM2	5

mapping terrains = ([
/* 
 *                 +---------------------- gridmap character 
 *                 |   +------------------ flags (0 = none, 1 = no random)
 *                 |   |   +-------------- probability of terrain (relative)
 *                 |   |   |  +----------- blending function number (0=linear)
 *                 |   |   |  |  +-------- function parameter 1 (0-100)
 *  worldmap char  |   |   |  |  |   +---- function parameter 2 (0-100)
 *     |           |   |   |  |  |   | */

TERRAIN_GLACIER : ({
   ({ TILE_SNOW, 0, 75, 0, 0, 100 }),
   ({ TILE_ICE, 0, 55, 0, 0, 100 }),
}),
TERRAIN_TUNDRA : ({
   ({ TILE_SNOW, 0, 20, 0, 0, 100 }),
   ({ TILE_S_BUSH, 0, 15, 0, 0, 100 }),
   ({ TILE_S_CON_TREE, 0, 15, 0, 0, 100 }),
   ({ TILE_S_DEC_TREE, 0, 5, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 10, 0, 0, 100 }),
   ({ TILE_ICE, 0, 5, 0, 0, 100 }),
}),
TERRAIN_PRAIRIE : ({
   ({ TILE_SAND, 0, 15, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 20, 0, 0, 100 }),
   ({ TILE_S_BUSH, 0, 5, 0, 0, 100 }),
   ({ TILE_BUSH, 0, 10, 0, 0, 100 }),
   ({ TILE_DEC_TREE, 0, 10, 0, 0, 100 }),
}),
TERRAIN_HILL : ({
   ({ TILE_GRASS, 0, 10, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 10, 0, 0, 100 }),
   ({ TILE_DEC_TREE, 0, 3, 0, 0, 100 }),
}),
TERRAIN_MOUNTAIN : ({
   ({ TILE_EARTH, 0, 23, 0, 0, 100 }),
   ({ TILE_ROCK, 0, 20, 0, 0, 100 }),
   ({ TILE_STONE_FLOOR, 0, 30, 0, 0, 100 }),
}),
TERRAIN_HIGH_MOUNTAIN : ({
   ({ TILE_SNOW, 0, 20, 0, 0, 100 }),
   ({ TILE_ICE, 0, 20, 0, 0, 100 }),
   ({ TILE_ROCK, 0, 20, 0, 0, 100 }),
   ({ TILE_STONE_FLOOR, 0, 20, 0, 0, 100 }),
}),
TERRAIN_PLAIN : ({
   ({ TILE_GRASS, 0, 30, 0, 0, 100 }),
   ({ TILE_L_GRASS, 0, 25, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 20, 0, 0, 100 }),
   ({ TILE_S_BUSH, 0, 15, 0, 0, 100 }),
   ({ TILE_BUSH, 0, 12, 0, 0, 100 }),
}),
TERRAIN_GRASSLAND : ({
   ({ TILE_GRASS, 0, 35, 0, 0, 100 }),
   ({ TILE_L_GRASS, 0, 35, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 25, 0, 0, 100 }),
   ({ TILE_S_BUSH, 0, 20, 0, 0, 100 }),
}),
TERRAIN_MOOR : ({
   ({ TILE_GRASS, 0, 35, 0, 0, 100 }),
   ({ TILE_L_GRASS, 0, 15, 0, 0, 100 }),
   ({ TILE_S_BUSH, 0, 20, 0, 0, 100 }),
}),
TERRAIN_SAVANNAH : ({
   ({ TILE_GRASS, 0, 35, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 25, 0, 0, 100 }),
   ({ TILE_SAND, 0, 20, 0, 0, 100 }),
   ({ TILE_JUN_TREE, 0, 15, 0, 0, 100 }),
   ({ TILE_L_JUN_TREE, 0, 15, 0, 0, 100 }),
   ({ TILE_S_JUN_TREE, 0, 10, 0, 0, 100 }),
}),
TERRAIN_LOWLAND : ({
   ({ TILE_GRASS, 0, 40, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 40, 0, 0, 100 }),
   ({ TILE_L_GRASS, 0, 30, 0, 0, 100 }),
   ({ TILE_S_CON_TREE, 0, 25, 0, 0, 100 }),
   ({ TILE_L_CON_TREE, 0, 30, 0, 0, 100 }),
}),
TERRAIN_HIGHLAND : ({
   ({ TILE_GRASS, 0, 10, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 10, 0, 0, 100 }),
   ({ TILE_S_CON_TREE, 0, 5, 0, 0, 100 }),
   ({ TILE_S_DEC_TREE, 0, 5, 0, 0, 100 }),
}),
TERRAIN_VALLEY : ({
   ({ TILE_GRASS, 0, 50, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 50, 0, 0, 100 }),
}),
TERRAIN_OASIS : ({
   ({ TILE_SAND, 0, 25, 0, 0, 100 }),
   ({ TILE_GRASS, 0, 40, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 25, 0, 0, 100 }),
}),
TERRAIN_WATERFALL : ({
   ({ TILE_GRASS, 0, 50, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 50, 0, 0, 100 }),
}),
TERRAIN_BADLAND : ({
   ({ TILE_SAND, 0, 50, 0, 0, 100 }),
   ({ TILE_ROCK, 0, 50, 0, 0, 100 }),
}),
TERRAIN_DESERT : ({
   ({ TILE_SAND, 0, 10, 0, 0, 100 }),
   ({ TILE_S_BUSH, 0, 2, 0, 0, 100 }),
}),
TERRAIN_BEACH : ({
   ({ TILE_SAND, 0, 15, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 5, 0, 0, 100 }),
}),
TERRAIN_LAVA : ({
   ({ TILE_LAVA, 0, 30, 0, 0, 100 }),
   ({ TILE_ROCK, 0, 10, 0, 0, 100 }),
   ({ TILE_STALAGMITE, 0, 5, 0, 0, 100 }),
}),
TERRAIN_VOLCANO : ({
   ({ TILE_ROCK, 0, 40, 0, 0, 100 }),
   ({ TILE_STONE_FLOOR, 0, 25, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 25, 0, 0, 100 }),
}),
TERRAIN_SWAMP : ({
   ({ TILE_MARSH, 0, 50, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 40, 0, 0, 100 }),
   ({ TILE_S_BUSH, 0, 30, 0, 0, 100 }),
   ({ TILE_WATER, 0, 27, 0, 0, 100 }),
   ({ TILE_L_GRASS, 0, 35, 0, 0, 100 }),
}),
TERRAIN_DECIDUOUS_FOREST : ({
   ({ TILE_EARTH, 0, 20, 0, 0, 100 }),
   ({ TILE_S_DEC_TREE, 0, 12, 0, 0, 100 }),
   ({ TILE_DEC_TREE, 0, 15, 0, 0, 100 }),
   ({ TILE_L_DEC_TREE, 0, 11, 0, 0, 100 }),
}),
TERRAIN_DEEP_DECIDUOUS_FOREST : ({
   ({ TILE_EARTH, 0, 50, 0, 0, 100 }),
   ({ TILE_S_DEC_TREE, 0, 40, 0, 0, 100 }),
   ({ TILE_DEC_TREE, 0, 35, 0, 0, 100 }),
   ({ TILE_L_DEC_TREE, 28, 0, 0, 100 }),
}),
TERRAIN_CONIFEROUS_FOREST : ({
   ({ TILE_EARTH, 0, 50, 0, 0, 100 }),
   ({ TILE_S_CON_TREE, 0, 25, 0, 0, 100 }),
   ({ TILE_CON_TREE, 0, 30, 0, 0, 100 }),
   ({ TILE_L_CON_TREE, 0, 15, 0, 0, 100 }),
}),
TERRAIN_DEEP_CONIFEROUS_FOREST : ({
   ({ TILE_EARTH, 0, 50, 0, 0, 100 }),
   ({ TILE_S_CON_TREE, 0, 15, 0, 0, 100 }),
   ({ TILE_CON_TREE, 0, 30, 0, 0, 100 }),
   ({ TILE_L_DEC_TREE, 0, 25, 0, 0, 100 }),
}),
TERRAIN_JUNGLE : ({
   ({ TILE_EARTH, 0, 10, 0, 0, 100 }),
   ({ TILE_S_JUN_TREE, 0, 7, 0, 0, 100 }),
   ({ TILE_JUN_TREE, 0, 10, 0, 0, 100 }),
   ({ TILE_L_JUN_TREE, 0, 6, 0, 0, 100 }),
}),
TERRAIN_DEEP_JUNGLE : ({
   ({ TILE_EARTH, 0, 8, 0, 0, 100 }),
   ({ TILE_S_JUN_TREE, 0, 15, 0, 0, 100 }),
   ({ TILE_JUN_TREE, 0, 20, 0, 0, 100 }),
   ({ TILE_L_JUN_TREE, 0, 12, 0, 0, 100 }),
}),
TERRAIN_RIVER : ({
   ({ TILE_WATER, 1, 100, 0, 100, 100 }),
}),
TERRAIN_DEEP_RIVER : ({
   ({ TILE_D_WATER, 1, 100, 0, 70, 70 }),
}),
TERRAIN_LAKE : ({
   ({ TILE_D_WATER, 1, 100, 0, 50, 50 }),
}),
TERRAIN_SEA : ({
   ({ TILE_D_WATER, 1, 100, 0, 40, 60 }),
}),
TERRAIN_SHORE : ({
   ({ TILE_SAND, 0, 50, 0, 0, 100 }),
   ({ TILE_EARTH, 0, 50, 0, 0, 100 }),
}),
]);


void
create_areamap()
{
   map_array = allocate(620); /* An array to optimize get_area() FIXME: clean up not implemented */
   for (int i = 0; i<620;i++)
	 map_array[i] = allocate(620);
   
   set_worldmap(explode(read_file("/world/WORLDMAP"), "\n"));

   add_area(26, 55, "/world/hilltop/hilltop1.c");
   add_area(310, 290, "/world/bral_gaur/sentrance.c");
   add_area(311, 290, "/world/bral_gaur/entrance.c");
   add_area(481, 188, "/world/tod/temple.c");

   set_area_dimensions( 620000, 620000);
}

void
set_worldmap(string *map)
{
   worldmap = map;
}

varargs object
build_map(int x, int y, string area)
{
   object o;
   string *map;
   string options;
   string *chars = allocate(4);
   mixed m;
   mapping params = ([]);
   string c;
   int i, j;
   
//   y /= 10;
//   x /=10;	
   
   chars[0] = worldmap[y][x..x];
   chars[1] = worldmap[y][x+1..x+1];   
   chars[2] = worldmap[y+1][x..x];
   chars[3] = worldmap[y+1][x+1..x+1];
   
   
   /* Create empty parameter list */
   
   for (i = 0; i < 4; i++)
	 {
		for (j = 0; j < sizeof(terrains[chars[i][0]]); j++)
		  {
			 c = terrains[chars[i][0]][j][T_CHAR];
			 if (!params[c])		/* Initialize only if not defined */
			   params[c] = ({ 
				  0, 0, 0, 0, 
				  terrains[chars[i][0]][j][T_FLAGS],
				  terrains[chars[i][0]][j][T_FNUM], 
				  terrains[chars[i][0]][j][T_FPARAM1], 
				  terrains[chars[i][0]][j][T_FPARAM2]
			   });
			 params[c][i] = terrains[chars[i][0]][j][T_PROB];
		  }
		if (j == 0)		/* Unknown terrain */
			 params[TERRAIN_UNKNOWN] = ({ 1, 1, 1, 1, 1, 0, 0, 100 });
	 }
   
   options = sprintf("%d %d", sizeof(keys(params)), y*1024+x);
   for (i = 0; i < sizeof(keys(params)); i++)
	 {
		m = params[keys(params)[i]];
		options += sprintf(" %c %d %d %d %d %d %d %d %d",
						   keys(params)[i],
						   m[4], m[0], m[1], m[2], m[3],
						   m[5], m[6], m[7]);
	 }
   
   if (area)
	 o = clone_object (area);
   else
	 o = clone_object(GRIDMAP);
   
   /*
   write_file("/mapgen.debug", 
			  sprintf("(%d, %d) [%c%c%c%c]: %s\n", x, y, 
					  worldmap[y][x], worldmap[y][x+1], worldmap[y+1][x], worldmap[y+1][x+1],
					  options));
   write_file("/mapgen.debug", sprintf("%s\n", implode(map, "\n")));
	*/
   
   map = generate_map(options, 100, 100);
   
   o->set_gridmap(map);
   return o;
}

object /* Check if an gridmap is loaded */
area_loaded(int x, int y)
{
   if (map_array[y/10][x/10])
	 return map_array[y/10][x/10][y - y/10*10][x - x/10*10];
   return 0;
}


void /* Add an gridmap to table */
add_map(int x, int y, object o)
{
   int i;
   int cx, cy;
   
   cx = x/10;
   cy = y/10;
   x -= cx*10;
   y -= cy*10;
   
   if (!map_array[cy][cx])
	 {
		map_array[cy][cx] = allocate(10);
		for (i = 0; i<10; i++)
		  map_array[cy][cx][i] = allocate(10);
	 }
   map_array[cy][cx][y][x] = o;
   
}
 

object /* Get an area which contains the given coordinates */
get_area(int x, int y)
{
   string f;
   object o;
   
   if (x < 0 || y < 0 || x >= areamap_w || y >= areamap_h)
	 return 0;
   
   x = x/100;
   y = y/100;
   
   f = sprintf("%dx%dy", x, y);
   if (areas[f])
	 {
		if (!(o = area_loaded(x, y)))
		  {
			 o = THOB->build_map (x, y, areas[f]);
			 o->set_area_coordinates(x, y);
			 o->move(THOB);
			 add_map(x, y, o);
			 o->create_gridmap();
		  }
	 }
   else /* If the map wasn't loaded or cached, generate it */
	 { 
		o = THOB->build_map(x, y);
		o->set_area_coordinates(x, y);
		o->move(THOB);
		o->create_gridmap();
		add_map(x, y, o);
		add_area(x, y, file_name(o));
	 }
   
   return o;
   
}

