
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define LINE 0
#define COLLUMN 1
#define SECTOR 2
#define true 1
#define false 0

// this code is good code

typedef struct{
  int** data;
  int error;
} SudokuData;

typedef struct{
  int** g;
  int id;
  int type;
} VerifData;

typedef struct{
  int x, y;
  int ** g;
  SudokuData data;
} HintData;

typedef struct{
  int missing;
  int id;
  int type;
} RetData;

typedef struct PthreadNode{
  pthread_t data;
  struct PthreadNode * next;
} PNode;

void pushPNode(PNode** head){
  PNode* pushee = (PNode*)malloc(sizeof(PNode));
  pushee->next = *head;
  *head = pushee;
}
PNode* popPNode(PNode** head){
  if (*head == NULL) return NULL;
  PNode* ret = (*head);
  *head = (*head)->next;
  return ret;
}

int** malloc_grid(){
  int i, j;
  int **grid = (int**)malloc(sizeof(int*)*9);
  for (i = 0; i<9; i++) {
    grid[i]=(int*)malloc(sizeof(int)*9);
  }
  return grid;
}

SudokuData malloc_sudokuData(){
  int i,j;
  int **grid = (int**)malloc(sizeof(int*)*3);
  for (i = 0; i<3; i++) {
    grid[i]=(int*)malloc(sizeof(int)*9);
    for (j=0; j<9;j++) grid[i][j]=0;
  }
  SudokuData ret;
  ret.data = grid;
  ret.error = 0;
  return ret;
}

int scan_sudoku (int** grid){
  int i,j,k;
  char x;
  for (i = 0; i<9; i++) {
    for (j = 0; j<9 ; j++){
      scanf(" %c",&x);
      if (x == 'X' || x == 'x') k = 10;
      else if (x >= '1' && x <= '9') k = (int)(x - '0');
      else return 1;
      grid[i][j] = k;
    }
  }
  return 0;
}

int toBinRep(int digit){
  switch (digit){
  case 1:
    return 1;
  case 2:
    return 2;
  case 3:
    return 4;
  case 4:
    return 8;
  case 5:
    return 16;
  case 6:
    return 32;
  case 7:
    return 64;
  case 8:
    return 128;
  case 9:
    return 256;
  default:
    return 0;
  }
}

int containsData(int data, int digit){
  return data & toBinRep(digit);
}


void print_sudoku (int** grid){
  int i,j,k,l;
  for (i=0;i<9;i++){
    for (j=0;j<9;j++){
      if (grid[i][j] > 0){
	printf(" %d      ",grid[i][j]);
      }
      else{
	l = 0;
	for (k = 1; k <= 9; k++) if (containsData(-grid[i][j],k))l++;
	// Todo: centering
	printf("(");
	for (k = 1; k <= 9; k++) if (containsData(-grid[i][j],k)) printf("%d",k);
	printf(")");
	for (k = 0; k < 6-l; k++) printf(" ");
      }
    }
    printf("\n");
  }
}

void* thread_verificador(void*v){
  VerifData data = *((VerifData*) v);
  int** grid = data.g;
  int id = data.id;
  int type = data.type;
  free((VerifData*)v);
  int check[9];
  RetData ret;
  ret.missing = 0;
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
  // estiver faltando, grave-o no int Missing.

  for (i = 0; i < 9; i++){
    if (check[i]==false){
      ret.missing += toBinRep(i+1);
    }
  }

  RetData* r = (RetData*)malloc(sizeof(RetData));
  *r = ret;
  return (void*) r;
}


void* thread_dica(void*v){
  HintData param = *((HintData*)v);
  free(v);
  int 
    x = param.x,
    y = param.y;
  int
    a = param.data.data[LINE][y],
    b = param.data.data[COLLUMN][x],
    c = param.data.data[SECTOR][x/3 + 3*(y/3)];
  int ret = a & b & c;
  //if (ret == 0) return NULL;
  param.g[y][x] = -ret;
  return NULL;// (void*)&(param.g[y][x]);
}

SudokuData sudokuChecker (int** grid){
  int i, j;
  SudokuData ret = malloc_sudokuData();
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
  RetData *p_ret;
  for (i = 0; i<27;i++){
    pthread_join(thr[i],(void**)&p_ret);
    if (p_ret->missing != 0) ret.error = 1;
    (ret.data)[p_ret->type][p_ret->id]=p_ret->missing;
    free(p_ret);
  }
  return ret;
}

