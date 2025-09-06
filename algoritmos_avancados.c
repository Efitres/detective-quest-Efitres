// ============================================================================
//         PROJETO DETECTIVE QUEST - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS: 
// (NIVEL NOVATO)  - implementará um programa em C que simula o mapa da mansão como uma árvore binária com nome para cada cômodo.
                  // A árvore é montada de modo automático em alocação dinâmica, e o jogador poderá explorar esse mapa até chegar
                  // a um cômodo que não tenha mais caminhos.

// (NIVEL AVENTUREIRO) -

// (NIVEL MESTRE) - 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================
// Protótipos
// ================================
typedef struct Sala Sala;
Sala* criarSala(const char *nome);
void explorarSalas(Sala *atual);
void liberarArvore(Sala *raiz);

// ================================
// Struct
// ================================
struct Sala {
    char nome[50];
    Sala *esquerda;
    Sala *direita;
};

// ================================
// Função principal
// ================================
int main() {
    // Montando o mapa da mansão
    Sala *hall = criarSala("Hall de Entrada");
    hall->esquerda = criarSala("Sala de Estar");
    hall->direita = criarSala("Cozinha");

    hall->esquerda->esquerda = criarSala("Biblioteca");
    hall->esquerda->direita = criarSala("Jardim de Inverno");

    hall->direita->esquerda = criarSala("Despensa");
    hall->direita->direita = criarSala("Sala de Jantar");

    printf("Bem-vindo ao DETECTIVE QUEST!\n");
    printf("Você está no %s.\n\n", hall->nome);

    // Inicia exploração
    explorarSalas(hall);

    // Liberar memória
    liberarArvore(hall);

    return 0;
}

// ================================
// Implementações
// ================================

// Cria uma sala dinamicamente
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// Exploração da mansão pelo jogador
void explorarSalas(Sala *atual) {
    char opcao;

    while (atual != NULL) {
        printf("Você está em: %s\n", atual->nome);

        // Caso seja uma folha
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Não há mais caminhos! Exploração encerrada.\n");
            return;
        }

        printf("Escolha um caminho:\n");
        if (atual->esquerda != NULL) printf(" (e) Ir para %s\n", atual->esquerda->nome);
        if (atual->direita != NULL) printf(" (d) Ir para %s\n", atual->direita->nome);
        printf(" (s) Sair da exploração\n");

        printf(">> ");
        scanf(" %c", &opcao);

        if (opcao == 'e' && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if (opcao == 'd' && atual->direita != NULL) {
            atual = atual->direita;
        } else if (opcao == 's') {
            printf("Você decidiu sair da mansão.\n");
            return;
        } else {
            printf("Opção inválida, tente novamente.\n");
        }

        printf("\n");
    }
}

// Libera toda a árvore
void liberarArvore(Sala *raiz) {
    if (raiz == NULL) return;
    liberarArvore(raiz->esquerda);
    liberarArvore(raiz->direita);
    free(raiz);
}
