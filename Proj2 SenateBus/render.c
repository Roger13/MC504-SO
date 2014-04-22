#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define KNRM  "\033[0m"
#define KRED  "\033[31m"
#define KGRN  "\033[32m"
#define KYEL  "\033[33m"
#define KBLU  "\033[34m"
#define KMAG  "\033[35m"
#define KCYN  "\033[36m"
#define KWHT  "\033[37m"
#define FRAME_DUR 50000

char buff[70];

char busTop[50], busMid[50], busBot[50];

void clear(){
  printf("\033[2J");
}


void moveTo(int x, int y){
  printf("\033[%d;%df",y,x);
}


void catchABreath(){
  int i;
  for (i=0;i<200;i++) moveTo(1,1);
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

void printFrame(){
  printBox(1,1,60,3);
  printBox(1,4,60,17);
}

void printBus(int x, int y){
  printf("\033[01;33m");
  if (x >= 1){
    moveTo(x,y);
    printf("%s",busTop); moveTo(x,y+1);
    printf("%s",busMid); moveTo(x,y+2);
    printf("%s",busBot);
  } else if (x >-14){
    moveTo(1,y);
    printf("%s",busTop+(1-x)*3*sizeof(char)); moveTo(1,y+1);
    printf("%s",busMid+(1-x)*3*sizeof(char)); moveTo(1,y+2);
    printf("%s",busBot+(1-x)*3*sizeof(char));
  }
  printf("\033[01;37m");
}

void printRectangle(int x1, int y1, int x2, int y2, char* s){
  int i, j;
  for (i = x1; i<=x2; i++) for (j=y1; j<=y2; j++) {
      moveTo(i,j);
      printf("%s",s);
    }
}

void printIcon(int x, int y, char* s){
  moveTo(x,y);
  printf("%s",s);
}

void logBegin(){
  printRectangle(2,2,59,2," ");
  moveTo(2,2);
}

void logEnd(){
  printIcon(60,2,"║");
}

void clearBus(int x, int y){
  printRectangle(x,y,x+12,y+2," ");
}

void busLeave(){
  int i;
  for (i=3;i>-14;i--){
    printBus(i,17);
    printRectangle(i+14,17,i+14,19," ");
    catchABreath();
    moveTo(0,0);
    usleep(50000);
  }
}

void busArrive(){
  int i;
  for (i=59;i>3;i--){
    if (i>46){
      printBus(i,17);
      printRectangle(61,17,61+12,19," ");
    }
    else {
      printRectangle(i+14,17,i+14,19," ");
      printBus(i,17);
    }
    catchABreath();
    usleep(50000);
  }
}


void moveToSpot(int spot, char* icon){
  int x = (spot%28)-1;
  int y = 7 - (spot/28);
  int i=6,j;
    if (x > 0){
      for (i = 6; i < 6+2*x; i++){
	printIcon(i-1,8," ");
	printIcon(i,8,icon);
	catchABreath();
	usleep(FRAME_DUR/10);
      }
    } else if (x < 0) {
      for (i = 6; i > 6+2*x; i--){
	printIcon(i+1,8," ");
	printIcon(i,8,icon);
	catchABreath();
	usleep(FRAME_DUR);
      }
    }
    printIcon(i-1,8,"   ");
  for (j=8;j<8+y;j++){
    if (j!=8)printIcon(i,j-1," ");
    printIcon(i,j,icon);
    catchABreath();
    usleep(FRAME_DUR);
  }
  printIcon(i-1,j-1,icon);
  printIcon(i,j-1," ");
}

void moveToBus(int spot, char* icon){
  int x = 3+2*(spot%28);
  int y = 14-(spot/28);
  int i=x,j=y;
  printIcon(i+1,j,icon);
  printIcon(i,j," ");
  i++;
  
  for (j=j+0; j < 15; j++){
    printIcon(i,j," ");
    printIcon(i,j+1,icon);
    catchABreath();
    usleep(FRAME_DUR/10);
  }
  for (i=i;i>4;i--){
    printIcon(i,15," ");
    printIcon(i-1,15,icon);
    catchABreath();
    usleep(FRAME_DUR);
  }
  printIcon(4,15," ");
}

void generateIcon(char* tar){
  strcpy(tar,"");
  switch(rand()%6){
  case 0:
    strcat(tar,KGRN);
    break;
  case 1:
    strcat(tar,KYEL);
    break;
  case 2:
    strcat(tar,KBLU);
    break;
  case 3:
    strcat(tar,KMAG);
    break;
  case 4:
    strcat(tar,KCYN);
    break;
  }
  switch(rand()%5){
  case 0:
    strcat(tar,"♚");
    break;
  case 1:
    strcat(tar,"♛");
    break;
  case 2:
    strcat(tar,"♜");
    break;
  case 3:
    strcat(tar,"♝");
    break;
  case 4:
    strcat(tar,"♞");
    break;
  case 5:
    strcat(tar,"♟");
    break;
  }
  strcat(tar,KNRM);
}

void init(){
  clear();
  printFrame();
  printRectangle(2,5,5,7,"█");
  printRectangle(9,5,59,7,"█");
  moveTo(6,7);
  printf("⊂|⊃");
  printRectangle(2,16,59,16,"━");
  srand(time(NULL));
  strcpy(busTop,"╭───┬────────╮");
  strcpy(busMid,"│███│▊▊▊▊▊▊▊▊│");
  strcpy(busBot,"╰───┴0──────0╯");
}

void end(){
  char c;
  logBegin();
  printf("Aperte qualquer tecla para sair");
  scanf ("%c",&c);
  clear();
  moveTo(1,1);
}
/*
int main(int argc, char* argv[]){
  int i;
  init();
  moveTo(3,17);
  //printBus(-1,17);
  //printf("%s",busTop+4*3*sizeof(char));
  //  busArrive();
  //busLeave();
  printLog("Sample Log");
  printf("\n");
  srand(time(NULL));
  char buff[20];
  generateIcon(buff);
  for (i=0;i<10;i++){
    generateIcon(buff);
    moveToSpot(i,buff);
  }
  moveToBus(5,buff);
  //  printf("%d\n%d\n",rand(),time(NULL));
  char c;
  scanf("%c",&c);
  clear();
  moveTo(1,1);
  return 0;
}
*/