int verificador(){
  printf("Insira sudoku completo para verificacao.\n");
  int x = 0, i, j, k;

  int** grid = malloc_grid();
  i = scan_sudoku(grid);
  if (i){
    printf("Erro no input.\n");
    return 2;
  }

  SudokuData missing = sudokuChecker(grid);

  if (missing.error){
    for (i=0; i<3; i++){
      for (j=0;j<9;j++){
	if (missing.data[i][j]!=0){
	  printf("Erro encontrado n");
	  switch (i){
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
	  printf("%d: falta(m) o(s) numero(s)", j + 1);
	  for (k = 1; k<=9; k++) if (containsData(missing.data[i][j],k)) printf(" %d", k);
	  printf(".\n");
	}
      }
    }
  }
  else  printf("Nenhum erro encontrado na verificacao\n");
  return missing.error;
}

int hint_generator(int** grid, SudokuData missingData){
  int i,j, error=0;
  PNode* pnodeStack = NULL;
  HintData* data;
  for (i = 0; i<9; i++) {
    for (j = 0; j < 9; j++){
      if (grid[i][j] == 10){
	data = (HintData*)malloc(sizeof(HintData));
	data->x = j;
	data->y = i;
	data->g = grid;
	data->data = missingData;
	pushPNode(&pnodeStack);
	pthread_create(&(pnodeStack->data), NULL, thread_dica, (void*) data);
      }
    }
  }
  PNode* temp;
  void* ret;
  error = 0;
  
  while (pnodeStack){
    temp = popPNode(&pnodeStack);
    pthread_join(temp->data, &ret);
    free(temp);
    //if (ret == NULL) error = 1;
  }

  return error;

  // Retorno: 0 se sem problemas
  //          1 se alguma celula ficou sem possibilidades

}

//int** grid = malloc_grid();
//if (scan_sudoku(grid)) // erro 2;
//if (hint_generator(grid,sudoku_checker(grid));

int dica(){
  printf("Insira sudoku incompleto para geração de dicas.\n");
  int x = 0, i, j, erro = 0;
  int** grid = malloc_grid();
  if (scan_sudoku(grid)){
    printf("Erro no input.\n");
    return 2;
  }
  if(hint_generator(grid, sudokuChecker(grid))){
    printf("Erro na verificação.\n");
    return 1;
  }

  print_sudoku(grid);
  return 0;
}

int resolvedor (){
  printf("Insira sudoku incompleto para resolução.\n");
  int x = 0, i, j, erro = 0;
  int** grid = malloc_grid();
  if (scan_sudoku(grid)){             // Obtem a matriz com os coeficientes do sudoku
    printf("Erro no input.\n");
    return 2;
  }
  if(hint_generator(grid, sudokuChecker(grid))){
    printf("Erro na verificação.\n");
    return 1;
  }

  // Chama funcao recursiva preenchedora

  if (backTracker(grid) == 0){
    printf("Sudoku insolúvel!\n");
  }

  print_sudoku(grid);// IMPRIMIR MATRIZ PREENCHIDA
}

void* thread_check(void*v){
  VerifData data = *((VerifData*) v);
  int** grid = data.g;
  int id = data.id;
  int type = data.type;
  int check[9];

  printf("Thread %d",id);
  
  int k = 0, i;
  // Inicializa vetor de verificacao
  for (i = 0; i < 9; i++){
    check[i]=0;
  }
  // Para cada valor encontrado na linha,
  // marque a posição correspondente no vetor

  switch (type){
  case LINE:
    for (i = 0; i < 9; i++){
      k = grid[id][i];
      check[k-1]++;
    }
    break;
  case COLLUMN:
    for (i = 0; i < 9; i++){
      k = grid[i][id];
      check[k-1]++;
    }
    break;
  case SECTOR:
    for (i = 0; i < 9; i++){
      k = grid[3*(id/3)+(i/3)][3*(id%3)+(i%3)];
      check[k-1]++;
    }
    break;
  }

  // percorre o vetor de verificação. Se algum numero
  // estiver repitido mais de uma vez retorna 0
  int ret = 1;

  for (i = 0; i < 9; i++){
    if (check[i]>1){
      ret = 0;
    }
  }
  int* r = (int*)malloc(sizeof(int));
  *r = ret;
  return (void*) r;
}

int backTracker(int** grid){
  int i,j;
  pthread_t thr[3];
  VerifData data[3];
  // Inicia o vetor de dados
  for(i = 0; i < 3; i++){
    data[i].g = grid;
    data[i].type = i;
  }
  
  for(i = 0; i < 81; i++){ //Percorre grid em busca da palavra de dicas
    if(grid[i/9][i%9] < 0){ //Caso o elemento tenha uma palavra de dicas
      data[0].id = i/9;
      data[1].id = i%9;
      data[2].id = (i%9)/3 + 3*((i/9)/3);
      for(j = 1; j <= 9; j++){  //Procura os elementos da palavra
	if(containsData(-grid[i/9][i%9],j)){
	  grid[i/9][i%9] = j; //Atribui o primeiro valor da palavra
	  //Checa se viola linha, col ou setor
	  pthread_create(&thr[0], NULL, thread_check, (void*)&(data[0]));
	  pthread_create(&thr[1], NULL, thread_check, (void*)&(data[1]));
	  pthread_create(&thr[2], NULL, thread_check, (void*)&(data[2]));

	  int *p_ret;
	  int total = 0;

	  int k;
	  for(k = 0; k < 3; k++){
	    pthread_join(thr[k],(void**)&p_ret);
	    total += *p_ret;
	  }
	  if(total < 3){  //Caso uma das threads acuse erro
	    continue;
	  }
	  else if(backTracker(grid) == 1){ //Se recursão suceder
	    return 1;
	  }
	}
      }
      return 0; //Caso "vetor" de dicas tenha esgotado sem sucesso
    }
    else if(grid[i/9][i%9] == 0){
      printf("Encontrado valor invalido");
    }
  }
  return 1;
}

int main (int argc, char* argv[]){
  printf("Escolha uma opção:\n\nv - Verificar grid pronta\nd - Gerar dicas para sudoku incompleto\nr - Resolver sudoku incompleto\n");
  char input;
  scanf("%c",&input);
  switch(input){
  case 'v':
    verificador();
    break;
  case 'd':
    dica();
    break;
  case 'r':
    resolvedor();
    break;
  }

  return 0;
}
