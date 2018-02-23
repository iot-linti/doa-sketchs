
/*
  WiFi UDP Send and Receive String

 This sketch wait an UDP packet on localPort using a WiFi shield.
 When a packet is received an Acknowledge packet is sent to the client on port remotePort

 Circuit:
 * WiFi shield attached

 created 30 December 2012
 by dlf (Metodo2 srl)

 */


#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "secrets.h"

int status = WL_IDLE_STATUS;
const char* ssid     = AP_SSID;
const char* password = AP_PASS;

unsigned int localPort = 2390;      // local port to listen on

char packetBuffer[513]; //buffer to hold incoming packet

WiFiUDP Udp;

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Udp.begin(localPort);
}

void loop() {
  delay(500);  

  Udp.beginPacket("163.10.20.210", 9002);
  Udp.write("/");
  Udp.endPacket();

  int packetSize;
  unsigned long timeout = millis();
  while ((packetSize = Udp.parsePacket()) == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      return;
    }
  }
  if (packetSize) {
    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 512);
    if (len >= 0) {
      packetBuffer[len] = 0;
    }

    Serial.println("Contents:");
    Serial.println(packetBuffer);
  }
}
