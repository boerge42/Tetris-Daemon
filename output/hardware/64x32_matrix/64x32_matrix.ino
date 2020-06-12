/* ********************************************************************
 *
 * Anzeige Tetris-Grid auf einer 64x32-LED-Matrix
 * ==============================================
 *              Uwe Berger; 2020
 *
 * ...plus einiger Uhren in den Spielpausen...
 *
 * --> Controller: ESP8266 (WEMOS D1 mini)
 *
 * --> 64x32-LED-Matrix: https://github.com/2dom/PxMatrix
 * Die LED-Matrix wird hochkant verwendet.
 * 
 * Konfiguration fuer WLAN und MQTT-Broker liegt in der Datei
 * config.txt im SPIFFS des Controllers. 
 *
 * ---------
 * Have fun!
 * 
 *
 * ********************************************************************
 */


#define double_buffer
#include <PxMatrix.h>
#include <ezTime.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "FS.h"

// WLAN-Konfiguration (Default)
#define WLAN_SSID  "xyz"
#define WLAN_PWD  "42"
#define HOSTNAME  "my_pxmatrix"

// MQTT-Konfiguration (Default)
#define MQTT_BROKER "brocker"
#define MQTT_PORT   1883
#define MQTT_USER   ""
#define MQTT_PWD    ""
#define MQTT_CLIENT_ID "matrix64x32"

#define MQTT_TOPIC_GRID               "tetris/grid"
#define MQTT_TOPIC_GAME_STATUS        "tetris/gamestatus"
#define MQTT_TOPIC_GET_SCORE          "tetris/get_score"
#define MQTT_TOPIC_CREATE_GAME_SCREEN "tetris/create_game_screen"
#define MQTT_TOPIC_CLOCK_TYPE         "pxmatrix/clock_type"
#define MQTT_TOPIC_BRIGHTNESS         "pxmatrix/brightness"


// Zeitzone
#define MY_TIME_ZONE "Europe/Berlin"
Timezone myTZ;
int old_second = -1;
int old_minute = -1;
char my_time_zone[50] = MY_TIME_ZONE;


char ssid[30]     = WLAN_SSID;
char password[30] = WLAN_PWD;
char hostname[30] = HOSTNAME;

char mqttServer[30] 	= MQTT_BROKER;
int  mqttPort 		    = MQTT_PORT;
char mqttUser[30] 	    = MQTT_USER;
char mqttPassword[30]   = MQTT_PWD;
char mqttClientId[30]   = MQTT_CLIENT_ID;

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

bool with_mqtt = false;

#include <Ticker.h>
Ticker display_ticker;
Ticker display_mode_ticker;

#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

// Pins for LED MATRIX
//PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D); 
//PxMATRIX display(64,64,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);

/*
https://roboindia.com/tutorials/2-4″-touchscreen-lcd-shield-arduino-tft/    
// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
int32_t BLUE = tft.color565(50, 50, 255);
#define DARKBLUE 0x0010
#define VIOLET 0x8888
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY   tft.color565(64, 64, 64);
#define GOLD 0xFEA0
#define BROWN 0xA145
#define SILVER 0xC618
#define LIME 0x07E0
#define ORANGE 0xFD20
#define ORANGERED 0xFA20
*/

uint16_t tetris_GREY    = display.color565(64, 64, 64);
uint16_t tetris_WHITE   = 0xFFFF;
uint16_t tetris_BLACK   = 0x0000;
uint16_t tetris_GREEN   = 0x07E0;
uint16_t tetris_BLUE    = 0x0010;
uint16_t tetris_YELLOW  = 0xFFE0;
uint16_t tetris_RED     = 0xF800;
uint16_t tetris_BROWN   = 0xA145;
uint16_t tetris_MAGENTA = 0xF81F;
uint16_t tetris_CYAN    = 0x07FF;

const uint16_t tetris_colors[] =
{
	tetris_BLACK,    // black
    tetris_GREEN,    // green
    tetris_BLUE,     // blue
    tetris_YELLOW,   // yellow
    tetris_RED,      // red
    tetris_BROWN,    // brown
    tetris_MAGENTA,  // magenta
    tetris_CYAN      // lightblue/cyan
};

const uint8_t brightness_values[10] = {0, 25, 50, 75, 100, 125, 150, 175, 200, 255};

extern uint8_t clock_type;

#define DISPLAY_MODE_CLOCK  1
#define DISPLAY_MODE_TETRIS 2

uint8_t display_mode = DISPLAY_MODE_CLOCK;

