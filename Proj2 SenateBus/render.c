#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

char buff[70];

void clear(){
  printf("\033[2J");
}


void moveTo(int x, int y){
  printf("\033[%d;%df",y,x);
}


void printBox(int x, int y, int w, int h){
  int i;
  moveTo(x,y);
  printf("╔");
  for (i = 0; i< w - 2; i++) printf("═");
  printf("╗");
  for (i = 0; i< h - 2; i++){
    moveTo(x,y+i+1);
    printf("║");
    moveTo(x+w-1,y+i+1);
    printf("║");
  }
  moveTo(x,y+h-1);
  printf("╚");
  for (i = 0; i< w - 2; i++) printf("═");
  printf("╝");
}

void printBus(int x, int y){
  moveTo(x,y);
  printf("\033[01;33m");
  printf("╭───┬────────╮"); moveTo(x,y+1);
  printf("│███│▊▊▊▊▊▊▊▊│"); moveTo(x,y+2);
  printf("╰───┴0──────0╯");
  printf("\033[01;37m");
}

void printRectangle(int x1, int y1, int x2, int y2, char* s){
  int i, j;
  for (i = x1; i<=x2; i++) for (j=y1; j<=y2; j++) {
      moveTo(i,j);
      printf(s);
    }
}

void clearBus(int x, int y){
  printRectangle(x,y,x+12,y+2," ");
}

void printThread(int x, int y, int n){
  moveTo(x,y);
  printf("☺ {%d)",n);
}

void printLog(char* s){
  moveTo(2,2);
  printf(s);
}

int main(int argc, char* argv[]){

  clear();

  printBox(1,1,60,3);

  printBox(1,4,60,17);
  printRectangle(2,5,5,7,"█";
  printRectangle(9,5,59,7,"█");
  moveTo(6,7);
  printf("⊂|⊃");
  printRectangle(2,16,59,16,"━");
  printBus(3,17);
  printLog("Sample Log");
  printf("\n");
  srand(time(NULL));
  //  printf("%d\n%d\n",rand(),time(NULL));
  char c;
  scanf("%c",&c);
  clear();
  moveTo(1,1);
  return 0;
}
