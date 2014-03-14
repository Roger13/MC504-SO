#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define LINE 0
#define COLLUMN 1
#define SECTOR 2
#define true 1
#define false 0

typedef struct{
  int** g;
  int id;
  int type;
} VerifData;

typedef struct{
  int success;
  int id;
  int type;
} RetData;

void* thread_verificador(void*v){
  VerifData data = *((VerifData*) v);
  int** grid = data.g;
  int id = data.id;
  int type = data.type;
  free((VerifData*)v);
  int check[9];
  RetData ret;
  ret.success = true;
  ret.id = id;
  ret.type = type;
  int k = 0, i;
  // Inicializa vetor de verificacao
  for (i = 0; i < 9; i++){
    check[i]=false;
  }
  // Para cada valor encontrado na linha,
  // marque a posição correspondente no vetor

  switch (type){
  case LINE:
    for (i = 0; i < 9; i++){
      k = grid[id][i];
      check[k-1]=true;
    }
    break;
  case COLLUMN:
    for (i = 0; i < 9; i++){
      k = grid[i][id];
      check[k-1]=true;
    }
    break;
  case SECTOR:
    for (i = 0; i < 9; i++){
      k = grid[3*(id/3)+(i/3)][3*(id%3)+(i%3)];
      check[k-1]=true;
    }
    break;
  }

  // percorre o vetor de verificação. Se algum numero
  // estiver faltando, ret.success = false.
  for (i = 0; i < 9; i++){
    if (check[i]==false){
      ret.success = false;
      break;
    }
  }
  RetData* r = (RetData*)malloc(sizeof(RetData));
  *r = ret;
  return (void*) r;
}

int verificador(){
  printf("Insira sudoku completo para verificacao.\n");
  int x = 0, i, j, erro = 0; 
  int **grid = (int**)malloc(sizeof(int*)*9);
  for (i = 0; i<9; i++) grid[i]=(int*)malloc(sizeof(int)*9);
  for (i = 0; i<9; i++) for (j = 0; j<9 ; j++){
      scanf("%d",&x);
      if (x<1 || x>9){	
	erro = 2;
	break;
      }
      grid[i][j] = (int)x;
  }
  if (erro == 2){
    printf("Erro no input.\n");
    return 2;
  }

  pthread_t thr[27];
  VerifData* data;
  // Verificador de linhas
  for (i = 0; i<9; i++){
    data = malloc(sizeof(VerifData));
    (*data).g = (void*)grid;
    (*data).id = i;
    (*data).type = LINE;
    pthread_create(&thr[i], NULL, thread_verificador, (void*)data);
  }
  // Verificador de colunas
  for (i = 0; i<9; i++){
    data = malloc(sizeof(VerifData));
    (*data).g = (int**)grid;
    (*data).id = i;
    (*data).type = COLLUMN;
    pthread_create(&thr[i+9], NULL, thread_verificador, (void*)data);
  }
  // Verificador de setores
  for (i = 0; i<9; i++){
    data = malloc(sizeof(VerifData));
    (*data).g = (void*)grid;
    (*data).id = i;
    (*data).type = SECTOR;
    pthread_create(&thr[i+18], NULL, thread_verificador, (void*)data);
  }
  RetData ret;
  RetData *p_ret;
  erro = 0;
  for (i = 0; i<27; i++){
    pthread_join(thr[i], (void**)&p_ret);
    ret = *p_ret;
    free(p_ret);
    if (ret.success == 0){
      erro = 1;
      printf("Erro encontrado n");
      switch (ret.type){
      case LINE:
	printf("a linha ");
	break;
      case COLLUMN:
	printf("a coluna ");
	break;
      case SECTOR:
	printf("o setor ");
	break;
      }
      printf("%d\n", ret.id + 1);
    }
  }
  if (erro == 0) printf ("Nenhum erro encontrado na verificacao\n");
    return erro;
};

int main (int argc, char* argv[]){
  printf("Escolha uma opção:\n\nv - Verificar grid pronta\n");
  char input;
  scanf("%c",&input);
  switch(input){
  case 'v':
    verificador();
    break;
  }

  return 0;
}
