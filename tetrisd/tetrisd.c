/*
  
         Tetris-Daemon
     ======================
     Uwe Berger; 2015, 2018
 
 
  Tetris-Engine, welche via MQTT-Nachrichten gesteuert wird und 
  selbst Spielfeld und Spielstand als MQTT-Nachrichten versendet.
  Das Programm kann als Deamon gestartet werden...
  
  
  Aufrufparameter:
  ----------------
  Parameter				Default
  [-h <mqtt-host>]		localhost
  [-p <mqt-port>]		1883
  [-U <mqtt-user>] 		""
  [-P <mqtt-pwd>]		""
  [-i <mqtt-id>]		"tetris_joystick"
  [-q <mqtt-qosr>] 		0
  [-i <mqtt-id>] 		"tetrisd"
  [-d]			 		no
  [-?]					no
 
 
 
 Interessante Links zum Thema Tetris:
 ------------------------------------
 --> http://www.colinfahey.com/tetris/tetris.html
 --> http://tetris.wiki
 
 
  ---------
  Have fun! 
 
 
 ToDo:
 -----
 --> Score bei "Game over" als JSON via MQTT
 
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <mosquitto.h>

#include "timer.h"
#include "tetris_io.h"
#include "random.h"
#include "my_mqtt.h"

#include "tetris.h"

static brick_t brick, next_brick;
static grid_t grid;
static score_t score = {1, 0, 0, 0, 0};
static int drop_key_pressed=0;

char mqtt_host[50]	= MQTT_HOST;
int  mqtt_port    	= MQTT_PORT;
char mqtt_user[50]	= "";
char mqtt_pwd[50]	= "";
uint8_t mqtt_qos    = MQTT_QOS;
char mqtt_id[50]    = MQTT_CLIENT_ID;
uint8_t daemonize   = 0;

uint8_t get_score	= 0;


// ********************************************
void timer_handler(void) 
{
	brick_t newbrick;
	static int free_iterations=0;
	static uint8_t z = 0x30;
	
	// kleine Animation, wenn Programm kein Service
	if (!daemonize) {
		printf("\b%c", z);
		fflush(stdout);
		if (z<0x39) {
			z++;
		} else {
			z=0x30;
		}
	}

	// Spielzeit
	if (!drop_key_pressed) 
		score.game_time = score.game_time + compute_iteration_time(&score)/10;
	draw_score(&score);
	// Stein eine Zeile runter?
	newbrick=brick;
	newbrick.y++;
	if (!collision(&newbrick, &grid)) {
			// ...ja, passt hin
			clear_brick(&brick);
			draw_brick(&newbrick, &grid);
			brick=newbrick;
			if (!drop_key_pressed) free_iterations++;											
	} else {
		// ...nein, Endlage erreicht!
		stop_timer();
		// Spielstein in Spielfeld uebernehmen
		copy_brick2grid(&brick, &grid);
		// Spielfeld auf vollständige Zeilen pruefen, ausgeben, Spielstand
		score.lines = score.lines + delete_full_lines(&grid);
		draw_grid(&grid);
		// Spielstand berechnen/ausgeben, Schalter initialisieren
		score.points=score.points+((21+(3*score.level))-free_iterations);
		compute_level(&score);
		draw_score(&score);
		free_iterations=0;
		drop_key_pressed=0;
		// neuen Spielstein generieren/anzeigen
		brick=next_brick;
		score.bricks++;
		next_brick=generate_new_brick();
		draw_next_brick(&next_brick);	
		if (collision(&brick, &grid)) {
			score.game_over=1;
			draw_score(&score);
		} else {

			draw_brick(&brick, &grid);	
			// ...und Timer wieder starten
			start_timer(compute_iteration_time(&score), &timer_handler);
		}
	}
}

// ********************************************
void game_init(void)
{
	int i, j;
	stop_timer();
	// Spielstaende nullen
	score.level=1;
	score.bricks=0;
	score.lines=0;
	score.points=0;
	score.game_time=0;
	score.is_pause=0;
	score.game_over=0;
	// Spielfeld initialisieren
	for (i=0; i<GRID_DX; i++) {
		for (j=0; j<GRID_DY; j++) {
			grid.grid[i][j]=BLANK;
		}	
	}
	// Flags initialisieren
	drop_key_pressed=0;
	// Spielfeld/Spielstand ausgeben
	draw_grid(&grid);
	draw_score(&score);
	// einen Spielstein generieren
	brick=generate_new_brick();
	score.bricks++;
	next_brick=generate_new_brick();
	draw_next_brick(&next_brick);
	draw_brick(&brick, &grid);
	// Timer starten
	start_timer(compute_iteration_time(&score), &timer_handler);
}

// ********************************************
int compute_iteration_time(score_t *s) 
{
	return (500-(s->level-1)*50);	
}

// ********************************************
void compute_level(score_t *s)
{
	if (s->lines <= 0) {
		s->level = 1;
	} else if ((s->lines >= 1) && (s->lines <= 90)) {
				s->level = 1 + ((s->lines - 1) / 10);
			} else if (s->lines >= 91) {
  						s->level = 10;
					}	
}


// ********************************************
int delete_full_lines(grid_t *g)
{
	int count=0;
	int full;
	int i, j, k;
	for (j=0; j<GRID_DY; j++) {
		full=1;
		for (i=0; i<GRID_DX; i++) { 
			if (g->grid[i][j]==BLANK) full=0;
		}
		// volle Zeile gefunden
		if (full) {
			count++;
			// drueber liegende Zeilen nach unten verschieben
			for (k=j; k>-1; k--) {
				for (i=0; i<GRID_DX; i++) {
					if (k>0) {
						g->grid[i][k]=g->grid[i][k-1];
					} else {
						g->grid[i][k]=BLANK;
					}
				}
			}
			// Zeile muss nochmal untersucht werden
			//j++;
		}
	}
	return count;
}

// ********************************************
brick_t generate_new_brick(void) 
{
	return default_bricks[generate_random_number()];
}

// ********************************************
void copy_brick2grid(brick_t *b, grid_t *g)
{
	int i, j;
	for (i=0; i < b->dxy; i++) {
		for (j=0; j < b->dxy; j++) {
			// nur uebernehmen, wenn Spielsteinfeld belegt
			if (b->grid[i][j]!=BLANK) 
				g->grid[(j+b->x)][(i+b->y)] = b->grid[i][j];
		}	
	}
}

// ********************************************
brick_t rotate_brick_cw(brick_t b)
{
	brick_t new_b=b;
	int i, j;
	for (i=0; i<b.dxy; i++) {
		for (j=0; j<b.dxy; j++) {
			new_b.grid[i][j]=b.grid[b.dxy-j-1][i];
		}	
	}
	return new_b;	
}


// ********************************************
int collision(brick_t *b, grid_t *g)
{
	int i, j;
	// alle Positionen des Spielsteines pruefen
	for (i=0; i < b->dxy; i++) {
		for (j=0; j < b->dxy; j++) {
			if (b->grid[i][j]!=BLANK) {
				// ...linke Seite
				if ((b->x+j)<0) return 1;
				// ...rechte Seite ((b->x+b->dxy-(b->dxy-j-1))>GRID_DX)
				if ((b->x+j+1)>GRID_DX) return 1;
				// ...oben
				if ((b->y+i)<0) return 1;
				// ...unten
				if ((b->y+i+1)>GRID_DY) return 1;
				// ...ein Feld belegt
				if (g->grid[b->x+j][b->y+i]!=BLANK) return 1;
			}
		}
	}
	// theoretisch fehlt noch eine Pruefung, ob bei
	// der Rotation Steine im Weg sind...!
	// Bei Drehung im Uhrzeigersinn:
	// * alter/neuer Ziegel zusammen 
	// * in den dann freien Feldern rechts oben und
	//   links unten darf kein Stein auf dem Spielfeld
	//   sein 
	return 0;
} 


// ************************************************
void mqtt_key_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{

	brick_t newbrick;

    if (strcmp(message->topic, MQTT_TOPIC_GET_SCORE) == 0) {
		// aktuellen Spielstand angefordert
		get_score = 1;
		create_game_screen();
		draw_score(&score);
		get_score = 0;
		draw_grid(&grid);
		draw_next_brick(&next_brick);
	} else {
		// Tastensteuerung		
		switch (atoi(message->payload)) {
			// Spielende
			case KEY_QUIT_GAME:
					//run=0;
					break;
			// neues Spiel
			case KEY_NEW_GAME:
					game_init();
					create_game_screen();
					break;
			// Spielpause
			case KEY_PAUSE_GAME:
					if (!score.game_over) {
						if (score.is_pause) {
							score.is_pause=0;
							start_timer(compute_iteration_time(&score), &timer_handler);
						} else {
							score.is_pause=1;
							stop_timer();
						}
					}
					draw_score(&score);
					break;
			// Spielstein drehen
			case KEY_BRICK_ROTATE:
					if (score.is_pause || score.game_over) break;
					newbrick=rotate_brick_cw(brick);
					if (!collision(&newbrick, &grid)) {
						clear_brick(&brick);
						draw_brick(&newbrick, &grid);
						brick=newbrick;
					}
					break;
			// Spielstein runterfallen lassen
			case KEY_BRICK_DROP:
					if (score.is_pause || score.game_over) break;
					stop_timer();
					drop_key_pressed=1;
					// Schnelldurchlauf
					start_timer(5, &timer_handler);
					break;
			// Spielstein nach links
			case KEY_BRICK_LEFT:
					if (score.is_pause || score.game_over) break;
					newbrick=brick;
					newbrick.x--;
					if (!collision(&newbrick, &grid)) {
						clear_brick(&brick);
						draw_brick(&newbrick, &grid);
						brick=newbrick;											
					};
					break;
			// Spielstein nach rechts
			case KEY_BRICK_RIGHT:
					if (score.is_pause || score.game_over) break;
					newbrick=brick;
					newbrick.x++;
					if (!collision(&newbrick, &grid)) {
						clear_brick(&brick);
						draw_brick(&newbrick, &grid);
						brick=newbrick;											
					};
					break;
		}
	}
}

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
			stop_timer();
			destroy_game_screen();
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


// ********************************************
// ********************************************
// ********************************************
int main(int argc, char **argv)
{
	
	int c;

	// Aufrufparameter auslesen/verarbeiten
	while ((c=getopt(argc, argv, "h:p:u:P:q:i:d?")) != -1) {
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
			case '?':
				puts("tetrisd [-h <mqtt-host>]  --> MQTT-Broker    (default: localhost)");
				puts("        [-p <mqtt-port>]  --> MQTT-Port      (default: 1883)");
				puts("        [-U <mqtt-user>]  --> MQTT-User      (default: \"\")"); 
				puts("        [-P <mqtt-pwd>]   --> MQTT-Pwd       (default: \"\")");
				puts("        [-q <mqtt-qos>]   --> MQTT-QoS       (default: 0)");
				puts("        [-i <mqtt-id>]    --> MQTT-Client-ID (default: tetrisd)");
				puts("        [-d]              --> ...as daemon   (default: no)");
				puts("        [-?]              --> print this...");
				exit(0);
				break;
		}
	}
	
	// Tetris-Engine daemonisieren
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

	// Initialisierung Zufallsgenerator
	random_init();
	
	// Spiel aufbauen
	create_game_screen();
	
	// Spiel initialisieren
	game_init();
	
	// forever loop...
	while(1) sleep(1);

	return 0;

}


