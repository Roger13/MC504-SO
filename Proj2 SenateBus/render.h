#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

char buff[70];

char busTop[50], busMid[50], busBot[50];

void clear();
void init();
void moveTo(int x, int y);
void printBox(int x, int y, int w, int h);
void printFrame();
void printBus(int x, int y);
void printRectangle(int x1, int y1, int x2, int y2, char* s);
void logBegin();
void logEnd();
void clearBus(int x, int y);
void busLeave();
void busArrive();
void printIcon(int x, int y, char* s);
void moveToSpot(int spot, char* icon);
void moveToBus(int spot, char* icon);
void generateIcon(char* tar);
void printFila(int n);
void printPonto(int n);
void printOnibus(int n);
void end();
