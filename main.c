#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PROCESSOS 100

// Representacao de processo
typedef struct {
    int pid;
    int tempo_chegada;
    int tempo_restante;
    int tempo_execucao;
    int tempo_conclusao;
    int tempo_espera;
} processo;

// Funcao de comparacao para ordenar os processos pelo tempo de chegada (FIFO)
int compare_tempo_chegada(const void *a, const void *b) {
    return ((processo *)a)->tempo_chegada - ((processo *)b)->tempo_chegada;
}
// Funcao de comparacao para ordenar os processos pelo tempo de chegada (FIFO)
int compare_duracao(const void *a, const void *b) {
    return ((processo *)a)->tempo_execucao - ((processo *)b)->tempo_execucao;
}
// Funcao de comparacao para ordenar os processos pelo tempo de espera (melhorar print)
int compare_tempo_espera(const void *a, const void *b) {
    return ((processo *)a)->tempo_espera - ((processo *)b)->tempo_espera;
}

// Simulacao do escalonamento FIFO
void fifo(processo processos[], int n) {
    qsort(processos, n, sizeof(processo), compare_tempo_chegada);
    int tempo = 0; 
    for (int i = 0; i < n; i++) {
		processos[i].tempo_espera = tempo;
        processos[i].tempo_conclusao = tempo + processos[i].tempo_execucao;
        processos[i].tempo_restante -= processos[i].tempo_execucao;
        tempo += processos[i].tempo_execucao;
    }

    printf("\nRepresentação gráfica do escalonamento\n");
    // Nome dos processsos
    for (int i = 0; i < n; i++) printf("|   P%d  ", i+1);
    printf("|\n");
    // Linha do tempo
    printf("%02d", 0);
    for (int i = 0; i < n; i++) printf("------%02d", processos[i].tempo_conclusao);
    printf("\n");
}

// Simulacao do escalonamento SJF (Não preemptivo)
void sjf_nao_preemptivo(processo processos[], int n) {
    qsort(processos, n, sizeof(processo), compare_duracao);
    int tempo = 0; 
    int executados = 0;
    bool tempo_incrementado = false;
    while(executados < n){
		int i = 0;
		while (i < n) {                                              
			if(tempo >= processos[i].tempo_chegada && processos[i].tempo_restante > 0){               				                                
		        processos[i].tempo_espera = tempo - processos[i].tempo_chegada; // tempo atual está rodando então esperou até o penúltimo tempo
		        processos[i].tempo_conclusao = tempo + processos[i].tempo_execucao; // tempo de conclusao sera o tempo que comecou rodar mais o tempo de execucao
				tempo += processos[i].tempo_execucao;
				processos[i].tempo_restante = 0; // processo rodou então tempo restante zerado                                     
		        executados++;
		        tempo_incrementado = true;
		    }		                                                                     
			i++;                                                                       
		}
		if(!tempo_incrementado) tempo++;
		tempo_incrementado = false;
	}
	qsort(processos, n, sizeof(processo), compare_tempo_espera);

    printf("\nRepresentação gráfica do escalonamento\n");
    // Nome dos processsos
    for (int i = 0; i < n; i++) printf("|   P%d  ", i+1);
    printf("|\n");
    // Linha do tempo
    printf("%02d", 0);
    for (int i = 0; i < n; i++) printf("------%02d", processos[i].tempo_conclusao);
    printf("\n");
}                                                             

// Simulacao do escalonamento Round Robin
void round_robin(processo processos[], int n, int quantum) {
    int tempo = 0, restante = n;
    bool executado;

    // Construção da representação gráfica
    char nome_processos[150];
    char linha_tempo[150];
    int str_padding = 0;
    printf("\nRepresentação gráfica do escalonamento\n|");
    
    // Ordena os processos pelo tempo de chegada para garantir que sejam atendidos na ordem correta
    qsort(processos, n, sizeof(processo), compare_tempo_chegada);
    
    while (restante > 0) {
        executado = false;

        for (int i = 0; i < n; i++) {
            if (processos[i].tempo_restante > 0 && tempo >= processos[i].tempo_chegada) {
                executado = true;

                sprintf(&nome_processos[str_padding], " P%d |", i+1);
                // Se o tempo restante for menor ou igual ao quantum, finaliza o processo
                if (processos[i].tempo_restante <= quantum) {
                    tempo += processos[i].tempo_restante;
                    processos[i].tempo_restante = 0;
                    processos[i].tempo_conclusao = tempo;
                    processos[i].tempo_espera = tempo - processos[i].tempo_chegada - processos[i].tempo_execucao;
                    restante--;
                } else {
                    // Executa o quantum e reduz o tempo restante
                    tempo += quantum;
                    processos[i].tempo_restante -= quantum;
                }
                str_padding += sprintf(&linha_tempo[str_padding], "---%02d", tempo);
            }
        }

        // Se nenhum processo foi executado, avancamos no tempo ate o proximo processo chegar
        if (!executado) {
            tempo++;
        }
    }

    printf("%s\n00%s\n", nome_processos, linha_tempo, tempo);

    // Ordena os processos pelo tempo de chegada novamente para saida organizada
    qsort(processos, n, sizeof(processo), compare_tempo_chegada);
}


// Funcao para imprimir os resultados
void imprimir_resultados(processo processos[], int n) {
    printf("\n%-5s | %-14s | %-14s | %-10s | %-15s\n", "PID", "Tempo Chegada", "Tempo Execucao", "Conclusao", "Tempo de Espera");
    printf("----------------------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-5d | %-14d | %-14d | %-10d | %-15d\n", 
               processos[i].pid, processos[i].tempo_chegada, processos[i].tempo_execucao, 
               processos[i].tempo_conclusao, processos[i].tempo_espera);
    }
}

int main() {
    int n, escolha, quantum;
    processo processos[MAX_PROCESSOS];
    
    printf("Numero de processos: ");
    scanf("%d", &n);
    
    for (int i = 0; i < n; i++) {
        processos[i].pid = i + 1;
        printf("processo %d - Tempo de chegada: ", i + 1);
        scanf("%d", &processos[i].tempo_chegada);
        printf("processo %d - Tempo de execucao: ", i + 1);
        scanf("%d", &processos[i].tempo_execucao);
        processos[i].tempo_restante = processos[i].tempo_execucao;
    }
    
    printf("1. FIFO\n2. SJF não preemptivo\n3. Round Robin\nEscolha o algoritmo: ");
    scanf("%d", &escolha);
    
    switch (escolha) {
        case 1:
            fifo(processos, n);
            break;
        case 2:
            sjf_nao_preemptivo(processos, n);
            break;
        case 3:
            printf("Quantum para Round Robin: ");
            scanf("%d", &quantum);
            round_robin(processos, n, quantum);
            break;
        default:
            printf("Opcao invalida.\n");
            return 1;
    }
    
    imprimir_resultados(processos, n);
    return 0;
}
