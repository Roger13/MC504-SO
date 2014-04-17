/*  The Senate Bus Problem
 *  (Rogério de O. Bernardo - 140922 / Pedro Ciambra - RA)
 *  -Passageiros se aglomeram no ponto
 *  -Quando onibus chega, passageiros do ponto começam a embarcar
 *  -Caso o onibus seguinte chegue antes do término do embarque, os passageiros podem obtar por qualquer onibus
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define P 60 // P deve ser maior ou igual a O, devido ao laço de criação das threads.
#define O 2

volatile int numPonto = 0; // Número de pessoas no ponto
volatile int numOnibus = 0; // Número de onibus no ponto
volatile int numEmbarque = 0; // Número de pessoas para embarcar no onibus
volatile int numPassageiros = 0; // Número de pessoas no onibus

pthread_cond_t condBusChegou;
pthread_cond_t condFimEmbarque;
pthread_mutex_t mutex;


void* passageiro(void *v) {
    int thr_id = *(int *)v;

    pthread_mutex_lock(&mutex);

    ++numPonto; // Incrementa número de passageiros no ponto
    printf("Passageiro %d adentrou o ponto de onibus(numPonto: %d / numPassageiros: %d).\n",thr_id, numPonto, numPassageiros);

    do{
        pthread_cond_wait(&condBusChegou, &mutex); // Espera onibus
    }while(numOnibus == 0); // Enquanto não houverem onibus

    //"BoardBus"
    ++numPassageiros; // Incrementa número de passageiros no onibus
    --numPonto; // Decrementa número de passageiros no ponto
    --numEmbarque; // Decrementa número de passageiros que devem embarcar
    printf("Passageiro %d embarcou no onibus(numPonto: %d / numPassageiros: %d).\n",thr_id, numPonto, numPassageiros);

    if(numEmbarque == 0 || numPassageiros == 50){ // Caso onibus encheu ou não tem mais passageiros para embarcar
        pthread_cond_broadcast(&condFimEmbarque); // Avisa que onibus pode partir
        numOnibus = 0;
    }
    pthread_mutex_unlock(&mutex);
}

void* onibus(void *v) {
    pthread_mutex_lock(&mutex);

    ++numOnibus;
    printf("Onibus chegou no ponto(numPonto: %d / numPassageiros: %d)!\n", numPonto,numPassageiros);

    if(numPonto == 0){ // Caso onibus esteja vazio
        printf("Ponto vaziu, onibus partindo...\n");
        pthread_mutex_unlock(&mutex);
        return;
    }

    pthread_cond_broadcast(&condBusChegou); // Avisa aos passageiros do ponto que podem embarcar
    numEmbarque = numPonto;
    pthread_cond_wait(&condFimEmbarque, &mutex); // Espera o fim do embarque

    //"Depart"
    printf("Fim do embarque, onibus partindo(numPonto: %d / numPassageiros: %d)!\n", numPonto, numPassageiros);
    numPassageiros = 0; // Zera número de passageiros para o próximo onibus

    pthread_mutex_unlock(&mutex);
}

int main()
{
    pthread_t passageiros[P]; // Vetor de threads passageiro
    pthread_t busoes[O]; // Vetor de onibus

    int i, id[P];

    printf("SenateBus!\n");

    //Cria passageiros e onibus
    for (i = 0; i < P; i++) {
        id[i] = i;
        pthread_create(&passageiros[i], NULL, passageiro, &id[i]);

        if(i < O - 1){
            pthread_create(&busoes[i], NULL, onibus, &id[P-1]);
        }
    }

    pthread_create(&busoes[O-1], NULL, onibus, &id[0]);

    //Espera todos os onibus
    for (i = 0; i < O; i++) {
        pthread_join(busoes[i], NULL);
    }

    return 0;
}
