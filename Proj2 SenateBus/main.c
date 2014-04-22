/*  The Senate Bus Problem
 *  (Rogério de O. Bernardo - 140922 / Pedro Ciambra - 137268)
 *  -Passageiros se aglomeram no ponto
 *  -Quando onibus chega, passageiros do ponto começam a embarcar
 *  -Caso o onibus seguinte chegue antes do término do embarque, os passageiros podem obtar por qualquer onibus
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

  pthread_mutex_lock(&mutex_fila);
  numFila++;
  pthread_mutex_unlock(&mutex_fila);

  pthread_mutex_lock(&mutex_catraca);

  pthread_mutex_lock(&mutex_fila);
  numFila--;
  pthread_mutex_unlock(&mutex_fila);

  generateIcon(icon);
  moveTo(2,2);
  printf("Passageiro %s %d entrou no ponto de onibus( %d / %d ).\n",icon,thr_id, numPonto, numPassageiros);
  
  for (local = 0; local<NUM_PASSAGEIROS; local++){
    if (localNoPonto[local]==-1){
      localNoPonto[local]=thr_id;
      break;
    }
  }

  moveToSpot(local,icon);

  ++numPonto; // Incrementa número de passageiros no ponto

  pthread_mutex_unlock(&mutex_catraca);

  pthread_mutex_lock(&mutex_onibus);

  do{
    pthread_cond_wait(&condBusChegou, &mutex_onibus); // Espera onibus
  }while(numOnibus == 0); // Enquanto não houverem onibus

  moveToBus(local,icon);
  ++numPassageiros; // Incrementa número de passageiros no onibus
  --numPonto; // Decrementa número de passageiros no ponto

  logBegin();
  printf("%s %d embarcou no onibus( %d / %d )\n",icon, thr_id, numPonto, numPassageiros);
  logEnd();

  if(numPonto <= 0 || numPassageiros >= MAX_PASSAGEIROS){ // Caso onibus encheu ou não tem mais passageiros para embarcar
    pthread_cond_broadcast(&condFimEmbarque); // Avisa que onibus pode partir
    numOnibus--;
  }
  pthread_mutex_unlock(&mutex_onibus);
}

void* onibus(void *v) {
  pthread_mutex_lock(&mutex_catraca);
  pthread_mutex_lock(&mutex_onibus);
  ++numOnibus;
  numPassageiros = 0;
  logBegin();
  printf("Onibus chegou ao ponto ( %d / %d )\n", numPonto,numPassageiros);
  logEnd();

  busArrive();

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
    printf("Onibus partindo ( %d / %d )", numPonto, numPassageiros);
    logEnd();

    numPassageiros = 0; // Zera número de passageiros para o próximo onibus
  }
  busLeave();
  pthread_mutex_unlock(&mutex_onibus);
  pthread_mutex_unlock(&mutex_catraca);
}

int main()
{
  pthread_t passageiros[NUM_PASSAGEIROS]; // Vetor de threads passageiro
  pthread_t busoes[NUM_ONIBUS]; // Vetor de onibus

  int i, id[NUM_PASSAGEIROS];

  init();
  logBegin();
  printf("SenateBus!\n");

  for (i = 0; i < NUM_PASSAGEIROS; i++)
    localNoPonto[i]=-1;

  //Cria passageiros e onibus
  for (i = 0; i < NUM_PASSAGEIROS; i++) {
    id[i] = i;
    pthread_create(&passageiros[i], NULL, passageiro, &id[i]);
  }
  for (i = 0; i < NUM_ONIBUS; i++) {
    pthread_create(&busoes[i], NULL, onibus, NULL);
  }

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
