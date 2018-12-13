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

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#include "my_mqtt.h"
#include "max7219.h"

#define DISPLAY_COUNT 3
#define SPI_CHANNEL   0


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

// ********************************************************************
void mqtt_scoreboard_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    char **topics;
    int topic_count;
    char buf[DISPLAY_COUNT*8+1];
    uint8_t i = 0;
	
    mosquitto_error_handling(mosquitto_sub_topic_tokenise(message->topic, &topics, &topic_count));
    
    if (strcmp(topics[0], "tetris") == 0) {
    
	    if (strcmp(topics[topic_count-1], "level") == 0) {
			if (atoi(message->payload)<10 && atoi(message->payload) > -1) {
				max7219_display_value(1, 7, atoi(message->payload), -1);
			}
		}
		if (strcmp(topics[topic_count-1], "bricks") == 0) {
			if (atoi(message->payload)<1000 && atoi(message->payload) > -1) {
				max7219_display_value(2, 4, atoi(message->payload), -1);
			}
		}
   	 	if (strcmp(topics[topic_count-1], "lines") == 0) {
			if (atoi(message->payload)<1000 && atoi(message->payload) > -1) {
				max7219_display_value(2, 0, atoi(message->payload), -1);
			}
		}
    	if (strcmp(topics[topic_count-1], "points") == 0) {
			if (atoi(message->payload)<1000000 && atoi(message->payload) > -1) {
				max7219_display_value(0, 0, atol(message->payload), -1);
			}
		}
    	if (strcmp(topics[topic_count-1], "time") == 0) {
			if (atof(message->payload)<10000.00 && atoi(message->payload) >= 0.0) {
				max7219_display_value(1, 0, atof(message->payload)*100, 2);
			}
		}
    	if (strcmp(topics[topic_count-1], "gamestatus") == 0) {
	    	switch (atoi(message->payload)) {
				case 1:
					// Pause --> 'P'
					max7219_send_data(0, REG_ADDR_DIGIT7, 0xE);
					break;
				case 2:
					// Spielende --> 'E'
					max7219_send_data(0, REG_ADDR_DIGIT7, 0xB);
					break;	
				default:
					max7219_send_data(0, REG_ADDR_DIGIT7, CHAR_BLANK);
					break;						
			}
		}

    	if (strcmp(topics[topic_count-1], "create_game_screen") == 0) {
			max7219_clear_all();
		}
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
	uint8_t daemonize   = 0;

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
			case 'd':
				daemonize = 1;
				break;
			case '?':
				puts("score [-h <mqtt-host>] [-p <mqtt-port>]");
				puts("      [-U <mqtt-user>] [-P <mqtt-pwd>]");
				puts("      [-q <mqtt-qosr>] [-i <mqtt-id>]");
				puts("      [-d -?]");
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

	// aktuellen Spielstand etc. anfordern
	mqtt_get_score(mqtt_qos);
	
	// Loop...
	//while (getchar() == 'Q') {
	//}
	mqtt_loop_forever();

	return 0;
}
