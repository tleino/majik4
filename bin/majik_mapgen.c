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

#include "../lpc_incl.h"

#define		MAX_TERRAINS	32			/* Maximum amount of parameters */
#define		OPTIONLENGTH	4096		/* Maximum length of option string */

/* Terrain parameter flags */
#define		TP_NOTHING		0x00
#define		TP_NORANDOM		0x01

/* Parameter order */
#define		NW		0
#define		NE		1
#define		SW		2
#define		SE		3


#ifdef PACKAGE_MAJIK_MAPGEN

const static char rcsid[] = "$Id$";

typedef struct
{
   char t;
   int flags;
   float (*BF)(float, float, float, float, float);
   float fparams[2];
   
   float p[4];
} TERRAINPARAM;

unsigned int seed = 0x42C574FA;

static float   Random()
{
   seed *= 0x9C72A295;
   return ((seed & 0xFFFF) / 65536.0);
}

static int highest (float *f, int a)
{
   int i, j;
   float max = 0.0;
   
   j = 0;
   for (i = 0; i < a; i++)
	 {
		if (f[i] > max)
		  {
			 max = f[i];
			 j = i;
		  }
	 }
   return j;
}

static float	BlendLinear (float x, float b1, float b2, float v1, float v2)
{
   if (x < b1)
	 return v1;
   if (x > b2)
	 return v2;
   return v1 * (b2 - x) / (b2 - b1) + v2 * (x - b1) / (b2 - b1);
}

static float	BlendSaw (float x, float pos, float w, float v1, float v2)
{
   if (x < pos - w)
	 return v1;
   if (x > pos + w)
	 return v2;
   if (x <= pos)
	 return v1 * (pos - x) / w + 1.0 * (x - (pos-w)) / w;
   if (x > pos)
	 return 1.0 * ((pos+w) - x) / w + v2 * (x - pos) / w;
}

static float  B (float x, int a)
{
   if (a != 3)
	 return x;
   else
	 {
		if (x < 0.3)
		  return 0;
		if (x >= 0.3 && x < 0.6)
		  return (x-0.3)*(1.0/0.3);
		return 1.0;
	 }
}

static void InterpolateMap (char *map, int params, TERRAINPARAM *t, int width, int height)
{
   int x, y, i, j, pos;
   float f, v;
   float dx, ddx, dy;
   float ymult[4][4];
   float *prob = (float *)malloc(width * height * params * sizeof(float));

   for (y = 0; y < height; y++)
	 {
		dy = (float)y / (height-1);
		ddx = (1.0 / (width - 1));
		dx = 0;
		for (i = 0; i < params; i++)
		  {
			 ymult[i][NW] = t[i].BF(dy, t[i].fparams[0], t[i].fparams[1], t[i].p[NW], t[i].p[SW]);
			 ymult[i][SW] = t[i].BF(dy, t[i].fparams[0], t[i].fparams[1], t[i].p[SW], t[i].p[NW]);
			 ymult[i][NE] = t[i].BF(dy, t[i].fparams[0], t[i].fparams[1], t[i].p[NE], t[i].p[SE]);
			 ymult[i][SE] = t[i].BF(dy, t[i].fparams[0], t[i].fparams[1], t[i].p[SE], t[i].p[NE]);

//			 printf("\n\n%f %f\n%f %f\n", ymult[i][NW], ymult[i][NE],
//					ymult[i][SW], ymult[i][SE]);
		  }
		
		for (x = 0; x < width; x++)
		  {
			 for (i = 0; i < params; i++)
			   {
				  f = t[i].BF(dx, t[i].fparams[0], t[i].fparams[1], ymult[i][NW], ymult[i][NE]);

				  prob[y*(width*params) + x*params + i] = f;
				  
				  if (!(t[i].flags & TP_NORANDOM))
					prob[y*(width*params) + x*params + i] *= Random();
//				  printf("%f ", prob[y*(width*params) + x*params + i]);
			   }
			 dx += ddx;
		 }
	}
   for (y = 0; y < height; y++)
	 {
		for (x = 0; x < width; x++)
		  {
			 pos = y * width * params + x * params;
			 j = highest(&prob[pos], params);
			 map[y*(width+1) + x] = t[j].t;
		  }
		map[y*(width+1)+width] = ';';
	 }
   free(prob);
}

