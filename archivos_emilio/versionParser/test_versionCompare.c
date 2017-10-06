/*  Returns:
*   1: currentVersion < newVersion
*   0: currentVersion == newVersion
*  -1: currentVersion > newVersion
*/
#include <stdio.h>
#include <string.h>

#define DEBUG

#ifdef DEBUG
#define DBG_PRINT(...) printf(__VA_ARGS__)
#else
#define DBG_PRINT(...)
#endif /* DEBUG */

int versionCompare(char * currentVersion, char * newVersion) {
  char *curstr, *newstr;
  char delim[] = " .-";
  char buffer1[15];
  char buffer2[15];
  int result = 0;
  char tempCur[15];
  char tempNew[15];

  printf("Current version:\t%s\n", currentVersion);
  printf("New version:\t\t%s\n", newVersion);
  printf("\n");

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

void printResult(int result) {
  if (result > 0) {
    printf("newVersion > currentVersion: bajar nueva version\n");
  } else if (result < 0) {
    printf("currentVersion > newVersion: dejar version actual\n");
  } else {
    printf("currentVersion == newVersion: ambas versiones son iguales\n");
  }
}

int main() {
  printf("\n");

  //                           current        new
  printResult(versionCompare("4.1.7.3-7", "4.1.7.4-7"));
  printf("RESULTADO ESPERADO: bajar nueva version\n(new > current)\n\n\n");

  printResult(versionCompare("4.1.7.5-7", "4.1.7.4-7"));
  printf("RESULTADO ESPERADO: dejar version actual\n(current > new)\n\n\n");

  printResult(versionCompare("4.1.7.5-7.4", "4.1.7.5-7"));
  printf("RESULTADO ESPERADO: dejar version actual\n(son iguales excepto que current tiene mas campos)\n\n\n");

  printResult(versionCompare("4.1.7.5-7", "4.1.7.5-7.6"));
  printf("RESULTADO ESPERADO: bajar nueva version\n(son iguales excepto que new tiene mas campos)\n\n\n");

  printResult(versionCompare("4.1.7.5-7", "4.2.7"));
  printf("RESULTADO ESPERADO: bajar nueva version\n(new > current)\n\n\n");

  printResult(versionCompare("4.3", "4.1.7.5-6"));
  printf("RESULTADO ESPERADO: dejar version actual\n(current > new)\n\n\n");

  printResult(versionCompare("4.1.7.5-7", "4.1.7.5-7"));
  printf("RESULTADO ESPERADO: ambas versiones son iguales\n(current == new)\n\n\n");

  printResult(versionCompare(NULL, "4.1.7"));
  printf("RESULTADO ESPERADO: bajar nueva version\n(current es NULL)\n\n\n");

  printResult(versionCompare("4.1.4", NULL));
  printf("RESULTADO ESPERADO: dejar version actual\n(new es NULL)\n\n\n");

  printResult(versionCompare(NULL, NULL));
  printf("RESULTADO ESPERADO: ambas versiones son iguales\n(current == new == NULL)\n\n\n");

  printResult(versionCompare("5.10.2", "5.7.3"));
  printf("RESULTADO ESPERADO: bajar nueva version (ERRONEO)\n(current > new, pero al comparar '10' contra '7' se compara el 1 con el 7)\n\n\n");

  return 0;
}