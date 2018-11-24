/*  Programa: Jogo Da Vida
       Autor: Rafael Timbó Matos (RA 106228)
       Turma MC102Y
       Versao: a
       Concluída em: 17/05/2010
       Breve descricao:
           Este programa simula o comportamento de celulas em um universo bidimensional
           de acordo com algumas regras.
*/

#include <stdio.h>
#include <allegro.h>

#define VIVA 4
#define MORTA -4
#define FALSE 0
#define TRUE -1

//_defines "personalizaveis"__
#define M 200 //100 com PROPORCAO 5
#define N 300
#define PROPORCAO 3 //tamanho de cada bacteria

//POSSIVEIS COMBINACOES
//M    N    PROPORCAO
//400  400  1
//300  300  1
//200  300  3
//220  220  3
//250  100  3
//100  300  3
//180  180  4
//150  150  4
//120  140  5
//120  120  5
//100  100  6
//100  100  5
//100  100  4
//100  100  2
//30   30   6
//10   10   20

#define DELTA_TEMPO 50 //a velocidade do jogo (usado em rest) >> MAIOR = MAIS LENTO

#define CORVIVA makecol(0, 90, 0)
#define CORMORTA makecol(255, 255, 150)
#define HOSTILIDADE_AMBIENTE 10
//_____________________________

#define TELA_LARGURA (N * PROPORCAO)
#define TELA_ALTURA (M * PROPORCAO)
//geracao0 representa geracao anterior
//geracao1 representa geracao atual

void init()
//Carrega as configurações iniciais necessárias para o jogo
{
    allegro_init();

    install_keyboard();

    install_timer();

    install_mouse();

    srand(time(NULL));

    set_color_depth(desktop_color_depth());

    if ( (set_gfx_mode(GFX_AUTODETECT_WINDOWED, TELA_LARGURA, TELA_ALTURA, 0, 0) ) != 0) {
        allegro_message(allegro_error);
        exit(EXIT_FAILURE);
    }

    set_window_title("Jogo Da Vida - Pressione ESC para sair");
}

void MaoDivina(int celula[M][N][2], int geracao)
//Esta funcao permite ao jogador criar bacterias com o clique do mouse
{

    if (mouse_b & 1)

        if (mouse_x <= TELA_LARGURA && mouse_y <= TELA_ALTURA)
            celula[mouse_y / PROPORCAO][mouse_x / PROPORCAO][geracao] = VIVA;
            //rectfill ( screen, mouse_x, mouse_y, mouse_x + PROPORCAO, mouse_y + PROPORCAO, CORVIVA);

}
int celvizinhas(int celula[M][N][2], int icel , int jcel, int geracao0)
//Esta função verificará as celulas que estão vivas ao redor da celula escolhida(numa area de 3x3)
//retornando esta quantidade.

{
    int i,j, vizinhasvivas = 0, contabilizou
    ;

    //allegro_message("Linha 50 inicio celvizinhas");
    if ( (icel != 0) && (icel != M - 1) && (jcel != 0) && (jcel != N - 1) )
    {
        for (i = icel - 1 ; i <= icel + 1 ; i++)
            for (j = jcel - 1; j <= jcel + 1 ; j++)
                if (j !=  jcel || i != icel) //nao contabilize a propria celula

                    if (celula[i][j][geracao0] == VIVA) vizinhasvivas++;
    }
    else //CASOS ESPECIAIS : BORDAS DA TELA
    {
        for (i = icel - 1 ; i <= icel + 1 ; i++)
            for (j = jcel - 1; j <= jcel + 1 ; j++)
                if (j !=  jcel || i != icel) //nao contabilize a propria celula
                {
                    contabilizou = FALSE;
                    if ( i == -1) //ligue a borda superior com a borda inferior e vice versa
                    {
                        if (celula[M - 1][j][geracao0] == VIVA)
                        {
                            vizinhasvivas++;
                            contabilizou = TRUE;
                        }
                    }
                    else if ( i == M)
                        if (celula[0][j][geracao0] == VIVA)
                        {
                            vizinhasvivas++;
                            contabilizou = TRUE;
                        }

                    if ( j == -1) //ligue a borda esquerda com a direita e vice versa
                    {
                        if (celula[i][N - 1][geracao0] == VIVA)
                        {
                            vizinhasvivas++;
                            contabilizou = TRUE;
                        }
                    }
                    else if ( j == N)
                        if (celula[i][0][geracao0] == VIVA)
                        {
                            vizinhasvivas++;
                            contabilizou = TRUE;
                        }
                    if (contabilizou == FALSE)
                        if (celula[i][j][geracao0] == VIVA) vizinhasvivas++;
                }
    }

    return vizinhasvivas;
}

