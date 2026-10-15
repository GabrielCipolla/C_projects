#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define MAX_LINHAS 30
#define MAX_COLUNAS 50

// Estrutura para coordenadas
typedef struct {
    int x, y;
} Coordenada;

// Estrutura do Fantasma
typedef struct {
    int x, y;
    int tipo; // 1: Fácil (Aleatório), 2: Difícil (Perseguição)
} Fantasma;

// Estrutura principal do Jogo
typedef struct {
    char mapa[MAX_LINHAS][MAX_COLUNAS];
    int linhas;
    int colunas;
    Coordenada jogador;
    Fantasma fantasmas[10];
    int qtdFantasmas;
    int pontosTotais;
    int pontosColetados;
    int dificuldade;
} Jogo;

// Declaração de Funções
void limparTela();
int exibirMenuDificuldade();
int carregarMapaAleatorio(Jogo *j);
void exibirTabuleiro(Jogo j);
int moverJogador(Jogo *j, char direcao);
void moverFantasmas(Jogo *j);
int abs(int n);

int main() {
    // Aluno: Gabriel Lobato Cipolla
    setlocale(LC_ALL, "Portuguese");

    Jogo j;
    j.pontosColetados = 0;
    j.pontosTotais = 0;

    srand(time(NULL));

    // Exibe o menu inicial para escolher o modo de jogo
    j.dificuldade = exibirMenuDificuldade();

    // Carrega o mapa diretamente os mapas .txt
    if (!carregarMapaAleatorio(&j)) {
        limparTela();
        printf("Erro critico: Nao foi possivel carregar os arquivos de mapa (.txt)!\n");
        printf("Certifique-se de que mapa1.txt, mapa2.txt e mapa3.txt estao na mesma pasta.\n");
        return 1;
    }

    char tecla;
    int status = 0; // 0: Jogando, 1: Vitória, -1: Derrota

    // Loop Principal
    while (status == 0) {
        limparTela();
        exibirTabuleiro(j);

        printf("\n----------------------------------------\n");
        printf("Nome do aluno: Gabriel Lobato Cipolla\n");
        printf("Dificuldade: %s\n", (j.dificuldade == 1) ? "FACIL (Aleatorio)" : "DIFICIL (Perseguicao)");
        printf("Pontos Coletados: %d | Restantes: %d\n", j.pontosColetados, j.pontosTotais - j.pontosColetados);
        printf("Controles: [W] Cima | [S] Baixo | [A] Esquerda | [D] Direita | [Q] Sair\n");
        printf("Digite sua jogada: ");
        scanf(" %c", &tecla);


        // Tabela ASCII
        if (tecla >= 'A' && tecla <= 'Z') {
            tecla += 32;
        }

        if (tecla == 'q') {
            printf("\nJogo encerrado pelo usuario.\n");
            break;
        }

        int movimentoValido = moverJogador(&j, tecla);

        if (movimentoValido) {
            // Verifica colisão com os fantasmas após o movimento do jogador
            for (int i = 0; i < j.qtdFantasmas; i++) {
                if (j.jogador.x == j.fantasmas[i].x && j.jogador.y == j.fantasmas[i].y) {
                    status = -1; // Derrota
                }
            }

            // Condição de Vitória
            if (j.pontosColetados >= j.pontosTotais) {
                status = 1; // Vitória
            }

            // Se o jogo continua, os fantasmas se movem
            if (status == 0) {
                moverFantasmas(&j);

                // Verifica colisão após o movimento dos fantasmas
                for (int i = 0; i < j.qtdFantasmas; i++) {
                    if (j.jogador.x == j.fantasmas[i].x && j.jogador.y == j.fantasmas[i].y) {
                        status = -1; // Derrota
                    }
                }
            }
        }
    }

    limparTela();
    exibirTabuleiro(j);
    printf("\n----------------------------------------\n");
    if (status == 1) {
        printf("PARABENS! Voce venceu o jogo coletando todos os pontos!\n");
    } else if (status == -1) {
        printf("GAME OVER! Voce foi alcancado por um fantasma!\n");
    }
    printf("----------------------------------------\n");

    return 0;
}

// Limpa a tela de forma cross-platform
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Menu interativo de dificuldade
int exibirMenuDificuldade() {
    int opcao = 0;
    while (opcao != 1 && opcao != 2) {
        limparTela();
        printf("========================================\n");
        printf("           PAC-MAN! - MENU          \n");
        printf("========================================\n");
        printf("Escolha o modo de jogo:\n\n");
        printf("  [1] FACIL   - Fantasmas com movimento Aleatorio\n");
        printf("  [2] DIFICIL - Fantasmas com perseguição Inteligente\n");
        printf("----------------------------------------\n");
        printf("Opção desejada (1 ou 2): ");
        scanf("%d", &opcao);
    }
    return opcao;
}

