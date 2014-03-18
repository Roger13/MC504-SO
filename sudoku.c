// Pedro Ferrazoli Ciambra 137268
// Rogerio de Oliveira Bernardo 140922
// Sudoku Multithread - MC504 - 1S2014

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define LINE 0
#define COLLUMN 1
#define SECTOR 2
#define true 1
#define false 0

// Structs usados como parametros e valores de retorno pelas threads.

// SudokuData retorna uma matriz 3x9 com os vetores de bit contendo as
// informacoes sobre quais numeros faltam em cada linha, coluna ou setor;
// O valor de erro poupa processamento na thread de dica.
typedef struct{
  int** data;
  int error;
} SudokuData;

// VerifData eh a struct utilizada como parametro para a thread de verificacao.
// Envia o ponteiro para a matriz, o tipo de verificacao (linha, coluna ou
// setor) e o identificador da verificacao (de 0 a 8).
typedef struct{
  int** g;
  int id;
  int type;
} VerifData;

// HintData eh o parametro da thread de dica. Envia a posicao x e y da celula
// a ser verificada, o apontador para a grid e a informacao sobre os valores
// faltantes.
typedef struct{
  int x, y;
  int ** g;
  SudokuData data;
} HintData;

// RetData eh o valor de retorno da thread de verificacao.
// Retorna o tipo e o identificador da linha/coluna/setor que verificou,
// bem como o vetor de bits que armazena quais os valores faltantes obtidos.
typedef struct{
  int missing;
  int id;
  int type;
} RetData;

// BEGIN obsessao com eficiencia de memoria

// Implementacao de lista ligada para as threads do verificador de dica.

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
// END

// Encapsula a criacao de uma grid com malloc, para o
// apontador poder ser passado por referencia.
int** malloc_grid(){
  int i, j;
  int **grid = (int**)malloc(sizeof(int*)*9);
  for (i = 0; i<9; i++) {
    grid[i]=(int*)malloc(sizeof(int)*9);
  }
  return grid;
}

void free_grid(int** g){
  int i;
  for (i=0; i<9; i++)free(g[i]);
  free(g);
}

// Similar ao anterior, aloca um SudokuData.
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

void free_sudokuData(SudokuData s){
  int i;
  for (i=0; i<3; i++)free(s.data[i]);
  free(s.data);
}

// Encapsulamento do leitor de Sudoku.
// Os numeros dados sao armazenados de 1 a 9,
// espacos vazios (X) sao 10. Futuramente,
// vetores de bit para valores faltantes serao
// armazenados como numeros nao-positivos.
// Retorna 1 se nao reconhecer o input.
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

// Potencias de 2 para implementacao de vetor de bit.
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

// Teste do vetor de bit. Bitwise And for the win.
int containsData(int data, int digit){
  return data & toBinRep(digit);
}

// Impressao do sudoku. Feito para ser identico ao
// do enunciado; existe a possibilidade de
// descentralizacao se o numero de possibilidades
// de alguma celula for muito grande.
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
	printf("(");
	for (k = 1; k <= 9; k++) if (containsData(-grid[i][j],k)) printf("%d",k);
	printf(")");
	for (k = 0; k < 6-l; k++) printf(" ");
      }
    }
    printf("\n");
  }
}

// INICIO DAS THREADS

// Thread de verificacao: Recebe um VerifData com o tipo de verificacao
// e o identificador dela; retorna esses valores e um vetor de bit
// com os numeros nao encontrados na linha, vetor ou coluna.

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
      k = grid[3*(id/3)+(i/3)][3*(id%3)+(i%3)]; // <3 operador de modulo <3
      check[k-1]=true;
    }
    break;
  }

  // percorre o vetor de verificação. Se algum numero
  // estiver faltando, grave-o no vetor de bits.

  for (i = 0; i < 9; i++){
    if (check[i]==false){
      ret.missing += toBinRep(i+1);
    }
  }

  RetData* r = (RetData*)malloc(sizeof(RetData));
  *r = ret;
  return (void*) r;
}

// Thread de dica: Recebe um SudokuData com as informacoes
// de valores faltantes. Aplica Bitwise And nos valores
// correspondentes aquelas coordenadas. Costumava retornar
// erro caso encontrasse um vetor vazio; tiramos essa funcionalidade
// para poder reutilizar a thread na verificacao.

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
  param.g[y][x] = -ret;
  return NULL;
}

// Thread de verificacao para o resolvedor:
// Similar ao de verificacao de sudoku resolvido.
// Ao inves de verificar se ha algum valor faltando,
// verifica se ha algum valor repetido.

void* thread_check(void*v){
  VerifData data = *((VerifData*) v);
  int** grid = data.g;
  int id = data.id;
  int type = data.type;
  int check[9];

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

// FIM DAS THREADS

// sudokuChecker: A partir de uma grid, cria um SudokuData
// com as informacoes de valores faltantes em cada linha,
// coluna ou setor.

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
  // Itera sobre as threads, dando join e gravando suas informacoes
  // no SudokuData a ser retornado. Se algum dos vetores de
  // valores faltantes for diferente de zero, o erro sera 1;
  // poupa iteracao na funcao verificador().

  RetData *p_ret;
  for (i = 0; i<27;i++){
    pthread_join(thr[i],(void**)&p_ret);
    if (p_ret->missing != 0) ret.error = 1;
    (ret.data)[p_ret->type][p_ret->id]=p_ret->missing;
    free(p_ret);
  }
  return ret;
}

