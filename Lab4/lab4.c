#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "time.h"
#include <pthread.h>

int *vetEntrada;
float *vetSaidaConc;
float *vetSaidaSeq;
long long int dim;
int nthreads;
pthread_mutex_t lock;

//função ehPrimo
int ehPrimo(long long int n){
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2 == 0) return 0;
    for(int i = 3; i<sqrt(n) +1; i+=2){
        if(n%i==0) return 0;
    }
    return 1;
}

void processaPrimos(int vetEntrada[], float vetSaidaSeq[], int dim) {
    for(int i=0; i<dim; i++) {
        if (ehPrimo(vetEntrada[i])){
            vetSaidaSeq[i] = sqrt(vetEntrada[i]);
        }else{
            vetSaidaSeq[i] = vetEntrada[i];
        }
    }
}

void *tarefa(void *arg){
    pthread_mutex_lock(&lock);
    for(int i=0; i<dim; i++) {
        if (ehPrimo(vetEntrada[i])){
            vetSaidaConc[i] = sqrt(vetEntrada[i]);
        }else{
            vetSaidaConc[i] = vetEntrada[i];
        }
    }
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    pthread_t *tid;
    double inicioSeq, finalSeq, inicioConc, finalConc;

    if(argc<2){
        fprintf(stderr, "Digite: %s 'dimensão do vetor' 'número de threads", argv[0]);
    }
    dim = atoll(argv[1]);
    nthreads = atoi(argv[2]);

    // alocação de memória
    
    vetEntrada = (int*) malloc(sizeof(int)*dim);
    if (vetEntrada == NULL) {
        fprintf(stderr, "erro de malloc");
        return 2;
    }

    vetSaidaSeq = (float*) malloc(sizeof(float)*dim);
    if (vetSaidaSeq == NULL) {
        fprintf(stderr, "erro de malloc");
        return 2;
    }

    vetSaidaConc = (float*) malloc(sizeof(float)*dim);
    if (vetSaidaConc == NULL) {
        fprintf(stderr, "erro de malloc");
        return 2;
    }

     // preencher vetor
    srand(time(NULL));

    for (int i = 0; i<dim; i++){
        vetEntrada[i] = rand() % 100;
    }

    GET_TIME(inicioConc);
    //alocação dos identificadores das threads
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid==NULL){
        puts("Erro de malloc\n");
        return 2;
    }

    //inicializa a variável de exclusão mútua
    pthread_mutex_init(&lock,NULL);

    //criação de threads
    for(long long int i=0;i<nthreads;i++){
        if(pthread_create(tid + 1,NULL,tarefa,(void*)i)){
            printf("erro de criação de threads\n");
            exit(-1);
        }
    }

    //espera as threads terminarem
    for(long long int i=0; i<nthreads;i++){
        pthread_join(*(tid+i),NULL);
    }
    GET_TIME(finalConc);
    

    //função sequencial
    GET_TIME(inicioSeq);
    processaPrimos(vetEntrada,vetSaidaSeq,dim);
    GET_TIME(finalSeq);

    printf("Tempo sequencial: %lf\n",finalSeq-inicioSeq);
    printf("Tempo concorrente: %lf\n",finalConc-inicioConc);
    printf("Aceleração: %lf\n",(finalSeq-inicioSeq)/(finalConc-inicioConc));

    free(tid);
    free(vetEntrada);
    free(vetSaidaSeq);
    free(vetSaidaConc);
    pthread_mutex_destroy(&lock);
    return 0;
}