/* ***********************************
 *
 * Scoreboard mit MAX7219
 * ======================
 *    Uwe Berger; 2018
 * 
 * 
 * ---------
 * Have fun!
 * 
 *
 * ********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mosquitto.h>
#include <wiringPi.h>


#include "my_mqtt.h"
#include "max7219.h"

#define DISPLAY_COUNT 3
#define SPI_CHANNEL   0


// ********************************************************************
void mqtt_scoreboard_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    char **topics;
    int topic_count;
    char buf[DISPLAY_COUNT*8+1];
    uint8_t i = 0;
	
	static uint16_t level = 0;
	static uint16_t bricks = 0;
	static uint16_t lines = 0;
	static uint32_t points = 0;
	static uint32_t time = 0;
	static uint16_t  gamestatus = 0;
	
	//printf("%s --> %s\n", message->topic, (char *)message->payload);

    mosquitto_error_handling(mosquitto_sub_topic_tokenise(message->topic, &topics, &topic_count));
    
    if (strcmp(topics[0], "tetris") == 0) {
    
	    if (strcmp(topics[topic_count-1], "level") == 0) {
			if (atoi(message->payload)<10 && atoi(message->payload) > -1) 
				level = atoi(message->payload);
		}
		if (strcmp(topics[topic_count-1], "bricks") == 0) {
			if (atoi(message->payload)<1000 && atoi(message->payload) > -1) 
				bricks = atoi(message->payload);
		}
   	 	if (strcmp(topics[topic_count-1], "lines") == 0) {
			if (atoi(message->payload)<1000 && atoi(message->payload) > -1) 
				lines = atoi(message->payload);
		}
    	if (strcmp(topics[topic_count-1], "points") == 0) {
			if (atoi(message->payload)<1000000 && atoi(message->payload) > -1) 
				points = atol(message->payload);
		}
    	if (strcmp(topics[topic_count-1], "time") == 0) {
			if (atof(message->payload)<10000.00 && atoi(message->payload) >= 0.0) 
				time = atof(message->payload)*100;
		}
    	if (strcmp(topics[topic_count-1], "gamestatus") == 0) {
			gamestatus = atoi(message->payload);
		}
    
    	// ...und ausgeben
    	max7219_clear_all();
    	switch (gamestatus) {
			case 1:
				// Pause --> 'P'
				max7219_send_data(0, REG_ADDR_DIGIT7, 0xE);
				break;
			case 2:
				// Spielende --> 'E'
				max7219_send_data(0, REG_ADDR_DIGIT7, 0xB);
				break;		
		}
   		max7219_display_value(0, 0, points, -1);
    	max7219_display_value(1, 7, level, -1);
    	max7219_display_value(1, 0, time, 2);
    	max7219_display_value(2, 4, bricks, -1);
    	max7219_display_value(2, 0, lines, -1);
    
	}
	
	
	if (strcmp(topics[0], "scoreboard") == 0) {
		memset(buf, 0, DISPLAY_COUNT*8+1);
        memcpy(buf, message->payload, DISPLAY_COUNT*8+1);
		//printf("%s --> %i\n", message->payload, message->payloadlen);
   		max7219_clear_all();
   		while (i<message->payloadlen && i<DISPLAY_COUNT*8 ) {
			if (buf[i]>=0x30 && buf[i]<=0x39) {
				max7219_send_data(i/8, 8-(i%8), buf[i]-0x30);
			} else {
				if (buf[i]==0x2D) {
					max7219_send_data(i/8, 8-(i%8), CHAR_NEGATIVE);
				} else {
					max7219_send_data(i/8, 8-(i%8), CHAR_BLANK);					
				}
			}
			i++;
		}
	}


    mosquitto_error_handling(mosquitto_sub_topic_tokens_free(&topics, topic_count));



}



// ********************************************************************
// ********************************************************************
// ********************************************************************
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


	// WiringPi init (allein fuer SPI nicht noetig, aber wer weiss, was noch kommt ;-)
	wiringPiSetup();
	
	// SPI init
	max7219_init(SPI_CHANNEL, 10000000, DISPLAY_COUNT);

	// Helligkeit, Zeichencode, Anschalten, Scanbreite, alle loeschen
	max7219_set_intensity_all(5);
	max7219_set_decode_all(CODE_B_7_0);
	max7219_set_shutdown_all(NORMAL_MODE);
	max7219_send_data_all(REG_ADDR_SCAN_LIMIT, DISPLAY_0_TO_7);
	max7219_clear_all();

	// MQTT initialisieren
	mqtt_init(mqtt_host, mqtt_port, mqtt_user, mqtt_pwd, mqtt_qos, mqtt_id);
	
	// Loop (...bis Taste q betaetigt...)
	while (getchar() == 'q') {
	}

	return 0;
}
