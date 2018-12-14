/* ***********************************
 *
 * Tetrisd: highscoredb
 * ====================
 *   Uwe Berger; 2018
 * 
 * 
 * ...End-Spielstand in einer sqlite3-Datenbank abspeichern
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

#include <sqlite3.h>

#include "my_mqtt.h"


char mqtt_host[50]	= MQTT_HOST;
int  mqtt_port    	= MQTT_PORT;
char mqtt_user[50]	= "";
char mqtt_pwd[50]	= "";
uint8_t mqtt_qos    = MQTT_QOS;
char mqtt_id[50]    = MQTT_CLIENT_ID;

char db_filename[256]    = "./highscore";

uint8_t daemonize   = 0;


// ********************************************
void signal_handler(int sig)
{
	switch(sig) {
		case SIGHUP:
			syslog(LOG_INFO, "...receive  SIGHUP, what's up?");
			break;
		case SIGTERM:
			syslog(LOG_INFO, "Stopped with SIGTERM!");
			// MQTT-LWT auf off
			mqtt_set_lwt_topic_off(mqtt_qos);
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
void houston_we_have_a_problem(char *msg)
{
	if (daemonize) {
		syslog(LOG_INFO, msg);
	} else {
		printf("higscoredb: %s\n", msg);
	}
	exit(EXIT_FAILURE);
}

// ********************************************************************
void mqtt_highscoredb_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	sqlite3 *db;
	char *err = NULL;
	char sql[1000];
	char buf[200];

	// highscoredb oeffnen
	if (sqlite3_open(db_filename, &db)) {
		sprintf(buf, "%s", sqlite3_errmsg(db));
		houston_we_have_a_problem(buf);
	}

	// Tabelle 'tetris' anlegen, wenn noch nicht da
	sprintf(sql, "%s", "create table if not exists tetris (highscore json)");
	if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
		sprintf(buf, "%s", err);
		houston_we_have_a_problem(buf);
	}
	
	// Datensatz (Inhalt von Payload) in Tabelle 'tetris' einfuegen
	sprintf(sql, "insert into tetris values (\'%s\')", (char *)message->payload);
	if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
		sprintf(buf, "%s", err);
		houston_we_have_a_problem(buf);
	}

	// highscoredb schliessen
	if (sqlite3_close(db) != SQLITE_OK) {
		sprintf(buf, "%s", err);
		houston_we_have_a_problem(buf);
	}
}



// ********************************************************************
// ********************************************************************
// ********************************************************************
int main(int argc, char **argv)
{
	int c;

	// Aufrufparameter auslesen/verarbeiten
	while ((c=getopt(argc, argv, "h:p:u:P:q:i:f:d?")) != -1) {
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

			case 'f':
				if (strlen(optarg) >= sizeof db_filename) {
					puts("db_filename too long!");
					exit(EXIT_FAILURE);
				} else {
					strncpy(db_filename, optarg, sizeof(db_filename));
				}
				break;

			case 'd':
				daemonize = 1;
				break;
			default:
				puts("score [-h <mqtt-host>] [-p <mqtt-port>]");
				puts("      [-U <mqtt-user>] [-P <mqtt-pwd>]");
				puts("      [-q <mqtt-qosr>] [-i <mqtt-id>]");
				puts("      [-f <highscoredb-file>]");
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

	// MQTT initialisieren
	mqtt_init(mqtt_host, mqtt_port, mqtt_user, mqtt_pwd, mqtt_qos, mqtt_id);
	
	// loop forever
	mqtt_loop_forever();

	return 0;
}
