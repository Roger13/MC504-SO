#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define N_THR 27

void verificador(){
  printf("Insira sudoku completo para verificacao.\n");
  char grid[9][9];
  int x = 0;
  for (int i = 0; i<9; i==) for (int j = 0; j<9 ; j++){
      scanf("%d",&x);
      grid[i][j] = (char)x;
  }
  pthread_t thr[N_THR];
  
};

int main (int argc, char* argv[]){
  printf("Escolha uma opção:\nv - Verificar grid pronta");
  char input;
  scanf("%c",&input);
  switch(input){
  case 'v':
    verificador();
    break;
  }

  return 0;
}
