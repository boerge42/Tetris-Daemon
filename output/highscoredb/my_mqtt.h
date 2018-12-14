/*
  
  ...ein paar MQTT-Routinen
  =========================
       Uwe Berger; 2018


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
#define MQTT_CLIENT_ID	"highscoredb"

// MQTT-Topics
#define MQTT_TOPIC_FINAL_SCORE	"tetris/finalscore"
#define MQTT_TOPIC_LWT 			"tetris/status/highscoredb"


int mosquitto_error_handling(int error);
void mqtt_init(char *host, int port, char *user, char *pwd, uint8_t qos, char *id);
void mqtt_loop_forever(void);
void mqtt_clear(void);
void mqtt_set_lwt_topic_off(uint8_t qos);

#endif
