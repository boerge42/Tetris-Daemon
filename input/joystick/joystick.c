/*
  
  Tetris-Input via Joystick
  =========================
      Uwe Berger, 2108
  
  
  
  Joystick-Eingabe fuer MQTT-Tetris.
  
  
  Aufrufparameter:
  ----------------
  Parameter				Default
  [-h <mqtt-host>]		localhost
  [-p <mqt-port>]		1883
  [-U <mqtt-user>] 		""
  [-P <mqtt-pwd>]		""
  [-i <mqtt-id>]		"tetris_joystick"
  [-q <mqtt-qosr>] 		0
  [-v]			 		no
  [-d]					no
  [-?]					no


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
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#include "my_mqtt.h"


struct js_event_t {
	unsigned int time;      // event timestamp in ms
	short value;   			// value 
	uint8_t type;           // event type 
	uint8_t number;         // axis/button number
};

#define JS_EVENT_BUTTON		0x01    // Button 
#define JS_EVENT_AXIS		0x02    // Axis

#define JS_DEVICE			"/dev/input/js0"


// Tastendefinitionen fuer Tetris
#define KEY_UNKNOWN			0
#define KEY_QUIT_GAME		1
#define KEY_PAUSE_GAME		2
#define KEY_NEW_GAME		3
#define KEY_BRICK_DROP		4
#define KEY_BRICK_ROTATE	5
#define KEY_BRICK_LEFT		6
#define KEY_BRICK_RIGHT		7

struct tetris_buttons_t {
	uint8_t js_type;
	uint8_t js_number;
	short 	js_value;
	uint8_t	tetris_key;
};


struct tetris_buttons_t tetris_buttons[] =
{
	{0, 0, 0, 		KEY_QUIT_GAME},
	{1, 6, 1, 		KEY_PAUSE_GAME},
	{1, 3, 1, 		KEY_PAUSE_GAME},
	{1, 7, 1, 		KEY_NEW_GAME},
	{1, 2, 1, 		KEY_NEW_GAME},
	{2, 7, 32767, 	KEY_BRICK_DROP},
	{2, 7, -32767, 	KEY_BRICK_ROTATE},
	{2, 6, -32767, 	KEY_BRICK_LEFT},
	{2, 6, 32767, 	KEY_BRICK_RIGHT}
};

#define SIZEOF_TETRIS_BUTTONS sizeof(tetris_buttons)/sizeof(tetris_buttons[0])

char mqtt_host[50]	= MQTT_HOST;
int  mqtt_port    	= MQTT_PORT;
char mqtt_user[50]	= "";
char mqtt_pwd[50]	= "";
uint8_t mqtt_qos    = MQTT_QOS;
char mqtt_id[50]    = MQTT_CLIENT_ID;
uint8_t daemonize   = 0;
uint8_t verbose     = 0;

extern struct mosquitto *mosq;



// ********************************************
void signal_handler(int sig)
{
	switch(sig) {
		case SIGHUP:
			syslog(LOG_INFO, "...receive  SIGHUP, what's up?");
			break;
		case SIGTERM:
			syslog(LOG_INFO, "Stopped with SIGTERM!");
			// Aufraeumen...
			mqtt_clear();
			closelog();
			exit(0);
			break;
	}
}

// ********************************************
void start_daemon (void) 
{
	int i;
	pid_t pid;
   
	// Elternprozess beenden, init uebernimmt
	if ((pid = fork ()) != 0) exit(EXIT_FAILURE);
	// Kindprozess uebernimmt
	if (setsid() < 0) exit(EXIT_FAILURE);
	// Kindprozess terminieren
	if ((pid = fork()) != 0) exit(EXIT_FAILURE);
	// Arbeitsverzeichnis "setzen" und Filezugriffsrechte setzen	
	if (chdir("/tmp")) exit(EXIT_FAILURE);
	umask(0);
	// alle offenen Files schliessen
	for (i = sysconf(_SC_OPEN_MAX); i>0; i--) close(i);
}



// ******************************************************
// ******************************************************
// ******************************************************
int main(int argc, char **argv)
{
	struct js_event_t e;
	int fd;
	uint8_t i, tetris_key;
	int c;
	char buf[10];
	
	
	// Aufrufparameter auslesen/verarbeiten
	while ((c=getopt(argc, argv, "h:p:u:P:q:i:vd?")) != -1) {
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
			case 'i':
				if (strlen(optarg) >= sizeof mqtt_id) {
					puts("id too long!");
					exit(EXIT_FAILURE);
				} else {
					strncpy(mqtt_id, optarg, sizeof(mqtt_id));
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
			case 'd':
				daemonize = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			case '?':
				puts("joystick [-h <mqtt-host>]  --> MQTT-Broker    (default: localhost)");
				puts("         [-p <mqtt-port>]  --> MQTT-Port      (default: 1883)");
				puts("         [-U <mqtt-user>]  --> MQTT-User      (default: \"\")"); 
				puts("         [-P <mqtt-pwd>]   --> MQTT-Pwd       (default: \"\")");
				puts("         [-q <mqtt-qos>]   --> MQTT-QoS       (default: 0)");
				puts("         [-i <mqtt-id>]    --> MQTT-Client-ID (default: tetrisd)");
				puts("         [-d]              --> ..as daemon    (default: no)");
				puts("         [-v]              --> verbose        (default: no)");
				puts("         [-?]              --> print this...");
				exit(0);
				break;
		}
	}
	
	
	// Programm daemonisieren
	if (daemonize) {
		// behandelte Signale initialisieren
		signal(SIGTERM, signal_handler);
		signal(SIGHUP, signal_handler);
		// "daemonisieren"
		start_daemon();
		// syslog
		openlog(argv[0], LOG_PID|LOG_CONS, LOG_DAEMON);
		syslog(LOG_INFO, "Started...!");
	}
	
	// MQTT initialisieren
	mqtt_init(mqtt_host, mqtt_port, mqtt_user, mqtt_pwd, mqtt_qos, mqtt_id);
    
    // Joystick-Device oeffnen
    fd = open (JS_DEVICE, O_RDONLY);
    if( fd < 0 ) {
        if (!daemonize) {
			printf("Cannot open: %s\n", JS_DEVICE);
		} else {
			syslog(LOG_INFO, "Error by open() joystick-device!");
		}
        exit(EXIT_FAILURE);
    }

    // loop forever...
    while( 1 )     {
        // Einlesen
        if (read(fd, &e, sizeof(e)) < 0) {
			if (!daemonize) {
				printf("Error %d by read() %s\n", errno, JS_DEVICE);
			} else {
				syslog(LOG_INFO, "Error by read() joystick-device!");
			}
			exit(EXIT_FAILURE);
		}
		// Tetristaste bestimmen, wenn Type Button oder Axis
		if( e.type == JS_EVENT_BUTTON || e.type == JS_EVENT_AXIS ) {
		
			i=0;
			tetris_key = KEY_UNKNOWN;
			while (i < SIZEOF_TETRIS_BUTTONS && tetris_key == KEY_UNKNOWN) {
				if (e.type   == tetris_buttons[i].js_type   &&
					e.number == tetris_buttons[i].js_number &&
					e.value  == tetris_buttons[i].js_value     ) {
					tetris_key = tetris_buttons[i].tetris_key;
				}
				i++;
			}
			// wenn eine Tetris-Taste, dann via MQTT senden
			if (tetris_key != KEY_UNKNOWN) {
				sprintf(buf, "%d", tetris_key);
				mosquitto_error_handling(
					mosquitto_publish(mosq, 
									NULL, 
									MQTT_TOPIC_KEY, 
									strlen(buf), 
									buf, 
									mqtt_qos, 
									false));
			}
			// evtl. Debugausgabe (Joysticktaste)
			if (verbose && !daemonize) printf("Type: %d, Number: %d, Value; %d, Tetris: %d\n", 
								e.type, e.number, e.value, tetris_key);
		}
    }
	mqtt_clear();
    return 0;
}

