/*  The Senate Bus Problem
 *  (Rogério de O. Bernardo - 140922 / Pedro Ciambra - 137268)
 *  -Passageiros se aglomeram no ponto
 *  -Quando onibus chega, passageiros do ponto começam a embarcar
 *  -Passageiros não podem embarcar em um ônibus que chegou antes deles.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "render.h"

#define MAX_PASSAGEIROS 50
#define NUM_PASSAGEIROS 60 // Deve ser maior ou igual a NUM_ONIBUS, devido ao laço de criação das threads.
#define NUM_ONIBUS 2

volatile int numFila = 0; // Número de pessoas na fila para entrar no ponto
volatile int numPonto = 0; // Número de pessoas no ponto
volatile int numOnibus = 0; // Número de onibus no ponto
volatile int numPassageiros = 0; // Número de pessoas no onibus
volatile int localNoPonto[NUM_PASSAGEIROS]; // Apenas para animacao

pthread_cond_t condBusChegou;
pthread_cond_t condFimEmbarque;
pthread_mutex_t mutex_onibus, mutex_catraca, mutex_fila;

void* passageiro(void *v) {
  char icon[20];
  int local;
  int thr_id = *(int *)v;
  //  printf("Passageiro %d chegou ao ponto de onibus(numPonto: %d / numPassageiros: %d).\n",thr_id, numPonto, numPassageiros);

  // Contador da fila. Possui um mutex próprio.

  pthread_mutex_lock(&mutex_fila);
  numFila++;
  printFila(numFila);
  pthread_mutex_unlock(&mutex_fila);

  // Mutex para entrar no ponto. É proibido entrar enquanto houver um ônibus no ponto.

  pthread_mutex_lock(&mutex_catraca);

  // Uma vez dentro do ponto, deduzir do contador da fila.
  pthread_mutex_lock(&mutex_fila);
  numFila--;
  printFila(numFila);
  pthread_mutex_unlock(&mutex_fila);

  // Gerar um ícone para a animação.
  generateIcon(icon);
  logBegin();
  printf("Passageiro %s %d entrou no ponto de onibus",icon,thr_id);
  logEnd();

  // Encontrar um espaço livre no ponto de ônibus. O limite é puramente gráfico.
  for (local = 0; local<NUM_PASSAGEIROS; local++){
    if (localNoPonto[local]==-1){
      localNoPonto[local]=thr_id;
      break;
    }
  }

  moveToSpot(local,icon);   // Animação

  ++numPonto; // Incrementa número de passageiros no ponto

  // Imprime o numero que acabou de ser alterado.
  printPonto(numPonto);
 
  // Permite que outro passageiro entre.
  pthread_mutex_unlock(&mutex_catraca);

  pthread_mutex_lock(&mutex_onibus);

  do{
    pthread_cond_wait(&condBusChegou, &mutex_onibus); // Espera onibus
  }while(numOnibus == 0); // Enquanto não houverem onibus

  localNoPonto[local]=-1;
  moveToBus(local,icon);
  ++numPassageiros; // Incrementa número de passageiros no onibus
  --numPonto; // Decrementa número de passageiros no ponto

  printOnibus(numPassageiros);
  printPonto(numPonto);

  logBegin();
  printf("%s %d embarcou no onibus",icon,thr_id);
  logEnd();

  if(numPonto <= 0 || numPassageiros >= MAX_PASSAGEIROS){ // Caso onibus encheu ou não tem mais passageiros para embarcar
    pthread_cond_broadcast(&condFimEmbarque); // Avisa que onibus pode partir
    numOnibus--;
  }
  printOnibus(numPassageiros);
  printPonto(numPonto);
  printFrame();

  pthread_mutex_unlock(&mutex_onibus);
}

void* onibus(void *v) {
  pthread_mutex_lock(&mutex_catraca); // Enquanto houver um onibus, ninguem entra no ponto.
  pthread_mutex_lock(&mutex_onibus);
  ++numOnibus;
  numPassageiros = 0;
  logBegin();
  printf("Onibus chegou ao ponto");
  logEnd();
  printOnibus(numPassageiros);
  printPonto(numPonto);

  busArrive(); // Animacao

  if(numPonto == 0){ // Caso ponto esteja vazio
    logBegin();
    printf("Ponto vazio, onibus partindo...");
    logEnd();
  }
  else{
    pthread_cond_broadcast(&condBusChegou); // Avisa aos passageiros do ponto que podem embarcar
    pthread_cond_wait(&condFimEmbarque, &mutex_onibus); // Espera o fim do embarque

    //"Depart"
    logBegin();
    printf("Onibus partindo");
    logEnd();

    numPassageiros = 0; // Zera número de passageiros para o próximo onibus
  }
  printOnibus(numPassageiros);
  printPonto(numPonto);

  busLeave();// Animação
  pthread_mutex_unlock(&mutex_onibus);
  pthread_mutex_unlock(&mutex_catraca);
}

int main()
{
  pthread_t passageiros[NUM_PASSAGEIROS]; // Vetor de threads passageiro
  pthread_t busoes[NUM_ONIBUS]; // Vetor de onibus

  int i, id[NUM_PASSAGEIROS];

  init();
  printOnibus(numPassageiros);
  printPonto(numPonto);

  logBegin();
  printf("SenateBus!\n");

  for (i = 0; i < NUM_PASSAGEIROS; i++) localNoPonto[i]=-1; // Inicializando espaços livres no ponto (animação apenas)

  //Cria passageiros e onibus. A ordem indica qual será o caso encontrado na barreira: ponto vazio, ponto cheio ou um meio-termo.

  for (i = 0; i < NUM_PASSAGEIROS; i++) {
    id[i] = i;
    pthread_create(&passageiros[i], NULL, passageiro, &id[i]);
  }
  for (i = 0; i < NUM_ONIBUS; i++) {
    pthread_create(&busoes[i], NULL, onibus, NULL);
  }/*
  for (i = NUM_PASSAGEIROS/2; i < NUM_PASSAGEIROS; i++) {
    id[i] = i;
    pthread_create(&passageiros[i], NULL, passageiro, &id[i]);
    }*/
  //Espera todos os onibus
  for (i = 0; i < NUM_ONIBUS; i++) {
    pthread_join(busoes[i], NULL);
  }
  for (i = 0; i < NUM_PASSAGEIROS; i++) {
    pthread_join(passageiros[i], NULL);
  }

  end();
  return 0;
}
