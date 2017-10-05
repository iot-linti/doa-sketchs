#include <stdio.h>
#include <string.h>

char currentVersion[] = "4.1.7.3-7";
char newVersion[] = "4.1.7.4-7";
char delim[] = " .-";
char *curstr, *newstr;
char buffer1[100];
char buffer2[100];
int result = 0;

void setup() {
  delay(2000); // setup delay
  Serial.begin(115200);
  Serial.printf("\n\n");

  Serial.printf("Old version: %s\n", currentVersion);
  Serial.printf("New version: %s\n", newVersion);

  curstr = strtok_r(currentVersion, delim, (char**) &buffer1);
  newstr = strtok_r(newVersion, delim, (char**) &buffer2);
  result = strcmp(newstr, curstr);      // TODO: USAR ATOIIIIIIIIIIIIIIIIII!!!!!!!!!!!!!!
  while (result == 0 && curstr != NULL && newstr != NULL) {

    if (result > 0) {
      Serial.printf("%s > %s\n", newstr, curstr);
    } else if (result < 0) {
      Serial.printf("%s < %s\n", newstr, curstr);
    } else {
      Serial.printf("%s == %s => seguir comparando\n", newstr, curstr);
    }

    curstr = strtok_r(NULL, delim, (char**) &buffer1);
    newstr = strtok_r(NULL, delim, (char**) &buffer2);
    result = strcmp(newstr, curstr);
  }

  if (result > 0) {
    Serial.printf("newVersion > currentVersion: bajar nueva version\n");
  } else if (result < 0) {
      Serial.printf("currentVersion > newVersion: dejar version actual\n");
  } else {  // result == 0
    if (curstr != NULL) {
      // curstr != NULL, newstr == NULL; currentVersion tiene más campos que newVersion
      Serial.printf("currentVersion > newVersion: dejar version actual\n");
    } else {
      if (newstr != NULL) {
        // curstr == NULL, newstr != NULL; newVersion tiene más campos que oldVersion
        Serial.printf("newVersion > currentVersion: bajar nueva version\n");
      } else {
        // curstr == NULL, newstr == NULL; ambas versiones no tienen más campos
        Serial.printf("currentVersion == newVersion: ambas versiones son iguales\n");
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
