/*
  
  Tetris (Zufallsroutinen; Definitionen)
  ======================================
           Uwe Berger; 2015
 
 
 Interessante Links zum Thema Tetris:
 ------------------------------------
 --> http://www.colinfahey.com/tetris/tetris.html
 
 
  ---------
  Have fun! 
 
 
 *----------------------------------------------------------------------
 * Copyright (c) 2015 Uwe Berger - bergeruw(at)gmx.net
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *----------------------------------------------------------------------
*/

#ifndef _RANDOM_H
#define _RANDOM_H

#include <stdlib.h>
#include <time.h>
#include "tetris.h"

void random_init(void);
int generate_random_number(void);

#endif
