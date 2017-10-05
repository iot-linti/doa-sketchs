/* strtok example */
#include <stdio.h>
#include <string.h>

char currentVersion[] = "4.1.7.3-b.7";
char newVersion[] = "4.1.7.4-a.7";
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
  while (result == 0 && curstr != NULL && newstr != NULL) {
    result = strcmp(newstr, curstr);
    if (result > 0) {
      Serial.printf("%s > %s => bajar nueva version\n", newstr, curstr);
    } else if (result < 0) {
      Serial.printf("%s < %s => dejar version actual\n", newstr, curstr);
    } else {
      Serial.printf("%s == %s => seguir comparando\n", newstr, curstr);
    }
    curstr = strtok_r(NULL, delim, (char**) &buffer1);
    newstr = strtok_r(NULL, delim, (char**) &buffer2);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
