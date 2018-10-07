/*
  
  Tetris (MQTT; Definitionen)
  ===========================
       Uwe Berger; 2018

  ---------
  Have fun! 
 
 *----------------------------------------------------------------------
 * Copyright (c) 2018 Uwe Berger - bergeruw(at)gmx.net
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

#ifndef _MY_MQTT_H
#define _MY_MQTT_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <syslog.h>
#include <mosquitto.h>

// MQTT-Defaults
#define MQTT_HOST 		"localhost"
#define MQTT_PORT 		1883
#define MQTT_RETAIN		false
#define MQTT_QOS		0
#define MQTT_KEEPALIVE 	60
#define MQTT_CLIENT_ID	"tetrisd"

// MQTT-Topics
#define MQTT_TOPIC_KEY	                "tetris/key"
#define MQTT_TOPIC_SCORE_TIME 			"tetris/score/time"
#define MQTT_TOPIC_SCORE_LEVEL 			"tetris/score/level"
#define MQTT_TOPIC_SCORE_BRICKS 		"tetris/score/bricks"
#define MQTT_TOPIC_SCORE_LINES 			"tetris/score/lines"
#define MQTT_TOPIC_SCORE_POINTS 		"tetris/score/points"
#define MQTT_TOPIC_GAME_STATUS 			"tetris/gamestatus"
#define MQTT_TOPIC_FINAL_SCORE    		"tetris/finalscore"
#define MQTT_TOPIC_NEXT_BRICK  			"tetris/next_brick"
#define MQTT_TOPIC_CREATE_GAME_SCREEN 	"tetris/create_game_screen"
#define MQTT_TOPIC_DESTROY_GAME_SCREEN	"tetris/destroy_game_screen"
#define MQTT_TOPIC_GRID					"tetris/grid"

int mosquitto_error_handling(int error);
void mqtt_init(char *host, int port, char *user, char *pwd, uint8_t qos, char *id);
void mqtt_clear(void);

#endif
