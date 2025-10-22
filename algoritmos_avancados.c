#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ============================================
// ESTRUTURAS DE DADOS
// ============================================

// Estrutura para a árvore binária da mansão
typedef struct NoSala {
    char nome[50];
    char pista[50];  // Pista encontrada na sala (se houver)
    struct NoSala* esquerda;
    struct NoSala* direita;
} NoSala;

// Estrutura para a BST de pistas
typedef struct NoPista {
    char pista[50];
    struct NoPista* esquerda;
    struct NoPista* direita;
} NoPista;

// Estrutura para a tabela hash (relaciona pista -> suspeito)
typedef struct NoHash {
    char pista[50];
    char suspeito[50];
    struct NoHash* proximo;  // Para tratamento de colisões
} NoHash;

#define TAMANHO_HASH 10

typedef struct {
    NoHash* tabela[TAMANHO_HASH];
} TabelaHash;

// ============================================
// FUNÇÕES DA ÁRVORE BINÁRIA (MANSÃO)
// ============================================

NoSala* criarSala(const char* nome, const char* pista) {
    NoSala* novaSala = (NoSala*)malloc(sizeof(NoSala));
    strcpy(novaSala->nome, nome);
    if (pista != NULL) {
        strcpy(novaSala->pista, pista);
    } else {
        strcpy(novaSala->pista, "");
    }
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

NoSala* construirMansao() {
    // Criando a estrutura da mansão
    NoSala* raiz = criarSala("Hall de Entrada", NULL);
    
    // Nível 1
    raiz->esquerda = criarSala("Biblioteca", "Carta Rasgada");
    raiz->direita = criarSala("Sala de Jantar", "Copo de Vinho");
    
    // Nível 2 - Esquerda
    raiz->esquerda->esquerda = criarSala("Escritorio", "Documento Financeiro");
    raiz->esquerda->direita = criarSala("Sala de Leitura", "Livro Antigo");
    
    // Nível 2 - Direita
    raiz->direita->esquerda = criarSala("Cozinha", "Faca Ensanguentada");
    raiz->direita->direita = criarSala("Adega", "Garrafa Quebrada");
    
    // Nível 3
    raiz->esquerda->esquerda->esquerda = criarSala("Cofre Secreto", "Joia Roubada");
    raiz->direita->esquerda->direita = criarSala("Despensa", "Veneno");
    
    return raiz;
}

// ============================================
// FUNÇÕES DA BST (PISTAS)
// ============================================

NoPista* criarNoPista(const char* pista) {
    NoPista* novo = (NoPista*)malloc(sizeof(NoPista));
    strcpy(novo->pista, pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

NoPista* inserirPista(NoPista* raiz, const char* pista) {
    if (raiz == NULL) {
        return criarNoPista(pista);
    }
    
    int comparacao = strcmp(pista, raiz->pista);
    
    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    // Se comparacao == 0, a pista já existe, não adiciona duplicata
    
    return raiz;
}

void emOrdem(NoPista* raiz) {
    if (raiz != NULL) {
        emOrdem(raiz->esquerda);
        printf("   - %s\n", raiz->pista);
        emOrdem(raiz->direita);
    }
}

int buscarPista(NoPista* raiz, const char* pista) {
    if (raiz == NULL) {
        return 0;
    }
    
    int comparacao = strcmp(pista, raiz->pista);
    
    if (comparacao == 0) {
        return 1;
    } else if (comparacao < 0) {
        return buscarPista(raiz->esquerda, pista);
    } else {
        return buscarPista(raiz->direita, pista);
    }
}

// ============================================
// FUNÇÕES DA TABELA HASH
// ============================================

int funcaoHash(const char* chave) {
    int hash = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        hash += (unsigned char)chave[i];
    }
    return hash % TAMANHO_HASH;
}

TabelaHash* criarTabelaHash() {
    TabelaHash* tabela = (TabelaHash*)malloc(sizeof(TabelaHash));
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabela->tabela[i] = NULL;
    }
    return tabela;
}

void inserirNaHash(TabelaHash* tabela, const char* pista, const char* suspeito) {
    int indice = funcaoHash(pista);
    
    // Criar novo nó
    NoHash* novo = (NoHash*)malloc(sizeof(NoHash));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->proximo = NULL;
    
    // Inserir no início da lista (tratamento de colisão)
    if (tabela->tabela[indice] == NULL) {
        tabela->tabela[indice] = novo;
    } else {
        novo->proximo = tabela->tabela[indice];
        tabela->tabela[indice] = novo;
    }
}

char* consultarSuspeito(TabelaHash* tabela, const char* pista) {
    int indice = funcaoHash(pista);
    NoHash* atual = tabela->tabela[indice];
    
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    
    return NULL;
}

void exibirRelacoes(TabelaHash* tabela) {
    printf("\n=== RELACAO PISTAS -> SUSPEITOS ===\n");
    for (int i = 0; i < TAMANHO_HASH; i++) {
        NoHash* atual = tabela->tabela[i];
        while (atual != NULL) {
            printf("   %s -> %s\n", atual->pista, atual->suspeito);
            atual = atual->proximo;
        }
    }
}

void encontrarCulpado(TabelaHash* tabela) {
    // Array para contar menções de cada suspeito
    char suspeitos[20][50];
    int contagem[20] = {0};
    int numSuspeitos = 0;
    
    // Percorrer toda a tabela hash
    for (int i = 0; i < TAMANHO_HASH; i++) {
        NoHash* atual = tabela->tabela[i];
        while (atual != NULL) {
            // Procurar se o suspeito já está na lista
            int encontrado = 0;
            for (int j = 0; j < numSuspeitos; j++) {
                if (strcmp(suspeitos[j], atual->suspeito) == 0) {
                    contagem[j]++;
                    encontrado = 1;
                    break;
                }
            }
            
            // Se não encontrou, adicionar novo suspeito
            if (!encontrado && numSuspeitos < 20) {
                strcpy(suspeitos[numSuspeitos], atual->suspeito);
                contagem[numSuspeitos] = 1;
                numSuspeitos++;
            }
            
            atual = atual->proximo;
        }
    }
    
    // Encontrar o suspeito com mais pistas
    if (numSuspeitos > 0) {
        int maxContagem = 0;
        int indiceCulpado = 0;
        
        printf("\n=== ANALISE DE SUSPEITOS ===\n");
        for (int i = 0; i < numSuspeitos; i++) {
            printf("   %s: %d pista(s)\n", suspeitos[i], contagem[i]);
            if (contagem[i] > maxContagem) {
                maxContagem = contagem[i];
                indiceCulpado = i;
            }
        }
        
        printf("\n*** PRINCIPAL SUSPEITO: %s (%d pista(s) relacionadas) ***\n", 
               suspeitos[indiceCulpado], maxContagem);
    }
}

// ============================================
// FUNÇÕES DO JOGO
// ============================================

void inicializarSuspeitos(TabelaHash* tabela) {
    // Relacionar pistas encontradas na mansão com suspeitos
    inserirNaHash(tabela, "Carta Rasgada", "Sr. Black");
    inserirNaHash(tabela, "Copo de Vinho", "Sra. Scarlet");
    inserirNaHash(tabela, "Documento Financeiro", "Sr. Black");
    inserirNaHash(tabela, "Livro Antigo", "Prof. Plum");
    inserirNaHash(tabela, "Faca Ensanguentada", "Coronel Mustard");
    inserirNaHash(tabela, "Garrafa Quebrada", "Sra. Scarlet");
    inserirNaHash(tabela, "Joia Roubada", "Sr. Black");
    inserirNaHash(tabela, "Veneno", "Sra. Peacock");
}

void explorarSalas(NoSala* sala, NoPista** pistasEncontradas, TabelaHash* tabela) {
    if (sala == NULL) {
        printf("Você está em um caminho sem saída!\n");
        return;
    }
    
    printf("\n===========================================\n");
    printf("Você está em: %s\n", sala->nome);
    
    // Verificar se há pista na sala
    if (strlen(sala->pista) > 0) {
        printf("*** Você encontrou uma pista: %s ***\n", sala->pista);
        *pistasEncontradas = inserirPista(*pistasEncontradas, sala->pista);
        
        // Consultar suspeito relacionado
        char* suspeito = consultarSuspeito(tabela, sala->pista);
        if (suspeito != NULL) {
            printf("    Esta pista está relacionada a: %s\n", suspeito);
        }
    }
    
    // Verificar se é folha
    if (sala->esquerda == NULL && sala->direita == NULL) {
        printf("Você chegou ao fim deste caminho!\n");
        printf("===========================================\n");
        return;
    }
    
    // Menu de navegação
    char opcao;
    do {
        printf("\nPara onde deseja ir?\n");
        if (sala->esquerda != NULL) {
            printf("   [E] Esquerda -> %s\n", sala->esquerda->nome);
        }
        if (sala->direita != NULL) {
            printf("   [D] Direita -> %s\n", sala->direita->nome);
        }
        printf("   [V] Voltar\n");
        printf("   [P] Ver pistas coletadas\n");
        printf("   [S] Sair da exploração\n");
        printf("Escolha: ");
        scanf(" %c", &opcao);
        opcao = toupper(opcao);
        
        if (opcao == 'E' && sala->esquerda != NULL) {
            explorarSalas(sala->esquerda, pistasEncontradas, tabela);
            break;
        } else if (opcao == 'D' && sala->direita != NULL) {
            explorarSalas(sala->direita, pistasEncontradas, tabela);
            break;
        } else if (opcao == 'V') {
            break;
        } else if (opcao == 'P') {
            printf("\n--- Pistas Coletadas (em ordem alfabética) ---\n");
            if (*pistasEncontradas == NULL) {
                printf("   Nenhuma pista coletada ainda.\n");
            } else {
                emOrdem(*pistasEncontradas);
            }
        } else if (opcao == 'S') {
            return;
        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 'S');
}

void menuPrincipal(NoSala* mansao, NoPista* pistas, TabelaHash* tabela) {
    int opcao;
    
    do {
        printf("\n\n");
        printf("╔═══════════════════════════════════════════╗\n");
        printf("║      DETECTIVE QUEST - NIVEL MESTRE      ║\n");
        printf("╚═══════════════════════════════════════════╝\n");
        printf("\n1. Explorar a Mansão\n");
        printf("2. Ver Pistas Coletadas\n");
        printf("3. Ver Relação Pistas -> Suspeitos\n");
        printf("4. Análise Final - Descobrir o Culpado\n");
        printf("5. Sair do Jogo\n");
        printf("\nEscolha uma opção: ");
        scanf("%d", &opcao);
        
        switch(opcao) {
            case 1:
                explorarSalas(mansao, &pistas, tabela);
                break;
                
            case 2:
                printf("\n=== PISTAS COLETADAS ===\n");
                if (pistas == NULL) {
                    printf("Nenhuma pista coletada ainda.\n");
                } else {
                    emOrdem(pistas);
                }
                break;
                
            case 3:
                exibirRelacoes(tabela);
                break;
                
            case 4:
                if (pistas == NULL) {
                    printf("\nVocê ainda não coletou nenhuma pista!\n");
                    printf("Explore a mansão primeiro.\n");
                } else {
                    encontrarCulpado(tabela);
                }
                break;
                
            case 5:
                printf("\nObrigado por jogar Detective Quest!\n");
                break;
                
            default:
                printf("\nOpção inválida!\n");
        }
    } while (opcao != 5);
}

// ============================================
// FUNÇÃO PRINCIPAL
// ============================================

int main() {
    printf("╔═══════════════════════════════════════════╗\n");
    printf("║   BEM-VINDO AO DETECTIVE QUEST!          ║\n");
    printf("║   Nível Mestre - Solucione o Mistério   ║\n");
    printf("╚═══════════════════════════════════════════╝\n");
    printf("\nVocê é um detetive investigando um crime\n");
    printf("na mansão. Explore os cômodos, colete pistas\n");
    printf("e descubra quem é o culpado!\n");
    
    // Inicializar estruturas
    NoSala* mansao = construirMansao();
    NoPista* pistas = NULL;
    TabelaHash* tabela = criarTabelaHash();
    
    // Inicializar relações pista -> suspeito
    inicializarSuspeitos(tabela);
    
    // Iniciar o jogo
    menuPrincipal(mansao, pistas, tabela);
    
    // Liberar memória (simplificado - em produção, criar funções de liberação completas)
    free(tabela);
    
    return 0;
}