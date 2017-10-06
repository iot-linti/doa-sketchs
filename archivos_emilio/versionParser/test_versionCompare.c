#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#ifdef DEBUG
#define DBG_PRINT(...) printf(__VA_ARGS__)
#else
#define DBG_PRINT(...)
#endif /* DEBUG */

int compareInts(int a, int b) {
  if (a < b) return -1;
  else if (a > b) return 1;
  else return 0;
}

/*  Returns:
*   1: currentVersion < newVersion
*   0: currentVersion == newVersion
*  -1: currentVersion > newVersion
*/
int versionCompare(char * currentVersion, char * newVersion) {
  char *curstr, *newstr;
  char delim[] = " .-";
  char *ptrCur;
  char *ptrNew;
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

  curstr = strtok_r(tempCur, delim, (char **) &ptrCur);
  newstr = strtok_r(tempNew, delim, (char **) &ptrNew);
  if (curstr != NULL && newstr != NULL) {
    result = compareInts(atoi(newstr), atoi(curstr));
  }

  while (result == 0 && curstr != NULL && newstr != NULL) {
    DBG_PRINT("%s == %s\n", newstr, curstr);

    curstr = strtok_r(NULL, delim, (char**) &ptrCur);
    newstr = strtok_r(NULL, delim, (char**) &ptrNew);
    if (curstr != NULL && newstr != NULL) {
      result = compareInts(atoi(newstr), atoi(curstr));
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
    printf("newVersion > currentVersion:\tbajar nueva version\n");
  } else if (result < 0) {
    printf("currentVersion > newVersion:\tdejar version actual\n");
  } else {
    printf("currentVersion == newVersion:\tambas versiones son iguales\n");
  }
}

int main() {
  printf("\n");

  //                           current        new
  printResult(versionCompare("4.1.7.3-7", "4.1.7.4-7"));
  printf("RESULTADO ESPERADO:\t\tbajar nueva version\t(new > current)\n\n\n");

  printResult(versionCompare("4.1.7.5-7", "4.1.7.4-7"));
  printf("RESULTADO ESPERADO:\t\tdejar version actual\t(current > new)\n\n\n");

  printResult(versionCompare("4.1.7.5-7.4", "4.1.7.5-7"));
  printf("RESULTADO ESPERADO:\t\tdejar version actual\t(son iguales excepto que current tiene mas campos)\n\n\n");

  printResult(versionCompare("4.1.7.5-7", "4.1.7.5-7.6"));
  printf("RESULTADO ESPERADO:\t\tbajar nueva version\t(son iguales excepto que new tiene mas campos)\n\n\n");

  printResult(versionCompare("4.1.7.5-7", "4.2.7"));
  printf("RESULTADO ESPERADO:\t\tbajar nueva version\t(new > current)\n\n\n");

  printResult(versionCompare("4.3", "4.1.7.5-6"));
  printf("RESULTADO ESPERADO:\t\tdejar version actual\t(current > new)\n\n\n");

  printResult(versionCompare("4.1.7.5-7", "4.1.7.5-7"));
  printf("RESULTADO ESPERADO:\t\tambas versiones son iguales\t(current == new)\n\n\n");

  printResult(versionCompare(NULL, "4.1.7"));
  printf("RESULTADO ESPERADO:\t\tbajar nueva version\t(current es NULL)\n\n\n");

  printResult(versionCompare("4.1.4", NULL));
  printf("RESULTADO ESPERADO:\t\tdejar version actual\t(new es NULL)\n\n\n");

  printResult(versionCompare(NULL, NULL));
  printf("RESULTADO ESPERADO:\t\tambas versiones son iguales\t(current == new == NULL)\n\n\n");

  printResult(versionCompare("5.10.2", "5.7.3"));
  printf("RESULTADO ESPERADO:\t\tdejar version actual\t(current > new)\n\n\n");

  printResult(versionCompare("5.233.2", "5.234.3"));
  printf("RESULTADO ESPERADO:\t\tbajar nueva version\t(new > current)\n\n\n");

  // TODO: pruebas con numeros y letras (por ej. 5.10b.3)

  printf("Fin del programa.\n");
  return 0;
}