#include <stdio.h>
#include <string.h>

#define USE_SERIAL Serial

#define DEBUG
#ifdef DEBUG
#define DBG_PRINT(...) USE_SERIAL.printf(__VA_ARGS__)
#else
#define DBG_PRINT(...)
#endif /* DEBUG */

/*  Returns:
*   1: currentVersion < newVersion
*   0: currentVersion == newVersion
*  -1: currentVersion > newVersion
*/
int versionCompare(char * currentVersion, char * newVersion) {
  char *curstr, *newstr;
  char delim[] = " .-";
  char buffer1[15];
  char buffer2[15];
  int result = 0;
  char tempCur[15];
  char tempNew[15];

  USE_SERIAL.printf("Current version:\t%s\n", currentVersion);
  USE_SERIAL.printf("New version:\t\t%s\n", newVersion);
  USE_SERIAL.printf("\n");

  if (currentVersion == NULL) {
    if (newVersion == NULL) return 0;
    else return 1;
  } else if (newVersion == NULL) return -1;

  /* copiamos los parametros a nuevas variables porque strtok_r intenta modificar directamente los parametros y no puede porque no tiene permiso */
  strcpy(tempCur, currentVersion);
  strcpy(tempNew, newVersion);

  curstr = strtok_r(tempCur, delim, (char **) &buffer1);
  newstr = strtok_r(tempNew, delim, (char **) &buffer2);
  if (curstr != NULL && newstr != NULL) {
    result = strcmp(newstr, curstr);
  }

  while (result == 0 && curstr != NULL && newstr != NULL) {
    DBG_PRINT("%s == %s => seguir comparando\n", newstr, curstr);

    curstr = strtok_r(NULL, delim, (char**) &buffer1);
    newstr = strtok_r(NULL, delim, (char**) &buffer2);
    if (curstr != NULL && newstr != NULL) {
      result = strcmp(newstr, curstr);
    }
  }

  if (result > 0) {
    DBG_PRINT("%s > %s\n", newstr, curstr);
    return 1;
  } else if (result < 0) {
    DBG_PRINT("%s < %s\n", newstr, curstr);
    return -1;
  } else if (newstr != NULL) {
    DBG_PRINT("%s > NULL\n", newstr);
    return 1;
  } else if (curstr != NULL) {
    DBG_PRINT("NULL < %s\n", curstr);
    return -1;
  } else return 0;
}

void setup() {
  delay(2000); // setup delay
  USE_SERIAL.begin(115200);
  USE_SERIAL.printf("\n\n");


}

void loop() {

}
