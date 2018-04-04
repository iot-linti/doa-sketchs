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


/*
 * https://nofurtherquestions.wordpress.com/2016/03/14/making-an-esp8266-web-accessible/
 */

const char* ssid     = AP_SSID;
const char* password = AP_PASS;

const char* host = "www.google.com";

#define DONT_VERIFY  0x0
#define FINGER_PRINT 0x1
#define FINGER_PRINT256 0x2
#define EMBED_CERT 0x4


#define MODE FINGER_PRINT | EMBED_CERT

#if MODE & FINGER_PRINT
const char fingerprint[] = "F9:AF:0A:57:02:47:A7:FF:2A:F7:7B:9B:F5:42:5B:60:C0:2E:FC:47";
#elif MODE &  FINGER_PRINT256
const char fingerprint[] = "C3:81:10:3E:30:64:37:B5:98:22:E7:1C:D3:8B:DC:18:D7:59:F0:E7:9C:89:AD:B8:20:8D:17:B2:27:D7:63:64";
#endif

#if MODE & EMBED_CERT
#include "certificates.h"
#endif


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
int verified = 0;

void loop() {
  delay(500);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClientSecure class to create TCP connections
  WiFiClientSecure client;
  const int httpPort = 443;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  verified = client.verify(fingerprint, "www.google.com");
  Serial.print("Fingerprint verified: ");
  Serial.println(verified);

  client.setCertificate(certificates_esp8266_bin_crt, certificates_esp8266_bin_crt_len);
  client.setCertificate(certificates_esp8266_bin_crt, certificates_esp8266_bin_crt_len);

  verified = client.verifyCertChain("www.google.com");
  Serial.print("Cert chain verified: ");
  Serial.println(verified);

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

