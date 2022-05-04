#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "time.h"

long long int dim; //dimensão do vetor (N)
int nthreads; //número de threads
float *vetor; //vetor com dimensão dim
float *vetorLocal;
//fluxo das threads

void * acharMinMax(void *arg) {
    long int id = (long int) arg;
    long int tamBloco = dim/nthreads;
    long int init = id * tamBloco;
    long int fim;
    

    if(id == nthreads-1){
        fim = dim;
    }else{
        fim = init + tamBloco;
    }
    float minLocal = RAND_MAX;
    float maxLocal = 0;

    for(long long int j=0;j<nthreads*2;j+=2){
        for(long long int i=init;i<fim;i++){
            if(vetor[i]<minLocal){
                minLocal = vetor[i];
                
            }
            if(vetor[i]>maxLocal){
                maxLocal = vetor[i];
                
            }
        }
    vetorLocal[id*2] = minLocal; //coloca o valor minimo em um vetor na posição 0
    vetorLocal[id*2+1] = maxLocal; //coloca o valor máximo em um vetor na posição 1
    }
    pthread_exit(NULL);
    }

//fluxo principal
int main(int argc, char *argv[]){
    double inicioSeq, finalSeq, inicioConc, finalConc;
    pthread_t *tid;
    float minSeq = RAND_MAX;
    float maxSeq = 0;
    float minConc = RAND_MAX;
    float maxConc = 0;

    printf("\n");
    //recebe e valida os parametros de entrada (dim e nthreads)
    if(argc<3){
        fprintf(stderr, "Digite: %s 'dimensão do vetor' 'número de threads'\n", argv[0]);
        return 1;
    }
    dim = atoll(argv[1]);
    nthreads = atoi(argv[2]);

    //aloca os vetores
    vetor = (float*) malloc(sizeof(float)*dim);
    if (vetor == NULL) {
        fprintf(stderr, "erro de malloc");
        return 2;
    }

    vetorLocal = (float*) malloc(sizeof(float)*(nthreads*2));
    if (vetor == NULL) {
        fprintf(stderr, "erro de malloc");
        return 2;
    }

    //preenche o vetor de entrada com números aleatórios
    srand(time(NULL));

    float randMax = 1000000000;

    for (long long int i = 0;i<dim;i++){
        float res = rand() * (randMax / RAND_MAX); //obtém números inteiros pela função rand() e depois os transforma em floats
        vetor[i] = res;
    }

    //função sequencial para achar o valor minimo e maximo
    
    GET_TIME(inicioSeq);
    for(long long int i=0;i<dim;i++){
        if(vetor[i]<minSeq){
            minSeq = vetor[i];
        }
        if(vetor[i]>maxSeq){
            maxSeq = vetor[i];
        }
    }
    GET_TIME(finalSeq);

    GET_TIME(inicioConc);
    //alocação de memória dos identificadores das threads
    tid = (pthread_t *) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL){
        fprintf(stderr, "erro de malloc");
        return 2;
    }
    //criar as threads
    for(long int i=0;i<nthreads;i++){
        if(pthread_create(tid+i,NULL,acharMinMax, (void*)i)){
            fprintf(stderr, "erro de criação de thread");
            return 3;
        }
    }

    //aguardar o termino das threads
    for(long int i=0;i<nthreads;i++){
        pthread_join(*(tid+i),NULL);
    }

    //comparar os valores minimos e maximos achados pelas threads e de fato tirar os valores minimos e maximos
    for(long long int i=0;i<nthreads*2;i++){
        if(vetorLocal[i]<minConc){
            minConc = vetorLocal[i];
        }
        if(vetorLocal[i]>maxConc){
            maxConc = vetorLocal[i];
        }
    }

    GET_TIME(finalConc);

    //exibir os resultados
    // for (long long i = 0; i<dim; i++){
    //     printf("%f\n", vetor[i]);
    // }
    // printf("\n------------------------------\n");
    // for (long long i = 0; i<nthreads*2; i++){
    //     printf("%f\n", vetorLocal[i]);
    // }

    printf("Valor mínimo sequencial: %f\n",minSeq);
    printf("Valor máximo sequencial: %f\n",maxSeq);
    printf("Valor mínimo concorrente: %f\n",minConc);
    printf("Valor máximo concorrente: %f\n",maxConc);
    printf("Tempo sequencial: %lf\n",finalSeq-inicioSeq);
    printf("Tempo concorrente: %lf\n",finalConc-inicioConc);
    printf("Aceleração: %lf\n",(finalSeq-inicioSeq)/(finalConc-inicioConc));

    //liberar memoria
    free(vetor);

    return 0;
}
