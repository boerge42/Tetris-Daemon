/*
  
  Tetris-Input via Keyboard
  =========================
      Uwe Berger, 2108
  
  
  Keyboard-Eingabe fuer MQTT-Tetris.
  
  Tasten: 
  -------
  KEY_QUIT_GAME		--> q, Q, F9
  KEY_PAUSE_GAME	--> p, P
  KEY_NEW_GAME		--> n, N
  KEY_BRICK_DROP	--> Kursor runter
  KEY_BRICK_ROTATE	--> Kursor hoch
  KEY_BRICK_LEFT    --> Kursor links
  KEY_BRICK_RIGHT   --> Kursor rechts
 
  Aufrufparameter:
  ----------------
  Parameter				Default
  [-h <mqtt-host>]		localhost
  [-p <mqt-port>]		1883
  [-U <mqtt-user>] 		""
  [-P <mqtt-pwd>]		""
  [-i <mqtt-id>]		"tetris_keyboard"
  [-q <mqtt-qosr>] 		0

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
#include "timer.h"


// Tastendefinitionen
#define KEY_UNKNOWN			0
#define KEY_QUIT_GAME		1
#define KEY_PAUSE_GAME		2
#define KEY_NEW_GAME		3
#define KEY_BRICK_DROP		4
#define KEY_BRICK_ROTATE	5
#define KEY_BRICK_LEFT		6
#define KEY_BRICK_RIGHT		7

// MQTT-Defaults
#define MQTT_HOST 		"localhost"
#define MQTT_PORT 		1883
#define MQTT_RETAIN		false
#define MQTT_QOS		0
#define MQTT_KEEPALIVE 	60
#define MQTT_CLIENT_ID	"tetris_keyboard"
#define MQTT_TOPIC		"tetris/key"

// ********************************************
void key_screen(uint8_t key)
{
	if (key == KEY_BRICK_LEFT) attrset(A_REVERSE); else attrset(A_NORMAL);
	mvaddstr(2,0,  "    Left   ");
	if (key == KEY_BRICK_ROTATE) attrset(A_REVERSE); else attrset(A_NORMAL);
	mvaddstr(2,11, "   Rotate  ");
	if (key == KEY_BRICK_RIGHT) attrset(A_REVERSE); else attrset(A_NORMAL);
	mvaddstr(2,22, "   Right   ");
	if (key == KEY_BRICK_DROP) attrset(A_REVERSE); else attrset(A_NORMAL);
	mvaddstr(3,11, "   Drop   ");
	if (key == KEY_NEW_GAME) attrset(A_REVERSE); else attrset(A_NORMAL);
	mvaddstr(4,0,  "    New     ");
	if (key == KEY_PAUSE_GAME) attrset(A_REVERSE); else attrset(A_NORMAL);
	mvaddstr(4,11, "   Pause    ");
	if (key == KEY_QUIT_GAME) attrset(A_REVERSE); else attrset(A_NORMAL);
	mvaddstr(4,22, "   Quit     ");

	attrset(A_NORMAL);
	mvaddstr(6,0, "F10 --> Exit");	
}

// ************************************************
int mosquitto_error_handling(int error)
{
	switch(error)
    {
        case MOSQ_ERR_SUCCESS:
			return 0;
            break;
        case MOSQ_ERR_INVAL:
        case MOSQ_ERR_NOMEM:
        case MOSQ_ERR_NO_CONN:
        case MOSQ_ERR_PROTOCOL:
        case MOSQ_ERR_PAYLOAD_SIZE:
		case MOSQ_ERR_CONN_LOST:
		case MOSQ_ERR_NOT_SUPPORTED:
		case MOSQ_ERR_ERRNO:
				fprintf(stderr, "Mosquitto-Error(%i): %s\n", error, mosquitto_strerror(errno));
				// Programm-Ende!
				attrset(A_NORMAL);
				clear();
				curs_set(1);
				endwin();
				exit(EXIT_FAILURE);
				break;
    }
	return 0;
}

// ********************************************
void timer_handler(void) 
{
	stop_timer();
	key_screen(KEY_UNKNOWN);
}

// ********************************************
// ********************************************
// ********************************************
int main(int argc, char **argv)
{
	uint8_t run = 1;
	uint8_t key = 0;
	int c;
	struct mosquitto *mosq = NULL;
	char mqtt_host[50]	= MQTT_HOST;
	int  mqtt_port    	= MQTT_PORT;
	char mqtt_user[50]	= "";
	char mqtt_pwd[50]	= "";
	uint8_t mqtt_qos    = MQTT_QOS;
	char mqtt_id[50]    = MQTT_CLIENT_ID;
	char buf[10]		= "";

	// Aufrufparameter auslesen/verarbeiten
	while ((c=getopt(argc, argv, "h:p:u:P:q:?")) != -1) {
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
				puts("input_keyboard [-h <mqtt-host>] [-p <mqtt-port>]");
				puts("               [-U <mqtt-user>] [-P <mqtt-pwd>]");
				puts("               [-q <mqtt-qosr>]");
				exit(0);
				break;
		}
	}
	
	// Init Mosquitto-Lib...
    mosquitto_lib_init();
    // einen Mosquitto-Client erzeugen
    mosq = mosquitto_new(mqtt_id, true, NULL);
    if( mosq == NULL )
    {
        switch(errno){
            case ENOMEM:
                fprintf(stderr, "Error: Out of memory.\n");
                break;
            case EINVAL:
                fprintf(stderr, "Error: Invalid id and/or clean_session.\n");
                break;
        }
        mosquitto_lib_cleanup();
        exit(EXIT_FAILURE);
    }
	// MQTT-User/Pwd
	mosquitto_error_handling(mosquitto_username_pw_set(mosq, mqtt_user, mqtt_pwd));
	// mit MQTT-Broker verbinden
    mosquitto_error_handling(mosquitto_connect(mosq, mqtt_host, mqtt_port, MQTT_KEEPALIVE));
	// MQTT-Loop starten
	mosquitto_error_handling(mosquitto_loop_start(mosq));
	
	// Screen aufbereiten/ausgeben	
	initscr();
	// Ego befriedigen...
	attrset(A_REVERSE);
	mvaddstr(0,0, "Tetris-Keyboard; Uwe Berger; 2018");
	// Keys initial ausgeben	
	key_screen(KEY_UNKNOWN);
	// Kursor aus
	curs_set(0);
	// Schleife bis Programm-Ende --> F10
	while (run) {
		// welche Taste?
		switch(getch()) {
			// Programm-Ende!
			case KEY_F(10):
					run = 0;	
					break;	
					
			// Spiel --> Ende
			case 'q': 
			case 'Q':
			case KEY_F(9):
					key=KEY_QUIT_GAME;	
					break;
					
			// Spiel --> Pause
			case 'p': 
			case 'P':
					key=KEY_PAUSE_GAME;	
					break;
			// Spiel --> neues Spiel
			case 'n': 
			case 'N':
					key=KEY_NEW_GAME;	
					break;
					
			// Spiel --> Stein runterfallen lassen
			case KEY_DOWN:
					key=KEY_BRICK_DROP;	
					break;
					
			// Spiel --> Stein drehen
			case KEY_UP:
					key=KEY_BRICK_ROTATE;	
					break;
					
			// Spiel --> Stein nach links verschieben
			case KEY_LEFT:
					key=KEY_BRICK_LEFT;	
					break;
					
			// Spiel --> Stein nach rechts verschieben
			case KEY_RIGHT:
					key=KEY_BRICK_RIGHT;	
					break;
			
			// ...komische Taste!
			default:
					key=KEY_UNKNOWN;	
					break;					
		}
		
		if (key != KEY_UNKNOWN) {
			// MQTT-Nachricht senden
			sprintf(buf, "%i", key);
			mosquitto_error_handling(mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(buf), buf, mqtt_qos, false));
			// Bildschirmausgabe
			key_screen(key);
			stop_timer();
			start_timer(100, &timer_handler);
		}
	}
	// Programm-Ende!
	attrset(A_NORMAL);
	clear();
	curs_set(1);
	endwin();
	mosquitto_loop_start(mosq);
	mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

	return 0;
}