void SelecaoNatural(int celula[M][N][2], int geracao1)
/*Funcao responsavel por impor todas as regras do jogo para a sobrivencia da proxima geracao
    Para fins de programação, é necessario somente dizer que a celula que possui 3 vizinhas nasce( ou se mantem viva)
                                                                                 2 vizinhas continua o seu estado(viva ou morta).
    o restante das celulas (possuem menos que 2 ou mais que 3 celulas vizinhas) morrerá.
*/
{
    int i, j,vizinhanca
    ;

    for (i = 0; i < M ; i++)
    {
        for ( j = 0; j < N ; j++)
        {
            vizinhanca = celvizinhas(celula, i, j, (geracao1 + 1) % 2);

            if (vizinhanca == 2) celula[i][j][geracao1] = celula[i][j][(geracao1 + 1) % 2]; //geracao+1 % 2 é a geracao anterior
                else if (vizinhanca == 3) celula[i][j][geracao1] = VIVA; //Celulas com 3 vizinhas continuam vivas e "mortas nascem"
                        else celula[i][j][geracao1] = MORTA;
        }
    }
}

void Desenha(int celula[M][N][2], int geracao1, BITMAP **buffer)
//Desenha o plano da geracao atual na tela
{
    int i,j;

    rectfill ( *buffer, 0, 0, TELA_LARGURA, TELA_ALTURA, CORMORTA);
    for (i = 0 ; i < M ; i++)
        for (j = 0; j < N ; j++)
            if (celula[i][j][geracao1] == VIVA)
                rectfill ( *buffer, PROPORCAO * j, PROPORCAO * i, PROPORCAO * j + PROPORCAO, PROPORCAO * i + PROPORCAO, CORVIVA);

    blit(*buffer, screen, 0, 0, 0, 0, TELA_LARGURA, TELA_ALTURA);
    //blit(fundo, buffer, 0, 0, 0, 0, TANQUE_LARGURA, TANQUE_ALTURA);
            //void rectfill(BITMAP *bmp, int x1, int y1, int x2, int y2, int color);
}

void BigBang(int celula[M][N][2])
// Funcao responsavel pela criação inicial aleatoria da vida
// Invocada somente uma vez
{
    int i,j
    ;

    for ( i = 0; i < M; i++)
    {
        for ( j = 0; j < N; j++)
        {
            celula[i][j][1] = (rand() % HOSTILIDADE_AMBIENTE == 0 ? VIVA : MORTA);
            celula[i][j][0] = celula[i][j][1];
        }
    }
}

void Sair()
{
    if (key[KEY_ESC]) exit(EXIT_SUCCESS);
}

void GerenciadorVital()
// Funcao responsavel por gerenciar o jogo e chamar as funções
{
    int celula[M][N][2], geracao
    ;
    BITMAP *buffer;

    init();
    BigBang(celula);
    show_mouse(screen);
    buffer = create_bitmap(TELA_LARGURA, TELA_ALTURA);
    for ( geracao = 0 ; !key[KEY_ESC] ; geracao = (geracao + 1) % 2)
    {
        Desenha(celula, geracao, &buffer);
        //rest(DELTA_TEMPO);
        rest_callback((long)DELTA_TEMPO, Sair);
        SelecaoNatural(celula, geracao);
        MaoDivina(celula, geracao);
    }
}

int main(void)
{
    GerenciadorVital();
    return 0;
}
END_OF_MAIN();