// Funcao de verificacao. Utiliza o SudokuData da funcao anterior
// para determinar se falta algum numero nas linhas e colunas.
// Se faltarem numeros, eles serao acusados.

int verificador(){
  printf("Insira sudoku completo para verificacao.\n");
  int x = 0, i, j, k;
  int** grid = malloc_grid();

  if(scan_sudoku(grid)){
    printf("Erro no input.\n");
    return 2;
  }
  // Gera os dados de valores faltantes.
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
  // Como ambos a grid e o SudokuData sao alocados dinamicamente,
  // eh preciso libera-los no fim da funcao.
  free_grid(grid);
  free_sudokuData(missing);
  return missing.error;
}


// Hint Generator: Dadas uma grid com valores faltando (Xs) e as
// informacoes do SudokuData, esta funcao substitui todos os Xs
// pelas possibilidades daquela celula, armazenados num
// vetor de bits (mesmo formato do SudokuData).
// Para diferenciar valores fixos dos possiveis, 
// estes ultimos sao armazenados negativamente.
// Como nem todas as 81 celulas estao vazias,
// criei uma lista ligada para armazenar as threads
// ao inves de uma matriz.

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

  // Aqui, dou join em todas as threads, uma por uma.

  while (pnodeStack){
    temp = popPNode(&pnodeStack);
    pthread_join(temp->data, &ret);
    free(temp);
  }
  return 0;
}


// Funcao de dica. Similar a de verificacao,
// simplesmente utiliza as funcoes anteriores
// e imprime a grid.

int dica(){
  int x = 0, i, j, erro = 0;
  int** grid = malloc_grid();
  SudokuData missing;
  printf("Insira sudoku incompleto para geração de dicas.\n");
  if (scan_sudoku(grid)){
    printf("Erro no input.\n");
    return 2;
  }
  missing = sudokuChecker(grid);
  
  if(hint_generator(grid, missing)){
    printf("Erro na verificação.\n");
    return 1;
  }

  print_sudoku(grid);
  free_grid(grid);
  free_sudokuData(missing);
  return 0;
}

// Funcao de resolvedor: Implementa uma solução, por backtracking,
// do sudoku, no qual a linha, coluna e setor sao checados em threads
// diferentes para cada recursao. Poderiamos ter utilizado
// os dados do Hint Generator para economizar iteracoes nulas,
// mas a diferenca de performance nao seria significativa
// para este contexto.

int resolvedor (){
  printf("Insira sudoku incompleto para resolução.\n");
  int x = 0, i, j, erro = 0;
  int** grid = malloc_grid();
  if (scan_sudoku(grid)){             // Obtem a matriz com os coeficientes do sudoku
    printf("Erro no input.\n");
    return 2;
  }
  // Chama funcao recursiva preenchedora
  if (backTracker(grid) == 0){
    printf("Sudoku insolúvel!\n");
  }

  print_sudoku(grid);// IMPRIMIR MATRIZ PREENCHIDA
  free_grid(grid);
  return 0;
}


int backTracker(int** grid){
  int i,j,k;
  pthread_t thr[3];
  VerifData data[3];
  // Inicia o vetor de dados
  for(i = 0; i < 3; i++){
    data[i].g = grid;
    data[i].type = i;
  }

  for(i = 0; i < 9; i++){ //Percorre grid em busca da palavra de dicas
    for(j = 0; j < 9; j++){
        if(grid[i][j] == 10){ //Caso o elemento tenha uma palavra de dicas
          data[0].id = i;
          data[1].id = j;
          data[2].id = j/3 + 3*(i/3);
          for(k = 1; k <= 9; k++){
              grid[i][j] = k;
              //Checa se viola linha, col ou setor
              pthread_create(&thr[0], NULL, thread_check, (void*)&(data[0]));
              pthread_create(&thr[1], NULL, thread_check, (void*)&(data[1]));
              pthread_create(&thr[2], NULL, thread_check, (void*)&(data[2]));

              int *p_ret;
              int total = 0;

              int l;
              for(l = 0; l < 3; l++){
                pthread_join(thr[l],(void**)&p_ret);
                total += *p_ret;
		//	free(p_ret);
              }
              if(total < 3){  //Caso uma das threads acuse erro
                continue;
              }
              else if(backTracker(grid) == 1){ //Se recursão suceder
                return 1;
              }
            }
          grid[i][j] = 10;
          return 0; //Caso possibilidades tenham se esgotado sem sucesso
        }
    }
  }

  return 1;
}

// Implementacao da interface.

int main (int argc, char* argv[]){
    char input = 0;
    while(input != 'x'){
      printf("Escolha uma opção:\n\nv - Verificar grid pronta\nd - Gerar dicas para sudoku incompleto\nr - Resolver sudoku incompleto\nx - Sair\n");
      scanf(" %c",&input);
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
      case 'x':
	break;
      default:
	printf("Comando nao reconhecido\n");
      }
    }
  return 0;
}
