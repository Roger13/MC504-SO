#include <stdio.h>
#include <stdlib.h>


/* Existirão: thread passageiro e thread onibus
              mutex do ponto e mutex do embarque

   Funcionamento: threads passageiro pegam o mutex para adentrar o ponto de onibus
                  onibus espera mutex para adentrar o ponto e libera o mutex de embarque
                  quando numero de passageiros == 50 ou numero de threads no ponto == 0, onibus parte
*/


int main()
{
    printf("Hello world!\n");
    return 0;
}
