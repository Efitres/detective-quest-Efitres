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
#include <ctype.h>

// ================================
// Protótipos
// ================================
typedef struct Sala Sala;
typedef struct PistaNode PistaNode;
typedef struct HashNode HashNode;
typedef struct HashTable HashTable;

Sala* criarSala(const char *nome, const char *pista);
void explorarSalas(Sala *atual, PistaNode **raizPistas, HashTable *hash);
void inserirPista(PistaNode **raiz, const char *pista);
void exibirPistas(PistaNode *raiz);
void exibirPistasComSuspeito(PistaNode* raiz, HashTable* h);
void liberarArvoreSalas(Sala *raiz);
void liberarArvorePistas(PistaNode *raiz);

/* Hash (associa pista -> suspeito) */
HashTable* criarHash(int tamanho);
void inserirNaHash(HashTable *h, const char *pista, const char *suspeito);
const char* encontrarSuspeito(HashTable *h, const char *pista);
void liberarHash(HashTable *h);

/* Verificação final */
int contarPistasParaSuspeito(PistaNode *raiz, HashTable *h, const char *suspeito);
void verificarSuspeitoFinal(PistaNode *raiz, HashTable *h);

/* Utilitários */
static unsigned long djb2_hash(const char *str);
void limparEntrada();

// ================================
// Structs
// ================================
struct Sala {
    char nome[60];
    char pista[150];    // pista associada (pode ser string vazia)
    Sala *esquerda;
    Sala *direita;
};

struct PistaNode {
    char pista[150];
    PistaNode *esquerda;
    PistaNode *direita;
};

/* Lista encadeada para colisões na hash */
struct HashNode {
    char pista[150];
    char suspeito[60];
    HashNode *prox;
};

struct HashTable {
    int tamanho;
    HashNode **buckets;
};

// ================================
// Função principal, orquestrando a chamada de funções e permitindo o loop do jogo.
// ================================
int main() {
    /* Montagem fixa do mapa da mansão (árvore binária de salas),
       cada sala com uma pista estática associada. */
    Sala *hall = criarSala("Hall de Entrada", "Pegadas suspeitas no tapete");
    hall->esquerda = criarSala("Sala de Estar", "Livro aberto com anotações");
    hall->direita = criarSala("Cozinha", "Faca com resíduos");

    hall->esquerda->esquerda = criarSala("Biblioteca", "Carta rasgada encontrada");
    hall->esquerda->direita = criarSala("Jardim de Inverno", "Lenço com iniciais");
    hall->direita->esquerda = criarSala("Despensa", "Garrafas quebradas");
    hall->direita->direita = criarSala("Sala de Jantar", "Prato quebrado com tinta");

    /* Cria tabela hash e popula mapeamentos pista -> suspeito */
    HashTable *hash = criarHash(31); // 31 buckets (primo simples)

    inserirNaHash(hash, "Pegadas suspeitas no tapete", "Mr. Bigulu");
    inserirNaHash(hash, "Livro aberto com anotações", "Dona Méus");
    inserirNaHash(hash, "Faca com resíduos", "Senhor Viajante");
    inserirNaHash(hash, "Carta rasgada encontrada", "Senhor Cloviski");
    inserirNaHash(hash, "Lenço com iniciais", "Lady Rochele");
    inserirNaHash(hash, "Garrafas quebradas", "Senhor Bangue");
    inserirNaHash(hash, "Prato quebrado com tinta", "Senhora Valdete");

    printf("Bem-vindo ao DETECTIVE QUEST - NIVEL MESTRE\n\n");

    /* BST para armazenar pistas coletadas */
    PistaNode *pistasColetadas = NULL;

    /* Inicia a exploração interativa */
    explorarSalas(hall, &pistasColetadas, hash);

    /* Ao finalizar exploração, mostra pistas coletadas e conduz julgamento */
    printf("\n== Pistas coletadas (em ordem alfabética) ==\n");
    exibirPistas(pistasColetadas);

    printf("\n== Fase de Acusação ==\n");
    verificarSuspeitoFinal(pistasColetadas, hash);

    /* Limpeza de memória */
    liberarArvoreSalas(hall);
    liberarArvorePistas(pistasColetadas);
    liberarHash(hash);

    return 0;
}

// ================================
// Implementações das funções
// ================================

/*
 * criarSala()
 * Cria dinamicamente uma sala com nome e pista (pista pode ser string vazia).
 */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        fprintf(stderr, "Erro de alocação de memória!\n");
        exit(EXIT_FAILURE);
    }
    strncpy(nova->nome, nome, sizeof(nova->nome)-1);
    nova->nome[sizeof(nova->nome)-1] = '\0';
    if (pista != NULL) {
        strncpy(nova->pista, pista, sizeof(nova->pista)-1);
        nova->pista[sizeof(nova->pista)-1] = '\0';
    } else {
        nova->pista[0] = '\0';
    }
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