// **************************************************************
// ISR for display refresh
void display_updater()
{
  display.display(70);
}

// **************************************************************
// ISR for display_mode
void set_display_mode_clock()
{
  display_mode_ticker.detach();
  display_mode = DISPLAY_MODE_CLOCK;
}

// **************************************************************
void wifi_connect()
{
  // ins WIFI anmelden 
  WiFi.begin(ssid, password);
  WiFi.hostname(hostname);
  Serial.print("Connect to WiFi: ") ;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

// **************************************************************
void tetris_color_test(void)
{
  uint8_t i;
  tetris_drawborder();
  for (i=1; i<(sizeof tetris_colors / sizeof tetris_colors[0]); i++) {
    tetris_drawrect(0, i-1, tetris_colors[i]);
    tetris_drawrect(9, i-1, tetris_colors[i]);
    tetris_drawrect(0, 13+i-1, tetris_colors[i]);
    tetris_drawrect(9, 13+i-1, tetris_colors[i]);
  }
}

// **************************************************************
void tetris_drawborder(void)
{
  display.drawFastHLine(3, 0, 60, tetris_GREY);
  display.drawFastHLine(3, 31, 60, tetris_GREY);
  display.drawFastVLine(63, 0, 32, tetris_GREY);
}

// **************************************************************
void tetris_drawrect(uint8_t x, uint8_t y, uint16_t color)
{
  display.fillRect(3+y*3, 28-x*3, 3, 3, color);
}

// **************************************************************
void tetris_init_game_screen(void)
{
  display.fillScreen(tetris_BLACK);
  tetris_drawborder();
  display.showBuffer();
}

// **************************************************************
void tetris_drawgrid(char * grid, unsigned int len)
{
  uint16_t i;
  div_t divresult;
  uint8_t color;
  //unsigned long start_timer = micros();
  
  display.fillScreen(tetris_BLACK);
  tetris_drawborder();
  for (i=0; i<len; i++) {
	if (grid[i] != '0') {
		divresult=div(i, 20);
		color = grid[i]-0x30;
		if (color > 9) color=0;
		tetris_drawrect(divresult.quot, divresult.rem, tetris_colors[color]);  
	}
  }
  display.showBuffer();
  //Serial.println(micros()-start_timer);
}

// **************************************************************
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
  char tetris_gamestatus, brightness;
 
  if (strcmp(topic, MQTT_TOPIC_GRID) == 0) {
	tetris_drawgrid((char*)payload, length);
  } else
  if (strcmp(topic, MQTT_TOPIC_CLOCK_TYPE) == 0) {
	clock_type = payload[0]-0x30;	
	Serial.printf("--> clock_type: %i\n", clock_type);
  } else
  if (strcmp(topic, MQTT_TOPIC_BRIGHTNESS) == 0) {
	  brightness = payload[0]-0x30;	
      if (brightness > 9) brightness = 9;
      display.setBrightness(brightness_values[brightness]);
      Serial.printf("--> brightness: %i\n", brightness);
  } else  if (strcmp(topic, MQTT_TOPIC_GAME_STATUS) == 0) {
	tetris_gamestatus = payload[0]-0x30;	
	if (display_mode_ticker.active()) display_mode_ticker.detach(); 
	switch (tetris_gamestatus ) {
	  case 0:
	    display_mode = DISPLAY_MODE_TETRIS;
	    break;
	  case 1:
	  case 2:
	    display_mode_ticker.attach(10.0, set_display_mode_clock);
	    break;	
	}
  } else
  if (strcmp(topic, MQTT_TOPIC_CREATE_GAME_SCREEN) == 0) {
	if (display_mode_ticker.active()) display_mode_ticker.detach(); 
	display_mode = DISPLAY_MODE_TETRIS;
	tetris_init_game_screen();
  }  
}

