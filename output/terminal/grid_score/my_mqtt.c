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


#include "my_mqtt.h"


struct mosquitto *mosq = NULL;

extern void mqtt_grid_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);

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
				exit(EXIT_FAILURE);
				break;
    }
	return 0;
}

// ************************************************
void mqtt_init(char *host, int port, char *user, char *pwd, uint8_t qos, char *id)
{
	// Init Mosquitto-Lib...
    mosquitto_lib_init();
    // einen Mosquitto-Client erzeugen
    mosq = mosquitto_new(id, true, NULL);
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
	mosquitto_error_handling(mosquitto_username_pw_set(mosq, user, pwd));
	// Subscribe-Callback setzen
	mosquitto_message_callback_set(mosq, mqtt_grid_callback);
	// mit MQTT-Broker verbinden
    mosquitto_error_handling(mosquitto_connect_async(mosq, host, port, MQTT_KEEPALIVE));
	// Topics abbonieren

	mosquitto_error_handling(mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_CREATE_GAME_SCREEN, qos));
	mosquitto_error_handling(mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_DESTROY_GAME_SCREEN, qos));
	mosquitto_error_handling(mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_GRID, qos));
	mosquitto_error_handling(mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_NEXT_BRICK, qos));
	mosquitto_error_handling(mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_SCORE_TIME, qos));
	mosquitto_error_handling(mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_SCORE_LEVEL, qos));
	mosquitto_error_handling(mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_SCORE_BRICKS, qos));
	mosquitto_error_handling(mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_SCORE_LINES, qos));
	mosquitto_error_handling(mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_SCORE_POINTS, qos));
	mosquitto_error_handling(mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_GAME_STATUS, qos));

	// MQTT-Loop starten
	mosquitto_loop_start(mosq);
}

// ************************************************
void mqtt_get_score(uint8_t qos)
{
	mosquitto_error_handling(mosquitto_publish(mosq, NULL, MQTT_TOPIC_GET_SCORE, 2, "1", qos, false));
}

// ************************************************
void mqtt_clear(void)
{
	mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}
