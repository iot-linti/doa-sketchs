
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

char packetBuffer[256]; //buffer to hold incoming packet

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
  static bool state_send = true;
  
  if (state_send) {
    Udp.beginPacket("163.10.20.210", 9002);
    Udp.write("/");
    Udp.endPacket();
    state_send = false;
    delay(500);
  }
  else {
    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    while (packetSize) {
      // read the packet into packetBufffer
      int len = Udp.read(packetBuffer, 255);
      if (len > 0) {
        packetBuffer[len] = 0;
      }
      Serial.println("Contents:");
      Serial.println(packetBuffer);
      state_send = true;
      
      packetSize = Udp.parsePacket();
    }
  }
}