// Sorteia e abre um arquivo .txt de mapa, limpando P e G do fundo
int carregarMapaAleatorio(Jogo *j) {
    int escolha = (rand() % 3) + 1; // Sorteia entre 1, 2 e 3
    char nomeArquivo[20];
    sprintf(nomeArquivo, "mapa%d.txt", escolha);

    FILE *f = fopen(nomeArquivo, "r");

    // Retorna 0 (falha) caso o arquivo não seja encontrado na pasta
    if (!f) {
        return 0;
    }

    char linha[MAX_COLUNAS];
    j->linhas = 0;
    j->qtdFantasmas = 0;
    j->pontosTotais = 0;

    while (fgets(linha, sizeof(linha), f) != NULL) {
        int c = 0;
        while (linha[c] != '\0' && linha[c] != '\n' && linha[c] != '\r') {
            j->mapa[j->linhas][c] = linha[c];

            if (linha[c] == 'P') {
                j->jogador.x = j->linhas;
                j->jogador.y = c;
                j->mapa[j->linhas][c] = ' '; // Remove o P do fundo fixo do mapa e coloca um espaço em branco
            } else if (linha[c] == 'G') {
                if (j->qtdFantasmas < 10) {
                    j->fantasmas[j->qtdFantasmas].x = j->linhas;
                    j->fantasmas[j->qtdFantasmas].y = c;
                    j->fantasmas[j->qtdFantasmas].tipo = j->dificuldade; // Atribui comportamento conforme a dificuldade
                    j->qtdFantasmas++;
                }
                j->mapa[j->linhas][c] = ' '; // Remove o G do fundo fixo do mapa e coloca um espaço em branco
            } else if (linha[c] == '.') {
                j->pontosTotais++;
            }
            c++;
        }
        j->colunas = c;
        j->linhas++;
    }
    fclose(f);
    return 1;
}

// Exibe o tabuleiro na tela montando a matriz temporária
void exibirTabuleiro(Jogo j) {
    char matrizTemp[MAX_LINHAS][MAX_COLUNAS];

    for (int r = 0; r < j.linhas; r++) {
        for (int c = 0; c < j.colunas; c++) {
            matrizTemp[r][c] = j.mapa[r][c];
        }
    }

    for (int i = 0; i < j.qtdFantasmas; i++) {
        matrizTemp[j.fantasmas[i].x][j.fantasmas[i].y] = 'G';
    }

    matrizTemp[j.jogador.x][j.jogador.y] = 'P';

    printf("\n=== PAC-MAN UERJ ===\n");
    for (int r = 0; r < j.linhas; r++) {
        for (int c = 0; c < j.colunas; c++) {
            printf("%c", matrizTemp[r][c]);
        }
        printf("\n");
    }
}

// Move o jogador
int moverJogador(Jogo *j, char direcao) {
    int novoX = j->jogador.x;
    int novoY = j->jogador.y;

    if (direcao == 'w') novoX--;
    else if (direcao == 's') novoX++;
    else if (direcao == 'a') novoY--;
    else if (direcao == 'd') novoY++;
    else return 0;

    if (j->mapa[novoX][novoY] == '#') {
        return 0; // Parede
    }

    if (j->mapa[novoX][novoY] == '.') {
        j->pontosColetados++;
        j->mapa[novoX][novoY] = ' ';
    }

    j->jogador.x = novoX;
    j->jogador.y = novoY;
    return 1;
}

// Movimenta os fantasmas respeitando a dificuldade selecionada
void moverFantasmas(Jogo *j) {
    int movs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // Cima, Baixo, Esquerda, Direita

    for (int i = 0; i < j->qtdFantasmas; i++) {
        int fx = j->fantasmas[i].x;
        int fy = j->fantasmas[i].y;

        if (j->fantasmas[i].tipo == 1) {
            // MODO FÁCIL: Movimentação Aleatória
            Coordenada opcoesValidas[4];
            int qtdValidas = 0;

            for (int m = 0; m < 4; m++) {
                int proxX = fx + movs[m][0];
                int proxY = fy + movs[m][1];

                if (proxX >= 0 && proxX < j->linhas && proxY >= 0 && proxY < j->colunas) {
                    if (j->mapa[proxX][proxY] != '#') {
                        opcoesValidas[qtdValidas].x = proxX;
                        opcoesValidas[qtdValidas].y = proxY;
                        qtdValidas++;
                    }
                }
            }

            if (qtdValidas > 0) {
                int sorteio = rand() % qtdValidas;
                j->fantasmas[i].x = opcoesValidas[sorteio].x;
                j->fantasmas[i].y = opcoesValidas[sorteio].y;
            }

        } else {
            // MODO DIFÍCIL: Perseguição (Distância de Manhattan)
            int melhorX = fx, melhorY = fy;
            int menorDistancia = 99999;

            for (int m = 0; m < 4; m++) {
                int proxX = fx + movs[m][0];
                int proxY = fy + movs[m][1];

                if (proxX >= 0 && proxX < j->linhas && proxY >= 0 && proxY < j->colunas) {
                    if (j->mapa[proxX][proxY] != '#') {
                        int dist = abs(proxX - j->jogador.x) + abs(proxY - j->jogador.y);
                        if (dist < menorDistancia) {
                            menorDistancia = dist;
                            melhorX = proxX;
                            melhorY = proxY;
                        }
                    }
                }
            }

            j->fantasmas[i].x = melhorX;
            j->fantasmas[i].y = melhorY;
        }
    }
}

int abs(int n) {
    return (n < 0) ? -n : n;
}