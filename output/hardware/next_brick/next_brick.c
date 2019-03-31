/* ***********************************
 *
 * Anzeige next-brick mit 8x8-WS2812-Matrix
 * ========================================
 *             Uwe Berger; 2018
 * 
 * 
 * ---------
 * Have fun!
 * 
 *
 * ********************************************************************
 */


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ws2811.h"
#include "my_mqtt.h"


// Hardware-Defaults
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
#define STRIP_TYPE              WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
#define WS2811_WIDTH            8
#define WS2811_HEIGHT           8
#define LED_COUNT		        (WS2811_WIDTH * WS2811_HEIGHT)
#define LED_BRIGHTNESS			100

// MQTT-Defaults
char 	mqtt_host[50]	= MQTT_HOST;
int 	mqtt_port    	= MQTT_PORT;
char 	mqtt_user[50]	= "";
char 	mqtt_pwd[50]	= "";
uint8_t mqtt_qos    	= MQTT_QOS;
char 	mqtt_id[50]     = MQTT_CLIENT_ID;

#define MAX_LEN_MQTT_TOPIC		16

uint8_t daemonize   = 0;

ws2811_t ledstring =
{
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel =
    {
        [0] =
        {
            .gpionum = GPIO_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = LED_BRIGHTNESS,
            .strip_type = STRIP_TYPE,
        },
        [1] =
        {
            .gpionum = 0,
            .count = 0,
            .invert = 0,
            .brightness = 0,
        },
    },
};

ws2811_led_t dotcolors[] =
{
	0x00000000,  // black
    0x00200000,  // green
    0x00000020,  // blue
    0x00202000,  // yellow
    0x00002000,  // red
    0x00061005,  // brown
    0x00002020,  // magenta
    0x00100010   // lightblue/cyan
};


// ********************************************
void led_matrix_clear(void)
{
    int x, y;

    for (x = 0; x < WS2811_WIDTH; x++)
    {
        for (y = 0; y < WS2811_HEIGHT; y++)
        {
            ledstring.channel[0].leds[(y * WS2811_WIDTH) + x] = 0;
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
		case SIGINT:
		case SIGTERM:
			syslog(LOG_INFO, "Stopped with SIGTERM or SIGINT!");
			mqtt_set_lwt_topic_off(mqtt_qos);
			// Aufraeumen...
			mqtt_clear();
			led_matrix_clear();
			ws2811_render(&ledstring);
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
void mqtt_next_brick_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	char i_buf[MAX_LEN_MQTT_TOPIC+1];
	char o_buf[WS2811_WIDTH*WS2811_HEIGHT+1];
	int8_t i, j;
	
	// Payload-Laenge ok?
	if (message->payloadlen != MAX_LEN_MQTT_TOPIC) return;
	// "Zoom" payload
	memset(i_buf, 0, sizeof(i_buf));
	memcpy(i_buf, message->payload, message->payloadlen);	
	memset(o_buf, 0, sizeof(o_buf));
	for (i=0; i<message->payloadlen; i++) {
	    	o_buf[i*2]     = i_buf[i];
	    	o_buf[i*2+1]   = i_buf[i];
	}
	for (j=56; j>0; j=j-16) {
		for (i=0; i<8; i++) {
			o_buf[j+i]   = o_buf[(j/16)*8+i];
			o_buf[j-8+i] = o_buf[(j/16)*8+i];
		}
	}
	// Ausgabe auf WS281x-Matrix
	for (i=0; i<LED_COUNT; i++) {
		ledstring.channel[0].leds[i] = dotcolors[o_buf[i]-0x30];
	}
	ws2811_render(&ledstring);
}

// ********************************************
// ********************************************
// ********************************************
int main(int argc, char *argv[])
{
    ws2811_return_t ret;
	int c;

	// Aufrufparameter auslesen/verarbeiten
	while ((c=getopt(argc, argv, "h:p:u:P:q:i:b:d?")) != -1) {
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
			case 'b':
				if (atoi(optarg)>=0 && atoi(optarg)<=255) {
					ledstring.channel[0].brightness = atoi(optarg);
				} else {
					puts("brightness must be between 0...255!");
					exit(EXIT_FAILURE);
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
				puts("        [-b <brightness>] --> LED-brightness (default: 100)");
				puts("        [-d]              --> ...as daemon   (default: no)");
				puts("        [-?]              --> print this...");
				exit(0);
				break;
		}
	}

	// behandelte Signale initialisieren
	signal(SIGTERM, signal_handler);
	signal(SIGINT,  signal_handler);
	signal(SIGHUP,  signal_handler);
	if (daemonize) {
		// "daemonisieren"
		start_daemon();
		// syslog
		openlog(argv[0], LOG_PID|LOG_CONS, LOG_DAEMON);
		syslog(LOG_INFO, "Started...!");
	}

	// Hardware etc. initialisieren
	if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS) {
		fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
		return ret;
	}

	// MQTT initialisieren
	mqtt_init(mqtt_host, mqtt_port, mqtt_user, mqtt_pwd, mqtt_qos, mqtt_id);

	// aktuellen Spielstand etc. anfordern
	mqtt_get_score(mqtt_qos);
	
	// Loop...
	mqtt_loop_forever();

    ws2811_fini(&ledstring);

    return ret;
}
