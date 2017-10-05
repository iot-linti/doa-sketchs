#include <stdio.h>
#include <string.h>

void versionCompare(char * currentVersion, char * newVersion) {
  char *curstr, *newstr;
  char delim[] = " .-";
  char buffer1[15];
  char buffer2[15];
  int result = 0;

  /* hacemos esto porque strtok_r intenta modificar los parametros y no se puede porque no tiene permiso */
  char tempCur[15];
  char tempNew[15];
  strcpy(tempCur, currentVersion);
  strcpy(tempNew, newVersion);

  if (tempCur != NULL) {
    printf("Old version: %s\n", currentVersion);
  }
  if (tempNew != NULL) {
    printf("New version: %s\n", newVersion);
  }
  printf("\n");

  curstr = strtok_r(tempCur, delim, (char **) &buffer1);
  newstr = strtok_r(tempNew, delim, (char **) &buffer2);
  if (curstr != NULL && newstr != NULL) {
    result = strcmp(newstr, curstr);
  }

  while (result == 0 && curstr != NULL && newstr != NULL) {
    if (result > 0) {
      printf("%s > %s\n", newstr, curstr);
    } else if (result < 0) {
      printf("%s < %s\n", newstr, curstr);
    } else {
      printf("%s == %s => seguir comparando\n", newstr, curstr);
    }

    curstr = strtok_r(NULL, delim, (char**) &buffer1);
    newstr = strtok_r(NULL, delim, (char**) &buffer2);
    if (curstr != NULL && newstr != NULL) {
      result = strcmp(newstr, curstr);
    }
  }

  if (result > 0) {
    printf("newVersion > currentVersion: bajar nueva version\n");
  } else if (result < 0) {
      printf("currentVersion > newVersion: dejar version actual\n");
  } else {  // result == 0
    if (curstr != NULL) {
      // curstr != NULL, newstr == NULL; currentVersion tiene más campos que newVersion
      printf("currentVersion > newVersion: dejar version actual\n");
    } else {
      if (newstr != NULL) {
        // curstr == NULL, newstr != NULL; newVersion tiene más campos que oldVersion
        printf("newVersion > currentVersion: bajar nueva version\n");
      } else {
        // curstr == NULL, newstr == NULL; ambas versiones no tienen más campos
        printf("currentVersion == newVersion: ambas versiones son iguales\n");
      }
    }
  }
}





int main() {
  // delay(2000); // setup delay
  // Serial.begin(115200);
  printf("\n\n");

  //               current        new
  versionCompare("4.1.7.3-7", "4.1.7.4-7");
  printf("RESULTADO ANTERIOR DEBE HABER SIDO: bajar nueva version (new > current)");
  printf("\n\n");
  versionCompare("4.1.7.5-7", "4.1.7.4-7");
  printf("RESULTADO ANTERIOR DEBE HABER SIDO: dejar version actual (current > new)");
  printf("\n\n");
  versionCompare("4.1.7.5-7.4", "4.1.7.5-7");
  printf("RESULTADO ANTERIOR DEBE HABER SIDO: dejar version actual (son iguales excepto que current tiene mas campos)");
  printf("\n\n");
  versionCompare("4.1.7.5-7", "4.1.7.5-7.6");
  printf("RESULTADO ANTERIOR DEBE HABER SIDO: bajar nueva version (son iguales excepto que new tiene mas campos)");
  printf("\n\n");
  versionCompare("4.1.7.5-7", "4.1.7");
  printf("RESULTADO ANTERIOR DEBE HABER SIDO: dejar version actual (son iguales excepto que current tiene mas campos)");
  printf("\n\n");
  versionCompare("4.1", "4.1.7.5-6");
  printf("RESULTADO ANTERIOR DEBE HABER SIDO: bajar nueva version (son iguales excepto que new tiene mas campos)");
  printf("\n\n");
  versionCompare("4.1.7.5-7", "4.1.7.5-7");
  printf("RESULTADO ANTERIOR DEBE HABER SIDO: ambas versiones son iguales (current == new)");
  versionCompare(NULL, "4.1.7");
  printf("RESULTADO ANTERIOR DEBE HABER SIDO: bajar nueva version (current es NULL");
  versionCompare("4.1.4", NULL);
  printf("RESULTADO ANTERIOR DEBE HABER SIDO: dejar version actual (new es NULL)");

  return 0;
}