/*
 * inserirPista()
 * Insere uma pista na BST de pistas coletadas (evita duplicação literal).
 */
void inserirPista(PistaNode **raiz, const char *pista) {
    if (*raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        if (!novo) {
            fprintf(stderr, "Erro de alocação de memória!\n");
            exit(EXIT_FAILURE);
        }
        strncpy(novo->pista, pista, sizeof(novo->pista)-1);
        novo->pista[sizeof(novo->pista)-1] = '\0';
        novo->esquerda = novo->direita = NULL;
        *raiz = novo;
        return;
    }
    int cmp = strcmp(pista, (*raiz)->pista);
    if (cmp < 0) inserirPista(&((*raiz)->esquerda), pista);
    else if (cmp > 0) inserirPista(&((*raiz)->direita), pista);
    /* se igual, não insere (evita duplicatas) */
}

/*
 * exibirPistas()
 * Percorre a BST em ordem (in-order) e imprime as pistas.
 */
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

/*
 * explorarSalas()
 * Permite navegação do jogador pela árvore binária de salas.
 * Ao entrar em uma sala, mostra a pista (se houver) e a adiciona automaticamente à BST.
 *
 * Parâmetros:
 *  - atual: ponteiro para a sala atual
 *  - raizPistas: ponteiro para a raiz da BST de pistas coletadas
 *  - hash: tabela com mapeamento pista -> suspeito (apenas para referência)
 */
