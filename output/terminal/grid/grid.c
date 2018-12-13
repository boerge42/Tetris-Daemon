/*
  
  Tetris-Grid mit mcurses via MQTT
  ================================
          Uwe Berger, 2018
  
 
  Darstellung Tetris-Spielfeld, Daten via MQTT-Nachrichten.
  
  
  Aufrufparameter:
  ----------------
  Parameter				Default
  [-h <mqtt-host>]		localhost
  [-p <mqt-port>]		1883
  [-U <mqtt-user>] 		""
  [-P <mqtt-pwd>]		""
  [-q <mqtt-qosr>] 		0
  [-i <mqtt-id>]		"tetris_grid"


  abonnierte Topics:
  ------------------
 	"tetris/create_game_screen"
	"tetris/destroy_game_screen"
	"tetris/grid"
    "tetris/next_brick"

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

// Farbdefinitionen
#define BLANK		'0'
#define GREEN		'1'
#define BLUE		'2'
#define YELLOW		'3'
#define RED			'4'
#define BROWN		'5'
#define MAGENTA		'6'
#define CYAN		'7'

// Groesse Spielfeld 
#define GRID_DX		10
#define GRID_DY		20

// Position Spielfeld 
#define GRID_X		5
#define GRID_Y		3

#define MAX_DXY		4



// ********************************************
static void mcurses_set_color(char c)
{
	switch (c) {
		case BLANK:    attrset(A_NORMAL);	break;
		case GREEN:    attrset(B_GREEN);	break;
		case BLUE :    attrset(B_BLUE);		break;
		case YELLOW:   attrset(B_WHITE);	break;
		case RED:      attrset(B_RED);		break;
		case BROWN:    attrset(B_BROWN);	break;
		case MAGENTA:  attrset(B_MAGENTA);	break;
		case CYAN:     attrset(B_CYAN);		break;
	}
}


// ************************************************
void mqtt_grid_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    char **topics;
    int topic_count, i, j;
	char buf[GRID_DX*GRID_DY+1];

    mosquitto_error_handling(mosquitto_sub_topic_tokenise(message->topic, &topics, &topic_count));

    if (strcmp(topics[topic_count-1], "create_game_screen") == 0) {
		// Spielfeld zeichnen...
		attrset(A_NORMAL);
		for (i=GRID_Y; i<(GRID_Y+GRID_DY); i++) {
			mvaddch(i, GRID_X-1, ACS_VLINE);
			mvaddch(i, GRID_X+GRID_DX, ACS_VLINE);
		}
		mvaddch(GRID_Y+GRID_DY, GRID_X-1, ACS_LLCORNER);
		mvaddch(GRID_Y+GRID_DY, GRID_X+GRID_DX, ACS_LRCORNER);
		for (i=GRID_X; i<GRID_X+GRID_DX; i++) {
		mvaddch(GRID_Y+GRID_DY, i, ACS_HLINE);
		}
	}

    if (strcmp(topics[topic_count-1], "next_brick") == 0) {
		// naechsten Stein zeichnen		
		memset(buf, 0, (MAX_DXY*MAX_DXY+1)*sizeof(char));
        memcpy(buf, message->payload, MAX_DXY*MAX_DXY*sizeof(char));
		for (i=0; i<MAX_DXY; i++) {
			for (j=0; j<MAX_DXY; j++) {
				mcurses_set_color(buf[i*MAX_DXY+j]);
				mvaddch((4+i), (20+j), ' '); 
				attrset(A_NORMAL);			
			}	
		}
	}

    if (strcmp(topics[topic_count-1], "destroy_game_screen") == 0) {
	}

    if (strcmp(topics[topic_count-1], "grid") == 0) {
		// Steine im Spielfeld zeichnen
		memset(buf, 0, (GRID_DX*GRID_DY+1)*sizeof(char));
        memcpy(buf, message->payload, GRID_DX*GRID_DY*sizeof(char));
		//puts(buf);
		for (i=0; i<GRID_DX*GRID_DY; i++) {
			mcurses_set_color(buf[i]);
			mvaddch((GRID_Y+i%GRID_DY), (GRID_X+i/GRID_DY), ' '); 
			attrset(A_NORMAL);
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
				puts("grid [-h <mqtt-host>] [-p <mqtt-port>]");
				puts("     [-U <mqtt-user>] [-P <mqtt-pwd>]");
				puts("     [-q <mqtt-qosr>] [-i <mqtt-id>]");
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
	mvaddstr(0,0, "Tetris-Grid; Uwe Berger; 2018");
	attrset(A_NORMAL);

	// aktuellen Speilstand etc. anfordern
	mqtt_get_score(mqtt_qos);

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


