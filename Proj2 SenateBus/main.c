#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define P 0
#define O 2


volatile int numPonto = 0;
volatile int numOnibus = 0;
volatile int numPassageiros = 0;

pthread_cond_t condBusChegou;
pthread_cond_t condFimEmbarque;
pthread_mutex_t mutex;


void* passageiro(void *v) {
    int thr_id = *(int *)v;

    pthread_mutex_lock(&mutex);
    ++numPonto; // Incrementa número de passageiros no ponto
    printf("Passageiro %d adentrou o ponto de onibus(numPonto: %d; numPassageiros: %d).\n",thr_id, numPonto, numPassageiros);

    do{
        pthread_cond_wait(&condBusChegou, &mutex); // Espera onibus
    }while(numOnibus == 0); // Enquanto não houverem onibus

    ++numPassageiros; // Incrementa número de passageiros no onibus
    --numPonto; // Decrementa número de passageiros no ponto
    printf("Passageiro %d embarcou no onibus(numPonto: %d; numPassageiros: %d).\n",thr_id, numPonto, numPassageiros);

    if(numPonto == 0 || numPassageiros == 50){ // Caso onibus encheu ou não tem mais passageiros para embarcar
        pthread_cond_signal(&condFimEmbarque); // Avisa que onibus pode partir
        numOnibus = 0;
    }
    pthread_mutex_unlock(&mutex);
}

void* onibus(void *v) {
    int thr_id = *(int *)v;

    pthread_mutex_lock(&mutex);
    printf("Onibus %d chegou no ponto(numPonto: %d; numPassageiros: %d)!\n",thr_id, numPonto,numPassageiros);

    if(numPonto == 0){ // Caso onibus esteja vazio
        printf("Ponto vaziu, onibus %d partindo...\n",thr_id);
        pthread_mutex_unlock(&mutex);
        return;
    }

    pthread_cond_broadcast(&condBusChegou); // Avisa aos passageiros do ponto que podem embarcar

    pthread_cond_wait(&condFimEmbarque, &mutex); // Espera o fim do embarque
    printf("Fim do embarque, onibus %d partindo(numPonto: %d; numPassageiros: %d)!\n",thr_id, numPonto, numPassageiros);

    numPassageiros = 0; // Zera número de passageiros para o próximo onibus

    pthread_mutex_unlock(&mutex);
}

int main()
{
    pthread_t passageiros[P];
    pthread_t busoes[O];

    int i, id[P + O];

    printf("SenateBus!\n");

    for (i = 0; i < P; i++) {
        id[i] = i;
        pthread_create(&passageiros[i], NULL, passageiro, &id[i]);
    }

    sleep(1);
    pthread_create(&busoes[0], NULL, onibus, &id[0]);

    return 0;
}