static int SkipSpaces (char *str, int num)
{
   int pos = 0;
   while (num > 0)
	 {
		while (str[pos] != ' ' && str[pos] != 0)
		  pos++;
		num--;
		pos++;
	 }
   return pos;
}

void
f_generate_map PROT(( void ))
{
   /* 'options' string format: '
	* <number of terrains (NOT)> <terrain data x NOT>
	* <terrain data> = <char> <flags> <probabilities> <function def>
	* <function def> = <function type> <function parameters>
	* */
   int num_terrains, i, j, func, pos;
   int nw, ne, sw, se, f1, f2;
   int height, width;
   TERRAINPARAM *tparams;

   char* map;
   char* options;
   array_t *map_array;
   
   height = (sp--)->u.number;
   width = (sp--)->u.number;
   options = sp->u.string;
   
   
   map = new_string((width+1)*height, "mapgen");
   //   map = (char *)malloc((width+1) * height * sizeof(char)+1);

   // Parse the initial parameters
   sscanf(options, "%d %d", &num_terrains, &seed);
   
   pos = SkipSpaces(options, 2);
   
   if (num_terrains > MAX_TERRAINS)
	 num_terrains = MAX_TERRAINS;		// Prevent some bugs
   
   tparams = (TERRAINPARAM *)malloc(num_terrains * sizeof(TERRAINPARAM));
   
   for (i = 0; i < num_terrains; i++)
	 {
		if (sscanf(&options[pos], "%c %d %d %d %d %d %d %d %d",
				   &tparams[i].t,
				   &tparams[i].flags, 
				   &nw, &ne, &sw, &se,
				   &func,
				   &f1, &f2) != 9)
//		  fprintf(stderr, "mapgen: parameter parsing bug.\n");
		  error("mapgen: parameter parsing bug.\n");
		  
		tparams[i].p[0] = (float)nw / 100.0;	   
		tparams[i].p[1] = (float)ne / 100.0;	   
		tparams[i].p[2] = (float)sw / 100.0;	   
		tparams[i].p[3] = (float)se / 100.0;
		tparams[i].fparams[0] = (float)f1 / 100.0;
		tparams[i].fparams[1] = (float)f2 / 100.0;

		switch (func)
		  {
		   case 0: 
			 tparams[i].BF = BlendLinear;
//			 printf("mapgen: using linear blending function (%f-%f).\n", tparams[i].fparams[0], tparams[i].fparams[1]);
			 break;
		   case 1: 
			 tparams[i].BF = BlendSaw; 	
//			 printf("mapgen: using saw blending function.\n");
			 break;
		   default: tparams[i].BF = BlendLinear; break;
		  }
		pos += SkipSpaces(&options[pos], 9);
	 }

//   printf("mapgen: generating %dx%d map, %d parameters, seed %d\n", 
//		   width, height, num_terrains, seed);
   
   InterpolateMap(map, num_terrains, tparams, width, height);
   free(tparams);
   
/*   map_array = allocate_empty_array(height);
   free_string_svalue(map_array);
   for (j = 0; j < height; j++)
	 {
		map_array->item[j].type = T_STRING;
		map_array->item[j].subtype = STRING_MALLOC;
		map_array->item[j].u.string = new_string(width, "majik_mapgen");
		for (i = 0; i < width; i++)
		  map_array->item[j].u.string[i] = map[ j * width + i];
	 }
  */ 
//   free(map);
   map[(width+1)*height] = '\0';
   map_array = explode_string(map, strlen(map), ";", 1);
   free_string_svalue(sp);
   put_array(map_array);
   //   put_malloced_string(map);
}

#endif
