/*
  
  Tetris (Ein-/Ausgaben via MQTT)
  ===============================
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


#include "tetris_io.h"


extern struct mosquitto *mosq;
extern uint8_t mqtt_qos;



// ********************************************
void create_game_screen2mqtt(void)
{
	// MQTT...
	mosquitto_error_handling(mosquitto_publish(mosq, NULL, MQTT_TOPIC_CREATE_GAME_SCREEN, 2, "1", mqtt_qos, false));
}



// ********************************************
void destroy_game_screen2mqtt(void)
{
	// MQTT
	mosquitto_error_handling(mosquitto_publish(mosq, NULL, MQTT_TOPIC_DESTROY_GAME_SCREEN, 2, "1", mqtt_qos, false));	
}


// ********************************************
void score2mqtt(score_t *s)
{
	char buf[200];
	
	//...Zeit liegt in 100tel Sekunden vor
	sprintf(buf, "%u,%02u", (unsigned int)(s->game_time/100), (unsigned int)(s->game_time%100));
	mosquitto_error_handling( mosquitto_publish(mosq, NULL, MQTT_TOPIC_SCORE_TIME, strlen(buf), buf, mqtt_qos, false));
	// Level
	sprintf(buf, "%u", s->level);
	mosquitto_error_handling( mosquitto_publish(mosq, NULL, MQTT_TOPIC_SCORE_LEVEL, strlen(buf), buf, mqtt_qos, false));
	// Anzahl versenkte Spielsteine
	sprintf(buf, "%u", s->bricks);
	mosquitto_error_handling( mosquitto_publish(mosq, NULL, MQTT_TOPIC_SCORE_BRICKS, strlen(buf), buf, mqtt_qos, false));
	// Anzahl abgeraumte Zeilen
	sprintf(buf, "%u", s->lines);
	mosquitto_error_handling( mosquitto_publish(mosq, NULL, MQTT_TOPIC_SCORE_LINES, strlen(buf), buf, mqtt_qos, false));
	// Punkte
	sprintf(buf, "%u", s->points);
	mosquitto_error_handling( mosquitto_publish(mosq, NULL, MQTT_TOPIC_SCORE_POINTS, strlen(buf), buf, mqtt_qos, false));
	// Pause bzw. Spielende
	if (s->is_pause) {
		sprintf(buf, "%u", 1);
	} else 
	if (s->game_over) {
		sprintf(buf, "%u", 2);
	} else { 
		sprintf(buf, "%u", 0);
	}
	mosquitto_error_handling( mosquitto_publish(mosq, NULL, MQTT_TOPIC_GAME_STATUS, strlen(buf), buf, mqtt_qos, false));
	// wenn Spiel zuende, dann Endstand versenden
	if (s->game_over) {
		sprintf(buf,
		 		"{\"timestamp\":\"%lu\", \"time\":\"%lu\", \"level\":\"%u\", \"bricks\":\"%u\", \"lines\":\"%u\", \"points\":\"%u\"}",
		  		time(NULL), s->game_time, s->level, s->bricks,
		  		s->lines, s->points
		  		);
		mosquitto_error_handling(mosquitto_publish(mosq, NULL, MQTT_TOPIC_FINAL_SCORE, strlen(buf), buf, mqtt_qos, false));
	}
}


// ********************************************
void clear_brick_dummy(brick_t *b) 
{
	// MQTT
	// ...dummy
}

// ********************************************
void draw_brick2mqtt(brick_t *b, grid_t *g) 
{
	int i, j;
	char buf[GRID_DX*GRID_DY+1] = "";

	// MQTT...
	// ...zuerst das Grid generieren...
	memset(buf, 0, (GRID_DX*GRID_DY+1)*sizeof(char));
	for (i=0; i<GRID_DX; i++) {
		for (j=0; j<GRID_DY; j++) {
			buf[i*GRID_DY + j] = g->grid[i][j] + 0x30;
		}	
	}

	// ...und darueber den Stein
	for (i=0; i < b->dxy; i++) {
		for (j=0; j < b->dxy; j++) {
			// Spielsteinfeld nur uebernehmen, wenn nicht Blank (0)
			if (b->grid[j][i]) {
				buf[b->y+j + b->x*GRID_DY + i*GRID_DY] = b->grid[j][i] + 0x30;
			}


		}	
	}
	
	mosquitto_error_handling( mosquitto_publish(mosq, NULL, MQTT_TOPIC_GRID, strlen(buf), buf, mqtt_qos, false));
}


// ********************************************
void next_brick2mqtt(brick_t *b)
{
	int i, j;
	char buf[MAX_DXY*MAX_DXY+1] = "";

	// via MQTT senden
	for (i=0; i<MAX_DXY; i++) {
		for (j=0; j<MAX_DXY; j++) {
			sprintf(buf, "%s%i", buf, b->grid[i][j]);
		}	
	}
	mosquitto_error_handling( mosquitto_publish(mosq, NULL, MQTT_TOPIC_NEXT_BRICK, strlen(buf), buf, mqtt_qos, false));
}



// ********************************************
void draw_grid2mqtt(grid_t *g) 
{
	int i, j;
	char buf[GRID_DX*GRID_DY+1] = "";

	// MQTT
	memset(buf, 0, (GRID_DX*GRID_DY+1)*sizeof(char));
	for (i=0; i<GRID_DX; i++) {
		for (j=0; j<GRID_DY; j++) {
			buf[i*GRID_DY + j] = g->grid[i][j] + 0x30;
		}	
	}
	mosquitto_error_handling( mosquitto_publish(mosq, NULL, MQTT_TOPIC_GRID, strlen(buf), buf, mqtt_qos, false));
}

