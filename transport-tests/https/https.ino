/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "secrets.h"

const char* ssid     = AP_SSID;
const char* password = AP_PASS;

const char* host = HOST;

const char* fingerprint = "7A:B8:98:BE:30:8D:25:6A:FB:7F:15:FA:DF:44:D8:71:D0:D0:69:13";

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
}

int value = 0;
bool verified;

void loop() {
  delay(500);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  const int httpPort = 9003;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  verified = client.verify(fingerprint, "example.com");
  if (!verified) {
    Serial.println("Verificacion fallida");
    return;
  }
  
  // We now create a URI for the request
  String url = "/";
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}

