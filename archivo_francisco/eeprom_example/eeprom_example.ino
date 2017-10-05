
#include <EEPROM.h>
#include <stdio.h>
#include <string.h>

#define USE_SERIAl Serial

int addr = 0;
char newVersion[] = "40.100.01";

char delim[] = " .-";
char *newstr;
char buffer[100];
char version[30];


void write_eeprom(int addr, char* newVersion);
void read_version(int addr);

void setup() {

  USE_SERIAl.begin(115200);
  delay(2000);
  EEPROM.begin(512);
  USE_SERIAl.println("Inicio");
  write_eeprom(addr,newVersion);
  EEPROM.commit();
  read_version(addr);
  
}

void loop() { 

}

void write_eeprom(int addr, char* newVersion){
  newstr = strtok_r(newVersion, delim, (char**) &buffer);
  while (newstr != NULL){
    USE_SERIAl.printf("Digito: %s ",newstr);
    USE_SERIAl.printf("\n\n");
    EEPROM.write(addr, atoi(newstr));
    addr++;
    newstr = strtok_r(NULL, delim, (char**) &buffer);
    }
  }

void read_version(int addr){
  USE_SERIAl.printf("\n");
  USE_SERIAl.printf("La version actual es %d.%d.%d", EEPROM.read(addr),EEPROM.read(addr+1),EEPROM.read(addr+2));
  //sprintf(version,"%d.%d.%d",EEPROM.read(addr),EEPROM.read(addr+1),EEPROM.read(addr+2));
  }
