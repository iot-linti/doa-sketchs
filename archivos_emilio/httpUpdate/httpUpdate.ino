/**
 * httpUpdate.ino
 *
 *  Created on: 27.11.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#define USE_SERIAL Serial

/* DEBUG httpUpdate */
//#define DEBUG_ESP_PORT Serial
//#define DEBUG ESP_HTTP_UPDATE

#include "wifi_secrets.h" // WIFI_SSID y WIFI_PASSWORD
#define SERVER_IP "163.10.20.226"
#define SERVER_PORT 8000
#define ARCHIVO "/BlinkESP.ino.bin"

void setup() {
  USE_SERIAL.begin(115200);
  USE_SERIAL.println();
  
  for(uint8_t t = 5; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  USE_SERIAL.println("antes del WiFi.begin");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  USE_SERIAL.println("despues del WiFi.begin");
  
  for(uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("Conectando al wifi %s... WAIT %d...\n", WIFI_SSID, t);
    USE_SERIAL.flush();
    delay(1000);
  }
}

void loop() {
  // wait for WiFi connection
  if((WiFi.status() == WL_CONNECTED)) {
    USE_SERIAL.print("CONECTADO al wifi ");
    USE_SERIAL.println(WIFI_SSID);
    USE_SERIAL.println("bajando .bin del servidor...");
    t_httpUpdate_return ret = ESPhttpUpdate.update(SERVER_IP, SERVER_PORT, ARCHIVO);
    //t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

    switch(ret) {
        case HTTP_UPDATE_FAILED:
            USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
            USE_SERIAL.println("HTTP_UPDATE_OK");
            USE_SERIAL.println("archivo .bin bajado del servidor. esperar al reset o resetear manualmente");
            break;
    }
  } else {
    USE_SERIAL.printf("Intentando conectar a %s:%d, status wifi: ", SERVER_IP, SERVER_PORT);
    USE_SERIAL.println(WiFi.status());
    delay(1000);
  }
}

