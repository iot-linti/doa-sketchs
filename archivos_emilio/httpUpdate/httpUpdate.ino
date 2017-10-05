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
#include <lwip/ip.h>
#include <lwip/dns.h>
#include <string.h>

extern "C" void esp_schedule();
extern "C" void esp_yield();

#define USE_SERIAL Serial


#include "wifi_secrets.h" // WIFI_SSID y WIFI_PASSWORD

firmwareinfo_t fwinfo;  // Set to zero because of the static storage

void doa_found_callback(const char * name, firmwareinfo_t *fwinfo, void *arg);
int newer_fwversion(const char *);
void upgrade_and_reset(const char *);

void setup() {
  USE_SERIAL.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  pinMode(2, OUTPUT);

  for(uint8_t t = 15; WiFi.status() != WL_CONNECTED && t > 0; t--) {
    USE_SERIAL.printf("Connecting to WiFi network %s... WAIT %d...\n", WIFI_SSID, t);
    USE_SERIAL.flush();
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED) {
      USE_SERIAL.println("Connection to WiFi network failed, restarting");
      ESP.restart();
  }

  // DOA query
  err_t err = dns_getfirmwareinfo("test1.example", doa_found_callback, &fwinfo);
  if (err == ERR_INPROGRESS) {
    esp_yield(); // wait signal from callback
  }

  if (strlen(fwinfo.firmware) > 0) {
    USE_SERIAL.print("firmware: ");
    USE_SERIAL.println(fwinfo.firmware);
    USE_SERIAL.print("firmware-sig: ");
    USE_SERIAL.println(fwinfo.firmware_sig);
    USE_SERIAL.print("firmware-version: ");
    USE_SERIAL.println(fwinfo.firmware_version);

    if (newer_fwversion(fwinfo.firmware_version)) {
        upgrade_and_reset(fwinfo.firmware);
    }
  }
  else {
    USE_SERIAL.println("No firmware info was retrieved");
  }
}

void loop() {
  digitalWrite(2, LOW);
  delay(1000);
  digitalWrite(2, HIGH);
  delay(1000);
}

void upgrade_and_reset(const char *url) {
  USE_SERIAL.println("Procceding to upgrade");
  if((WiFi.status() == WL_CONNECTED)) {
    USE_SERIAL.print("Downloading new firmware: ");
    USE_SERIAL.println(url);
    t_httpUpdate_return ret = ESPhttpUpdate.update(url);

    switch(ret) {
        case HTTP_UPDATE_FAILED:
            USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
            USE_SERIAL.println("HTTP_UPDATE_OK");
            USE_SERIAL.println("New firmware downloaded, wait for auto reset or restart manually");
            break;
    }
  } else {
    USE_SERIAL.printf("WiFi disconnected, reseting...");
  }
  delay(1000);
  ESP.restart();
}

int newer_fwversion(const char *name) {
  return 1;
}

void doa_found_callback(const char * name, firmwareinfo_t *fwinfo, void *arg){
  if (fwinfo) {
    memcpy(arg, fwinfo, sizeof(firmwareinfo_t));
  }
  else {
    memset(arg, 0, sizeof(firmwareinfo_t));
  }
  esp_schedule(); // resume on matching esp_yield()
}
