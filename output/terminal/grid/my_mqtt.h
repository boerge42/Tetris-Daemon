/*
  
  ...ein paar MQTT-Routinen
  =========================
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <mosquitto.h>

// MQTT-Defaults
#define MQTT_HOST 		"localhost"
#define MQTT_PORT 		1883
#define MQTT_RETAIN		false
#define MQTT_QOS		0
#define MQTT_KEEPALIVE 	60
#define MQTT_CLIENT_ID	"tetris_grid"

// MQTT-Topics
#define MQTT_TOPIC_CREATE_GAME_SCREEN 	"tetris/create_game_screen"
#define MQTT_TOPIC_DESTROY_GAME_SCREEN	"tetris/destroy_game_screen"
#define MQTT_TOPIC_GRID					"tetris/grid"
#define MQTT_TOPIC_NEXT_BRICK		    "tetris/next_brick"
#define MQTT_TOPIC_GET_SCORE		    "tetris/get_score"

int mosquitto_error_handling(int error);
void mqtt_init(char *host, int port, char *user, char *pwd, uint8_t qos, char *id);
void mqtt_get_score(uint8_t qos);
void mqtt_clear(void);

#endif
