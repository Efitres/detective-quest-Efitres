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

// (NIVEL AVENTUREIRO) - implementar o sistema de pistas coletadas durante a exploração da mansão. Para isso, ampliará o sistema
                      // anterior de árvore binária adicionando:
                      // Pistas associadas a cada cômodo da mansão.
                      // Uma árvore BST para armazenar e organizar as pistas conforme forem encontradas.
                      // O objetivo do programa é permitir que o detetive explore a mansão, colete pistas espalhadas pelos cômodos
                      // e visualize ao final todos os indícios organizados de acordo com o alfabeto.

// (NIVEL MESTRE) - 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================
// Protótipos
// ================================
typedef struct Sala Sala;
typedef struct PistaNode PistaNode;

Sala* criarSala(const char *nome, const char *pista);
void explorarSalasComPistas(Sala *atual, PistaNode **raizPistas);
void inserirPista(PistaNode **raiz, const char *pista);
void exibirPistas(PistaNode *raiz);
void liberarArvoreSalas(Sala *raiz);
void liberarArvorePistas(PistaNode *raiz);

// ================================
// Structs
// ================================
struct Sala {
    char nome[50];
    char pista[100];    // nova: pista associada ao cômodo
    Sala *esquerda;
    Sala *direita;
};

struct PistaNode {
    char pista[100];
    PistaNode *esquerda;
    PistaNode *direita;
};

// ================================
// Função principal
// ================================
int main() {
    // Montando o mapa da mansão com pistas
    Sala *hall = criarSala("Hall de Entrada", "Pegadas suspeitas no tapete");
    hall->esquerda = criarSala("Sala de Estar", "Um livro aberto com anotações estranhas");
    hall->direita = criarSala("Cozinha", "Uma faca com resquícios de sangue");

    hall->esquerda->esquerda = criarSala("Biblioteca", "Uma carta rasgada");
    hall->esquerda->direita = criarSala("Jardim de Inverno", "Um lenço com iniciais bordadas");

    hall->direita->esquerda = criarSala("Despensa", "Garrafas quebradas");
    hall->direita->direita = criarSala("Sala de Jantar", "Um prato caído no chão");

    printf("Bem-vindo ao DETECTIVE QUEST!\n");

    // Árvore de pistas coletadas
    PistaNode *pistasColetadas = NULL;

    // Inicia exploração
    explorarSalasComPistas(hall, &pistasColetadas);

    // Ao sair, exibe pistas coletadas
    printf("\nPistas coletadas durante a investigação:\n");
    exibirPistas(pistasColetadas);

    // Libera memória
    liberarArvoreSalas(hall);
    liberarArvorePistas(pistasColetadas);

    return 0;
}

// ================================
// Implementações
// ================================

// Cria uma sala dinamicamente, com pista opcional
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// Insere uma nova pista na BST
void inserirPista(PistaNode **raiz, const char *pista) {
    if (*raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        if (!novo) {
            printf("Erro de alocação de memória!\n");
            exit(1);
        }
        strcpy(novo->pista, pista);
        novo->esquerda = NULL;
        novo->direita = NULL;
        *raiz = novo;
    } else if (strcmp(pista, (*raiz)->pista) < 0) {
        inserirPista(&((*raiz)->esquerda), pista);
    } else if (strcmp(pista, (*raiz)->pista) > 0) {
        inserirPista(&((*raiz)->direita), pista);
    }
    // se for igual, não insere duplicada
}

// Exploração da mansão com coleta de pistas
void explorarSalasComPistas(Sala *atual, PistaNode **raizPistas) {
    char opcao;

    while (atual != NULL) {
        printf("Você está em: %s\n", atual->nome);

        // Coleta a pista da sala
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: %s\n", atual->pista);
            inserirPista(raizPistas, atual->pista);
        } else {
            printf("Nenhuma pista aqui.\n");
        }

        printf("\nEscolha um caminho:\n");
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
            printf("Você decidiu encerrar a exploração.\n");
            return;
        } else {
            printf("Opção inválida, tente novamente.\n");
        }

        printf("\n");
    }
}

// Exibe as pistas em ordem alfabética (in-order)
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

// Libera memória das salas
void liberarArvoreSalas(Sala *raiz) {
    if (raiz == NULL) return;
    liberarArvoreSalas(raiz->esquerda);
    liberarArvoreSalas(raiz->direita);
    free(raiz);
}

// Libera memória da árvore de pistas
void liberarArvorePistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    liberarArvorePistas(raiz->esquerda);
    liberarArvorePistas(raiz->direita);
    free(raiz);
}
