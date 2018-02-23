#include <ESP8266WiFi.h>
#include <lwip/def.h>
#include "secrets.h" // ssid and password
#include <assert.h>

typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long    u32_t;
typedef signed     long    s32_t;
typedef unsigned long   mem_ptr_t;

#define MAX_DNS_PAYLOAD 1024

uint8_t payloadBuff[MAX_DNS_PAYLOAD];
char dnsServer[] = "194.150.168.168";

/** DNS message header */
struct dns_hdr {
  u16_t id;
  u8_t flags1;
  u8_t flags2;
  u16_t numquestions;
  u16_t numanswers;
  u16_t numauthrr;
  u16_t numextrarr;
} __attribute__((packed));

/** DNS query message structure.
    No packing needed: only used locally on the stack. */
struct dns_query {
  /* DNS query record starts with either a domain name or a pointer
     to a name already present somewhere in the packet. */
  u16_t type;
  u16_t cls;
} __attribute__((packed));

struct dns_opt {
  u8_t name;
  u16_t type;
  u16_t udp_payload_size;
  u8_t higher_bits;
  u8_t ends0_ver;
  // separar
  u16_t z;
  u16_t length;
} __attribute__((packed));

#define PRINTST(struct_, field) { Serial.print(#struct_ "->" #field ": "); Serial.println((struct_)->field); }
#define PRINTSTBS(struct_, field) { Serial.print(#struct_ "->" #field ": "); Serial.println(ntohs((struct_)->field)); }
#define PRINTSTHEX(struct_, field) { Serial.print(#struct_ "->" #field ": "); Serial.println((struct_)->field, 16); }

WiFiClient dnsServerConn;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Not connected yet");
    delay(1000);
  }
  
  Serial.print("Ready!");
  Serial.println(WiFi.localIP());


  static struct dns_hdr *header;
  static struct dns_query *query;
  static struct dns_opt *opt;

  header = (struct dns_hdr *) payloadBuff;
  query = (struct dns_query *) (payloadBuff + sizeof(struct dns_hdr));
  opt = (struct dns_opt *) ((char *) query + sizeof(struct dns_query));

  static unsigned id = 0;
  static char name[] = "com";
  if (!dnsServerConn.connect(dnsServer, 53)){
    Serial.println("Couldn't connect to server");
    delay(500);
    return;
  }
  header->id = htons(id++);
  header->flags1 = 0x01;
  header->flags2 = 0x20;
  header->numquestions = htons(1);
  header->numanswers = htons(0);
  header->numauthrr = htons(0);
  header->numextrarr = htons(1);

  opt->name = 0x00; // ROOT
  opt->type = htons(0x0029); // OPT (41)
  opt->udp_payload_size = htons(0x1000); // 4096
  opt->higher_bits = 0x00;
  opt->ends0_ver = 0x00;
  // separar
  opt->z = htons(0x8000);
  opt->length = htons(0);

  dnsServerConn.write("\x00\x20", 2);

  dnsServerConn.write((char *) header, sizeof(struct dns_hdr));
  dnsServerConn.write('\x03'); // namelen
  dnsServerConn.write((char *) name, 4);
  dnsServerConn.write("\x00\x01\x00\x01", 4);
  dnsServerConn.write((char *) opt, sizeof(struct dns_opt));
  dnsServerConn.flush();

  Serial.println("Esperando respuesta");
  // Response
  while (!dnsServerConn.available()){
      Serial.print(".");
      delay(100);
  }
  Serial.println("");
  Serial.println("Respuesta");


  uint16_t len, rdlen;
  assert(dnsServerConn.read((uint8_t *) &len, 2) == 2);
  len = ntohs(len);
  assert(len <= MAX_DNS_PAYLOAD);
  rdlen = dnsServerConn.read(payloadBuff, len);
  Serial.print("Tam leido ");
  Serial.print(rdlen);
  Serial.print(" Tam supuesto ");
  Serial.println(len);

  Serial.print("Respuestas: ");
  Serial.println(ntohs(header->numanswers));
  Serial.print("Id: ");
  Serial.println(ntohs(header->id));

  PRINTSTBS(header, id);
  PRINTSTHEX(header, flags1);
  PRINTSTHEX(header, flags2);
  PRINTSTBS(header, numquestions);
  PRINTSTBS(header, numanswers);
  PRINTSTBS(header, numauthrr);
  PRINTSTBS(header, numextrarr);

  char *cur = (char *) header + sizeof(struct dns_hdr);
  int slen;

  for (int i = 0; i < ntohs(header->numquestions); i++){
      slen = *cur;
      Serial.println(slen);
      cur++;
      Serial.print("Name: "); Serial.println(cur);
      query = (struct dns_query *) cur + slen;
      Serial.print("Type: ");
      Serial.print(ntohs(query->type));
      Serial.print(" Class: ");
      Serial.println(ntohs(query->cls));
  }

/*
  for (int row = 0; dnsServerConn.available(); row++){
    for (int col = 0; dnsServerConn.available() && col < 20; col++){
        int data = dnsServerConn.read();
        if (data >= 0){
            Serial.print(data, 16);
            Serial.print(" ");
        }
    }
    Serial.println("");
  }
*/
}

void loop() {

}
