/*
  
  Tetris (Ein-/Ausgaben via MQTT; Definitionen)
  ============================================
             Uwe Berger; 2015, 2018


  ---------
  Have fun! 
 
 *----------------------------------------------------------------------
 * Copyright (c) 2015, 2018 Uwe Berger - bergeruw(at)gmx.net
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

#ifndef _TETRIS_IO_H
#define _TETRIS_IO_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <mosquitto.h>

#include "tetris.h"
#include "my_mqtt.h"


#define draw_brick			draw_brick2mqtt
#define draw_grid			draw_grid2mqtt
#define draw_score			score2mqtt
#define draw_next_brick		next_brick2mqtt
#define destroy_game_screen	destroy_game_screen2mqtt
#define create_game_screen	create_game_screen2mqtt
#define clear_brick			clear_brick_dummy


// Forwards
void create_game_screen(void);
void destroy_game_screen(void);	
void draw_brick(brick_t *b, grid_t *g);
void clear_brick(brick_t *b);
void draw_next_brick(brick_t *b);
void draw_grid(grid_t *g);
void draw_score(score_t *s);

#endif
