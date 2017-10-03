#include <ESP8266WiFi.h>
#include <lwip/ip.h>
#include <lwip/dns.h>
#include <string.h>

extern "C" void esp_schedule();
extern "C" void esp_yield();


const char* ssid     = "extension";
const char* password = NULL;



void doa_found_callback(const char * name, firmwareinfo_t *fwinfo, void *arg);
firmwareinfo_t fwinfo;
void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  
  
  memset(&fwinfo, 0, sizeof(fwinfo));
  err_t err = dns_getfirmwareinfo("test1.example", doa_found_callback, &fwinfo);
  if (err == ERR_INPROGRESS) {
    esp_yield(); // wait signal from callback
  }

  Serial.print("DOA get err: ");
  Serial.println(err);
  Serial.println(fwinfo.firmware);
  
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  delay(5000);

  Serial.print("fwinfo.firmware: ");
  Serial.println(fwinfo.firmware);

 
}

void doa_found_callback(const char * name, firmwareinfo_t *fwinfo, void *arg){
  if (fwinfo) {
    *(firmwareinfo_t *) arg = *fwinfo;
  }
  esp_schedule(); // resume on matching esp_yield()
}

