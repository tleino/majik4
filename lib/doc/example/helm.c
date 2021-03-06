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

#include <color.h>
#include <symbol.h>

inherit ARMOR;

void
create_armor()
{
   THOB->set_name("unholy helm");
   THOB->set_gridchar(IS_ARMOR);
   THOB->set_gridcolor(C_BLUE);
   THOB->set_target(({"head"}), ({ }));
   THOB->set_protection(([DT_IMPACT:100, DT_STAB:100, DT_CUT:100]));
   THOB->set_material("steel");
   THOB->set_dimension(50,50,40,90);

   THOB->set_speed_bonus(2);
   THOB->set_defence_bonus(10);
   THOB->set_offense_bonus(20);
   THOB->set_fatique_bonus(0);
}
