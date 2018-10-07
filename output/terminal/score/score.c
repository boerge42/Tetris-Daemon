/*
  
  Tetris-Scoreboard mit mcurses via MQTT
  ======================================
          Uwe Berger, 2108
  
 
  Spielstaende Tetris, Daten via MQTT.
  
  
  Aufrufparameter:
  ----------------
  Parameter				Default
  [-h <mqtt-host>]		localhost
  [-p <mqt-port>]		1883
  [-U <mqtt-user>] 		""
  [-P <mqtt-pwd>]		""
  [-q <mqtt-qosr>] 		0
  [-i <mqtt-id>] 		"tetris_score"

  abonnierte Topics:
  ------------------
  tetris/score/level
  tetris/score/bricks
  tetris/score/lines
  tetris/score/points
  tetris/score/time
  tetris/gamestatus


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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mosquitto.h>

#include "mcurses.h"
#include "my_mqtt.h"

// ************************************************
void mqtt_scoreboard_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    char **topics;
    int topic_count;
	char buf[9];

    mosquitto_error_handling(mosquitto_sub_topic_tokenise(message->topic, &topics, &topic_count));
    if (strcmp(topics[topic_count-1], "level") == 0) {
		sprintf(buf, "%8s", (char *)message->payload);
		mvaddstr(2, 11, buf);
	}
    if (strcmp(topics[topic_count-1], "bricks") == 0) {
		sprintf(buf, "%8s", (char *)message->payload);
		mvaddstr(4, 11, buf);
	}
    if (strcmp(topics[topic_count-1], "lines") == 0) {
		sprintf(buf, "%8s", (char *)message->payload);
		mvaddstr(6, 11, buf);
	}
    if (strcmp(topics[topic_count-1], "points") == 0) {
		sprintf(buf, "%8s", (char *)message->payload);
		mvaddstr(8, 11, buf);
	}
    if (strcmp(topics[topic_count-1], "time") == 0) {
		sprintf(buf, "%8s", (char *)message->payload);
		mvaddstr(10, 11, buf);
	}
    if (strcmp(topics[topic_count-1], "gamestatus") == 0) {
		switch(atoi(message->payload)) {
			case 0:		// ...laeuft
				mvaddstr(12, 1, "          ");
				break;
			case 1:		// ...Pause
				attrset(A_REVERSE);
				mvaddstr(12, 1, "Pause...  ");
				attrset(A_NORMAL);
				break;
			case 2:		// ...Game over
				attrset(A_REVERSE);
				mvaddstr(12, 1, "Game over!");
				attrset(A_NORMAL);
				break;
		}
	}
    mosquitto_error_handling(mosquitto_sub_topic_tokens_free(&topics, topic_count));
	fflush(stdout);
}

// ********************************************
// ********************************************
// ********************************************
int main(int argc, char **argv)
{
	int c;
	char mqtt_host[50]	= MQTT_HOST;
	int  mqtt_port    	= MQTT_PORT;
	char mqtt_user[50]	= "";
	char mqtt_pwd[50]	= "";
	uint8_t mqtt_qos    = MQTT_QOS;
	char mqtt_id[50]    = MQTT_CLIENT_ID;

	// Aufrufparameter auslesen/verarbeiten
	while ((c=getopt(argc, argv, "h:p:u:P:q:i:?")) != -1) {
		switch (c) {
			case 'h':
				if (strlen(optarg) >= sizeof mqtt_host) {
					puts("hostname too long!");
					exit(EXIT_FAILURE);
				} else {
					strncpy(mqtt_host, optarg, sizeof(mqtt_host));
				}
				break;
			case 'p':
				mqtt_port = atoi(optarg);
				break;
			case 'q':
				mqtt_qos = atoi(optarg);
				if (mqtt_qos < 0) mqtt_qos = 0;
				if (mqtt_qos > 2) mqtt_qos = 2;
				break;
			case 'u':
				if (strlen(optarg) >= sizeof mqtt_user) {
					puts("username too long!");
					exit(EXIT_FAILURE);
				} else {
					strncpy(mqtt_user, optarg, sizeof(mqtt_user));
				}
				break;
			case 'P':
				if (strlen(optarg) >= sizeof mqtt_pwd) {
					puts("password too long!");
					exit(EXIT_FAILURE);
				} else {
					strncpy(mqtt_pwd, optarg, sizeof(mqtt_pwd));
				}
				break;
			case 'i':
				if (strlen(optarg) >= sizeof mqtt_id) {
					puts("id too long!");
					exit(EXIT_FAILURE);
				} else {
					strncpy(mqtt_id, optarg, sizeof(mqtt_id));
				}
				break;
			case '?':
				puts("score [-h <mqtt-host>] [-p <mqtt-port>]");
				puts("      [-U <mqtt-user>] [-P <mqtt-pwd>]");
				puts("      [-q <mqtt-qosr>] [-i <mqtt-id>]");
				exit(0);
				break;
		}
	}

	// MQTT initialisieren
	mqtt_init(mqtt_host, mqtt_port, mqtt_user, mqtt_pwd, mqtt_qos, mqtt_id);
	
	// Screen aufbereiten/ausgeben	
	initscr();
	curs_set(0);
	attrset(A_REVERSE);
	mvaddstr(0,0, "Tetris-Scoreboard; Uwe Berger; 2018");
	attrset(A_NORMAL);
	mvaddstr(2, 1,  "Level...:");
	mvaddstr(4, 1,  "Bricks..:");
	mvaddstr(6, 1,  "Lines...:");
	mvaddstr(8, 1,  "Points..:");
	mvaddstr(10, 1, "Time [s]:");
	mvaddstr(15, 1, "'q' --> Exit");

	// Loop (mit getch()), bis Taste q betaetigt...
	while (c != 'q') c = getch();

	// Programm-Ende!
	attrset(A_NORMAL);
	clear();
	curs_set(1);
	endwin();

	mqtt_clear();

	return 0;
}