// **************************************************************
void mqtt_reconnect ()
{
  
  // MQTT...
  // ...initialisieren
  mqtt_client.setServer(mqttServer, mqttPort);
  // ...Callback setzen
  mqtt_client.setCallback(mqtt_callback);
  // ...verbinden
  while (!mqtt_client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqtt_client.connect(mqttClientId, mqttUser, mqttPassword )) {
       Serial.println("Connected!");  
       with_mqtt = true;
    } else {
      Serial.print("failed with state ");
      Serial.println(mqtt_client.state());
      // wenn der Broker mal nicht erreichbar ist, dann soll dieses
      // Programm, bis zum naechsten Restart/Reset, auch ohne MQTT-Zeugs 
      // funktionieren (PubSubClient-Lib arbeitet nicht asynchron, ein
      // Connect-Versuch blockiert den Rest ggf. eine Ewigkeit, also
      // kontraproduktiv fuer eine fortlaufende Uhr...)
      with_mqtt = false;
      break;
    }
  }
  // ...Topics abonnieren
  if (mqtt_client.connected()) {
    mqtt_client.subscribe(MQTT_TOPIC_GAME_STATUS);
    mqtt_client.subscribe(MQTT_TOPIC_GRID);
    mqtt_client.subscribe(MQTT_TOPIC_CLOCK_TYPE);
    mqtt_client.subscribe(MQTT_TOPIC_CREATE_GAME_SCREEN);
    mqtt_client.subscribe(MQTT_TOPIC_BRIGHTNESS);
  }
}


// **************************************************************
String config_getvalue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// **************************************************************
void config_string2chararray(String val, char * c, uint8_t len)
{
  val.toCharArray(c, len-1);
  // zur Sicherheit nullterminieren
  c[len-1] = '\0';
}

// **************************************************************
void config_read() 
{
  String key, val;
  SPIFFS.begin(); 
  File f = SPIFFS.open( "/config.txt", "r");
  if (!f) {
    Serial.println("config file open failed");
  }
  while(f.available()) {
      String line = f.readStringUntil('\n');
      key = config_getvalue(line, '=', 0);
      val = config_getvalue(line, '=', 1);
      if (key.equalsIgnoreCase("wlan_ssid")) {
		config_string2chararray(val, ssid, sizeof ssid);
	  } else 
	  if (key.equalsIgnoreCase("wlan_pwd")) {
		config_string2chararray(val, password, sizeof password);
	  } else 
	  if (key.equalsIgnoreCase("hostname")) {
		config_string2chararray(val, hostname, sizeof hostname);
	  } else 
	  if (key.equalsIgnoreCase("mqtt_brocker")) {
		config_string2chararray(val, mqttServer, sizeof mqttServer);
	  } else 
	  if (key.equalsIgnoreCase("mqtt_port")) {
		mqttPort=val.toInt();  
	  } else 
	  if (key.equalsIgnoreCase("mqtt_user")) {
		config_string2chararray(val, mqttUser, sizeof mqttUser);
	  } else 
	  if (key.equalsIgnoreCase("mqtt_pwd")) {
		config_string2chararray(val, mqttPassword, sizeof mqttPassword);
	  } else 
	  if (key.equalsIgnoreCase("mqtt_client_id")) {
		config_string2chararray(val, mqttClientId, sizeof mqttClientId);
	  }	else 
	  if (key.equalsIgnoreCase("time_zone")) {
		config_string2chararray(val, my_time_zone, sizeof my_time_zone);
	  }		  
      Serial.print(key);
      Serial.print(" --> ");
      Serial.println(config_getvalue(line, '=', 1));
  }
  f.close();
}

// **************************************************************
void setup() {
	
  // serielle Schnittstelle initialisieren
  Serial.begin(115200);
  Serial.println("");
  Serial.println("...setup");

  // Konfiguration aus Datei im SPIFFS auslesen
  config_read();

  // WiFi initialisieren
  wifi_connect();

  // Datum/Uhrzeit synchronisieren
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  myTZ.setLocation(my_time_zone);
  Serial.println("MyTZ time: " + myTZ.dateTime("d-M-y H:i:s"));

  // Display initialisieren
  display.begin(16);
  display.setFastUpdate(true);
  display.flushDisplay();
  display.setTextWrap(false);
  display_ticker.attach(0.002, display_updater);
  draw_clock();
  
  // MQTT initialisieren
  mqtt_reconnect();
  
  // Tetris-Spielstand anfordern
  mqtt_client.publish(MQTT_TOPIC_GET_SCORE, "1", 1);
  
}

// **************************************************************
// **************************************************************
// **************************************************************
void loop() {
	
  // Uhrzeit anzeigen?
  if (display_mode == DISPLAY_MODE_CLOCK) {
    if (old_second != second()) {
      old_second = second();
      draw_clock();
    }
    // ezTime-Loop
    events();
  }
  
  // MQTT-Loop (...wenn der Broker beim letzten connect-Versuch 
  // erreichbar war)
  if (with_mqtt == true) {
    if (!mqtt_client.connected()) {
      mqtt_reconnect();
    } else {
      mqtt_client.loop();
    }
  }
  
}
