#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define N_THR 27

typedef struct{
  void* g;
  int id;
  int type;
} Data;

void* thread_verificador(void*v){
  Data* data = (Data*) v;
  int** grid = (int**)(*data).g;
  int id = (*data).id;
  int type = (*data).type;
  free(data);
  int check[9];
  void* r = (void*)1;
  int k = 0, i;
  // Inicializa vetor de verificacao
  for (i = 0; i < 9; i++){
    check[i]=0;
  }
  // Para cada valor encontrado na linha,
  // marque a posição correspondente no vetor
  switch (type){
  case 0:
    for (i = 0; i < 9; i++){
      k = grid[id][i];
      check[k]=1;
    }
    break;
  case 1:
    for (i = 0; i < 9; i++){
      k = grid[i][id];
      check[k]=1;
    }
    break;
  case 2:
    for (i = 0; i < 9; i++){
      k = grid[3*(id/3)+(i/3)][3*(id%3)+(i%3)];
      check[k]=1;
    }
    break;
  }
  // Percorre o vetor de verificação. Se algum numero
  // estiver faltando, r = 0.
  for (i = 0; i < 9; i++){
    if (check[i]==0){
      r = (void*)0;
      break;
    }
  }
  return (void*) r;
}

void verificador(){
  printf("Insira sudoku completo para verificacao.\n");
  int grid[9][9];
  int x = 0, i, j; 
  for (i = 0; i<9; i++) for (j = 0; j<9 ; j++){
      scanf("%d",&x);
      grid[i][j] = (int)x;
  }
  pthread_t thr[N_THR];
  Data* data;
  // Verificador de linhas
  for (i = 0; i<9; i++){
    data = malloc(sizeof(Data));
    (*data).g = (void*)grid;
    (*data).id = i;
    (*data).type = 0;
    pthread_create(&thr[i], NULL, thread_verificador, (void*)data);
  }
      for (i = 0; i<9; i++){
    data = malloc(sizeof(Data));
    (*data).g = (void*)grid;
    (*data).id = i;
    (*data).type = 1;
    pthread_create(&thr[i+9], NULL, thread_verificador, (void*)data);
  }
  for (i = 0; i<9; i++){
    data = malloc(sizeof(Data));
    (*data).g = (void*)grid;
    (*data).id = i;
    (*data).type = 2;
    pthread_create(&thr[i+18], NULL, thread_verificador, (void*)data);
  }
  int r = 1;
  for (i = 0; i<27; i++){
    //  if (pthread_join(&thr[i])==(void*)0)r = 0;
  }
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