void explorarSalas(Sala *atual, PistaNode **raizPistas, HashTable *hash) {
    char opcao;
    while (atual != NULL) {
        printf("Você está em: %s\n", atual->nome);

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
        limparEntrada();

        if (opcao == 'e' && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if (opcao == 'd' && atual->direita != NULL) {
            atual = atual->direita;
        } else if (opcao == 's') {
            printf("Você decidiu encerrar a exploração.\n");
            return;
        } else {
            printf("Opção inválida ou caminho inexistente. Tente novamente.\n\n");
        }

        printf("\n");
    }
}

/* ---------- Hash table (pista -> suspeito) ---------- */

/* djb2 hash function */
static unsigned long djb2_hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

/*
 * criarHash()
 * Cria uma tabela hash com 'tamanho' buckets.
 */
HashTable* criarHash(int tamanho) {
    HashTable *h = (HashTable*) malloc(sizeof(HashTable));
    if (!h) {
        fprintf(stderr, "Erro de alocação de memória!\n");
        exit(EXIT_FAILURE);
    }
    h->tamanho = tamanho;
    h->buckets = (HashNode**) calloc(tamanho, sizeof(HashNode*));
    if (!h->buckets) {
        fprintf(stderr, "Erro de alocação de memória!\n");
        exit(EXIT_FAILURE);
    }
    return h;
}

/*
 * inserirNaHash()
 * Insere a associação pista -> suspeito na tabela hash.
 * Se a pista já existir, atualiza o suspeito (comportamento simples).
 */
void inserirNaHash(HashTable *h, const char *pista, const char *suspeito) {
    unsigned long code = djb2_hash(pista);
    int idx = code % h->tamanho;

    HashNode *node = h->buckets[idx];
    while (node) {
        if (strcmp(node->pista, pista) == 0) {
            /* atualiza suspeito se encontrado */
            strncpy(node->suspeito, suspeito, sizeof(node->suspeito)-1);
            node->suspeito[sizeof(node->suspeito)-1] = '\0';
            return;
        }
        node = node->prox;
    }

    /* não encontrado -> cria novo nó e insere no bucket (head insert) */
    HashNode *novo = (HashNode*) malloc(sizeof(HashNode));
    if (!novo) {
        fprintf(stderr, "Erro de alocação de memória!\n");
        exit(EXIT_FAILURE);
    }
    strncpy(novo->pista, pista, sizeof(novo->pista)-1);
    novo->pista[sizeof(novo->pista)-1] = '\0';
    strncpy(novo->suspeito, suspeito, sizeof(novo->suspeito)-1);
    novo->suspeito[sizeof(novo->suspeito)-1] = '\0';
    novo->prox = h->buckets[idx];
    h->buckets[idx] = novo;
}

/*
 * encontrarSuspeito()
 * Consulta o suspeito relacionado a uma pista na hash.
 * Retorna NULL se não houver associação.
 */
const char* encontrarSuspeito(HashTable *h, const char *pista) {
    unsigned long code = djb2_hash(pista);
    int idx = code % h->tamanho;

    HashNode *node = h->buckets[idx];
    while (node) {
        if (strcmp(node->pista, pista) == 0) return node->suspeito;
        node = node->prox;
    }
    return NULL;
}

/*
 * liberarHash()
 * Libera memória alocada para a hash.
 */
void liberarHash(HashTable *h) {
    if (!h) return;
    for (int i = 0; i < h->tamanho; ++i) {
        HashNode *n = h->buckets[i];
        while (n) {
            HashNode *tmp = n;
            n = n->prox;
            free(tmp);
        }
    }
    free(h->buckets);
    free(h);
}

/* ---------- Verificação final (julgamento) ---------- */

/*
 * contarPistasParaSuspeito()
 * Percorre a BST de pistas coletadas e conta quantas pistas apontam para 'suspeito'
 * usando a tabela hash (pista->suspeito).
 */
int contarPistasParaSuspeito(PistaNode *raiz, HashTable *h, const char *suspeito) {
    if (raiz == NULL) return 0;
    int count = 0;
    /* percorre esquerda */
    count += contarPistasParaSuspeito(raiz->esquerda, h, suspeito);

    /* verifica a pista atual */
    const char *s = encontrarSuspeito(h, raiz->pista);
    if (s != NULL && strcmp(s, suspeito) == 0) count++;

    /* percorre direita */
    count += contarPistasParaSuspeito(raiz->direita, h, suspeito);
    return count;
}

/*
 * verificarSuspeitoFinal()
 * Pergunta ao jogador quem ele acusa, conta quantas pistas coletadas apontam
 * para esse suspeito e decide se há evidência suficiente (>= 2 pistas).
 */
void verificarSuspeitoFinal(PistaNode *raiz, HashTable *h) {
    if (raiz == NULL) {
        printf("Nenhuma pista foi coletada. Não é possível realizar acusação.\n");
        return;
    }

    /* Para facilitar o jogador, mostramos os suspeitos conhecidos na hash. */
    printf("Suspeitos conhecidos:\n");
    /* coletar nomes únicos dos buckets (simples) */
    char suspeitosEncontrados[20][60];
    int scount = 0;
    for (int i = 0; i < h->tamanho; ++i) {
        HashNode *n = h->buckets[i];
        while (n) {
            int ja = 0;
            for (int j = 0; j < scount; ++j) {
                if (strcmp(suspeitosEncontrados[j], n->suspeito) == 0) { ja = 1; break; }
            }
            if (!ja && scount < 20) {
                strncpy(suspeitosEncontrados[scount], n->suspeito, sizeof(suspeitosEncontrados[scount])-1);
                suspeitosEncontrados[scount][sizeof(suspeitosEncontrados[scount])-1] = '\0';
                scount++;
            }
            n = n->prox;
        }
    }
    for (int i = 0; i < scount; ++i) printf(" - %s\n", suspeitosEncontrados[i]);

    char escolha[60];
    printf("\nQuem você acusa? (digite o nome exato do suspeito):\n>> ");
    if (fgets(escolha, sizeof(escolha), stdin) == NULL) {
        printf("Entrada inválida.\n");
        return;
    }
    /* remove newline */
    escolha[strcspn(escolha, "\n")] = '\0';

    if (strlen(escolha) == 0) {
        printf("Nenhum nome fornecido. Acusação cancelada.\n");
        return;
    }

    int contador = contarPistasParaSuspeito(raiz, h, escolha);
    printf("\nVocê acusou: %s\n", escolha);
    printf("Pistas que apontam para %s: %d\n", escolha, contador);

    if (contador >= 2) {
        printf("\nDESFECHO: Há evidências suficientes. %s é considerado(a) culpado(a).\n", escolha);
    } else {
        printf("\nDESFECHO: Evidências insuficientes para condenar %s. Caso não solucionado.\n", escolha);
    }

    // ---------------------------
    // Mostra todas as pistas coletadas e seus suspeitos
    // ---------------------------
    exibirPistasComSuspeito(raiz, h);

}


void exibirPistasComSuspeito(PistaNode* raiz, HashTable* h) {
    if (!raiz) return;
    exibirPistasComSuspeito(raiz->esquerda, h);
    const char* suspeito = encontrarSuspeito(h, raiz->pista);
    if (!suspeito) suspeito = "Desconhecido";
    printf(" - %s → Suspeito: %s\n", raiz->pista, suspeito);
    exibirPistasComSuspeito(raiz->direita, h);
    printf("\n\n");
}


/* ---------- Funções de liberação ---------- */

void liberarArvoreSalas(Sala *raiz) {
    if (raiz == NULL) return;
    liberarArvoreSalas(raiz->esquerda);
    liberarArvoreSalas(raiz->direita);
    free(raiz);
}

void liberarArvorePistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    liberarArvorePistas(raiz->esquerda);
    liberarArvorePistas(raiz->direita);
    free(raiz);
}

/* ---------- Utilitários ---------- */

/* Limpa restante da entrada até newline (útil após scanf de char) */
void limparEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
