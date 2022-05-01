#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"


float *m1;
float *m2;
int nthreads;
float *resultadoSeq;
float *resultadoConc;

typedef struct {
    int id;
    int dim;
}estrutura;

//função que as threads vão realizar
void *multiplicaMatriz(void *arg){
    estrutura *args = (estrutura*) arg;
    for (int i = args->id; i < args->dim; i += nthreads){
        for (int j = 0; j < args->dim; j++){
            for (int k = 0; k < args->dim; k++){
                resultadoConc[i * (args->dim) + j] += m1[i * (args->dim) + k] * m2[k * (args->dim) + j];
            }
        }
    }
    pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char* argv[]){
    int dim;
    pthread_t *tid;
    estrutura *args;
    double inicioSeq, fimSeq, inicioConc, fimConc, deltaSeq, deltaConc;
    
    //leitura e avaliação dos parâmetros de entrada
    if(argc<3){
        printf(" %s Digite a dimensão das matrizes e o número\n de threads respectivamente, conforme o exemplo X Y\n", argv[0]);
        return 1;
    }

    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    if (nthreads > dim){
        nthreads = dim;
    }

    //alocação de memória
    m1 = (float *) malloc(sizeof(float) * dim * dim);
    if(m1 == NULL) {printf("Erro de malloc\n"); return 2;}

    m2 = (float *) malloc(sizeof(float) * dim * dim);
    if(m2 == NULL) {printf("Erro de malloc\n"); return 2;}

    resultadoSeq = (float *) malloc(sizeof(float) * dim * dim);
    if(resultadoSeq == NULL) {printf("Erro de malloc\n"); return 2;}

    resultadoConc = (float *) malloc(sizeof(float) * dim * dim);
    if(resultadoConc == NULL) {printf("Erro de malloc\n"); return 2;}


    //inicialização das estruturas de dados
    srand(time(NULL));

    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            m1[i*dim + j] = rand() % 100;
            m2[i*dim + j] = rand() % 100;
        }
    }

    GET_TIME(inicioSeq);
    //multiplicação das matrizes
    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            for(int k=0; k<dim; k++){
                resultadoSeq[i*dim + j] += m1[i*dim + k] * m2[k*dim + j];
            }
        }
    }
    GET_TIME(fimSeq);
    deltaSeq = fimSeq - inicioSeq;
    printf("Tempo sequencial: %lf\n",deltaSeq);

    //alocação das estruturas
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid==NULL){
        puts("Erro de malloc\n");
        return 2;
    }

    args = (estrutura *) malloc(sizeof(estrutura) * nthreads);
    if(args==NULL){
        puts("Erro de malloc\n");
        return 2;
    }

    GET_TIME(inicioConc);
    //criação das threads
    for(int i=0; i<nthreads;i++){
        (args+i)->id = i;
        (args+i)->dim = dim;
        if(pthread_create(tid+1, NULL, multiplicaMatriz, (void*) (args+i))){
            puts("Erro de criação\n");
            return 3;
        }
    }

    //espera pelo término das threads
    for(int i=0; i<nthreads; i++) {
        pthread_join(*(tid+i),NULL);
    }
    GET_TIME(fimConc);
    deltaConc = fimConc - inicioConc;
    printf("Tempo concorrente: %lf\n",deltaConc);

    //Razão
    printf("Razão do tempo sequencial/tempo concorrente: %lf\n", deltaSeq / deltaConc);

    //Validação do produto de matrizes
    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
            if(resultadoSeq[i*dim + j] != resultadoConc[i*dim+j]){
                printf("matrizes não batem\n");
                return 0;
            }
        }
    }
    printf("Sucesso, resultados do produto de matrizes são iguais\n");

    //liberação da memória
    free(m1);
    free(m2);
    free(resultadoSeq);
    free(resultadoConc);
    free(args);
    free(tid);


    return 0;
}