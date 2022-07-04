#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_PASSENGERS 25
#define MAX_RIDES 3

/* Variaveis */
pthread_mutex_t embarque_lock; // mutex para controlar o acesso da variável 'embarcardos'
pthread_mutex_t passeio_lock; // mutex para controlar o acesso da variável 'desembarcados'


sem_t fila_embarque; // semáforo para garantir o embarque dos C passageiros
sem_t todos_embarcados; // semáforo binário para dizer as threads de passageiros para esperar pelo próximo passeio
sem_t fila_desembarque; // semáforo para garantir o desembarque dos C passageiros
sem_t todos_desembarcados; // semáforo binário para sinalizar as threads de passageiros para embarque 

volatile int embarcados = 0; // número atual de passageiros embarcados
volatile int desembarcados = 0; // número atual de passageiros desembarcados
volatile int passeio_atual = 0; // número atual de passeios
volatile int total_passeios; // número total de passeios
volatile int passageiros; // número total de threads de passageiros
volatile int capacidade; // capacidade do carrinho

/* Funções da montanha-russa */
void load(){
	printf("Passeio #%d vai começar!\n", passeio_atual+1);
	printf("A capacidade desse carrinho é %d\n", capacidade);
	sleep(2);
}
void run(){
	printf("O carrinho está cheio, hora do passeio!\n");
	sleep(2);
	printf("O carrinho está realizando o passeio!\n");
	sleep(2);
}
void unload(){
	printf("O passeio acabou!\n");
	sleep(2);
}
void board(){
	printf("%d passageiros embarcaram no carrinho...\n", embarcados);
	sleep(rand()%2);
}
void unboard(){
	printf("%d passageiros desembarcaram no carrinho...\n", desembarcados);
	sleep(rand()%2);
}

/* Threads */
void* carrinhoThread(){
	int i;
	// Roda até chegar no número total de passeios
	while(passeio_atual < total_passeios){
		load();
		
		for(i = 0; i < capacidade; i++) sem_post(&fila_embarque); // Sinaliza C threads de passageiros para embarcarem no carrinho
		sem_wait(&todos_embarcados); // Espera todos os passageiros embarcarem
		
		run();
		unload();
		
		for(i = 0; i < capacidade; i++) sem_post(&fila_desembarque); // Sinaliza para desembarcar os passageiros
		sem_wait(&todos_desembarcados); // Espera todos os passageiros desembarcarem
		printf("O carrinho está vazio novamente!\n\n");
		
		passeio_atual++;
	}
	return NULL;
}

void* passengerThread(){
	// Roda indefinidamente, as threads serão destruídas quando a execução principal acabar
	while(1){
		sem_wait(&fila_embarque); // Espera pela thread do carrinhho sinalizar para embarcar
		
		pthread_mutex_lock(&embarque_lock); // Bloqueia o acesso para o incremento de variáveis compartilhadas
		embarcados++;
		board();
		if (embarcados == capacidade){
			sem_post(&todos_embarcados); // Se esse for o último passageiro, libera o carrinho para o passeio
			embarcados = 0;
		}
		pthread_mutex_unlock(&embarque_lock); // Desbloqueia o acesso das variáveis compartilhadas

		sem_wait(&fila_desembarque); // Espera pelo fim do passeio
	
		pthread_mutex_lock(&passeio_lock); // Bloqueia o acesso para o incremento de variáveis compartilhadas
		desembarcados++;
		unboard();
		if (desembarcados == capacidade){
			sem_post(&todos_desembarcados); // Se esse for o último passageiro a desembarcar, sinaliza para que o embarque possa começar novamente.
			desembarcados = 0;
		}
		pthread_mutex_unlock(&passeio_lock); // Desbloqueia o acesso das variáveis compartilhadas
	}
    	return NULL;
}

/* Fluxo principal */
int main() {
	// Define os dados de entrada
	srand(time(NULL));
	passageiros =12;
	capacidade = 10;
	total_passeios = MAX_RIDES;

	pthread_t passageiro[passageiros];
	pthread_t carrinho;
	int i;
	
	// Criação dos mutexes e dos semáforos
	pthread_mutex_init(&embarque_lock, NULL);
	pthread_mutex_init(&passeio_lock, NULL);
	sem_init(&fila_embarque, 0, 0);
	sem_init(&todos_embarcados, 0, 0);
	sem_init(&fila_desembarque, 0, 0);
	sem_init(&todos_desembarcados, 0, 0);

	printf("A montanha-russa vai fazer %d passeios!\n", total_passeios);
	printf("Têm %d passageiros esperando na fila da montanha-russa!\n\n", passageiros);
	
	// Criação de threads e inicia a montanha-russa
	pthread_create(&carrinho, NULL, carrinhoThread, NULL);
	for(i = 0; i < passageiros; i++) pthread_create(&passageiro[i], NULL, passengerThread, NULL);
	// Join da thread carrinho quando todos os passeios terminarem
	pthread_join(carrinho, NULL);
	
	printf("Acabaram os passeios por hoje.\n");

	// Destruição dos mutexes e dos semáforos
	pthread_mutex_destroy(&embarque_lock);
	pthread_mutex_destroy(&passeio_lock);
	sem_destroy(&fila_embarque);
	sem_destroy(&todos_embarcados);
	sem_destroy(&fila_desembarque);
	sem_destroy(&todos_desembarcados);

	return 0;
}