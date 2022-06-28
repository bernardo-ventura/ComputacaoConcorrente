#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define NTHREADS 5

/* Variaveis globais */
int x = 0;
sem_t condt1, condt2, condt3; 
const char *frases[5] = {"Volte sempre!", "Fique a vontade", "Sente-se por favor", "Aceita um copo dagua?", "Seja bem-vindo!"};

/* Thread inicial do volte sempre*/
void *g1(void *arg)
{
    long int id = (long int)arg;
    printf("%s\n", frases[id]);
    for(int i = 0; i < 3; i++){
        sem_post(&condt1);
    }
    pthread_exit(NULL);
}

/* Threads do meio, onde a ordem não importa */
void *g2(void *arg)
{
    long int id = (long int)arg;
    sem_wait(&condt1);
    printf("%s\n", frases[id]);
    sem_wait(&condt3);
    x++;
    if(x == 3){
        sem_post(&condt2);
    }
    sem_post(&condt3);
    pthread_exit(NULL);
}

/* Thread de finalização */
void *g3(void *arg)
{
    long int id = (long int)arg;
    sem_wait(&condt2);
    printf("%s\n", frases[id]);
    
    pthread_exit(NULL);
}

/* Funcao principal */
int main()
{
    pthread_t *tid;
    tid = (pthread_t *)malloc(sizeof(pthread_t) * NTHREADS);
    sem_init(&condt1, 0, 0);
    sem_init(&condt2, 0, 0);
    sem_init(&condt3, 0, 1);

    for (long int i = 0;i<5;i++){
        if (i == 0){
            if (pthread_create(tid+i, NULL, g3, (void *)i))
                {
                    fprintf(stderr, "ERRO--pthread_create\n");
                    return 3;
                };
        }
        else if (i == 4){
            if (pthread_create(tid+i, NULL, g1, (void *)i))
            {
                fprintf(stderr, "ERRO--pthread_create\n");
                return 3;
            };
        }
        else{
            if (pthread_create(tid+i, NULL, g2, (void *)i))
            {
                fprintf(stderr, "ERRO--pthread_create\n");
                return 3;
            };
        }
    }

    /* Espera todas as threads completarem */
    for (int i = 0; i < NTHREADS; i++)
    {
        pthread_join(*(tid + i), NULL);
    }
    printf("fim\n");
    
    return 0;
}