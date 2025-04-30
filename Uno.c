#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h> // uni e termios p pegar as setas
#include <time.h> // p usar srand
#include <string.h> // usar %s
#include <stdbool.h>

void limparTela(){ 
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

enum cores{ // possiveis cores das cartas
    AMARELO,
    VERDE,
    AZUL,
    VERMELHO,
    PRETO
};

enum tipoDihCarta{ // possiveis tipos de carta
    NORMAL, 
    MAIS2,
    REVERTER,
    BLOQUEIO,
    MAIS4,
    CORINGA
};

typedef struct{  // itens da carta
    int numero;
    enum cores cor;
    enum tipoDihCarta tipo;
    int validade; // adicionei posteriormente, importante para verificação
}Carta;

typedef struct{
    int voce;
    int Jgdr1;
    int Jgdr2;
    int Jgdr3;
}Ranking;

void salvarRanking(const Ranking* ranking, const char* arquivo){
    FILE* file = fopen(arquivo, "w");
    if (file == NULL) return;

    fprintf(file, "voce:%d\n", ranking->voce);
    fprintf(file, "Jgdr1:%d\n", ranking->Jgdr1);
    fprintf(file, "Jgdr2:%d\n", ranking->Jgdr2);
    fprintf(file, "Jgdr3:%d\n", ranking->Jgdr3);
    fclose(file);
}

void carregarRanking(Ranking *ranking, const char *arquivo){
    FILE* file = fopen(arquivo, "r");
    if (file == NULL) return;
    
    fscanf(file, "voce:%d\n", &ranking->voce);
    fscanf(file, "Jgdr1:%d\n", &ranking->Jgdr1);
    fscanf(file, "Jgdr2:%d\n", &ranking->Jgdr2);
    fscanf(file, "Jgdr3:%d\n", &ranking->Jgdr3);
    fclose(file);
}

void atualizarRanking(Ranking* ranking, int jogador, int pontos){
    switch (jogador) {
        case 0:
            ranking->voce += pontos;
            break;
        case 1:
            ranking->Jgdr1 += pontos;
            break;
        case 2:
            ranking->Jgdr2 += pontos;
            break;
        case 3:
            ranking->Jgdr3 += pontos;
            break;
    }
}

Carta* criarBaralho(){ // criação do baralho
    
Carta *baralho = malloc(sizeof(Carta) * 108);
int i=0;

    //cartas normies
    for(int cor = AMARELO; cor <= VERMELHO; cor++){ // mesma coisa q um for de 0 a 4, no caso aq é p criar 9 cartas de cada cor
        for(int num=0; num<=9; num++){
            
            baralho[i].numero = num;
            baralho[i].cor = cor;
            baralho[i].tipo = NORMAL;
            baralho[i].validade = 1;
            i++;
            
            if(num != 0){ // tirando o 0, tem 2 carta igual
                baralho[i].numero = num;
                baralho[i].cor = cor;
                baralho[i].tipo = NORMAL;
                baralho[i].validade = 1;
                i++;
            }//fim do if
        }//fim do laço numero
    }// fim do laço cor
    
    //cartas especiais(apeloas)
    enum tipoDihCarta cartasEspeciais[] = {
        MAIS2, REVERTER, BLOQUEIO
    };// criando bagui p fzr o bagui sla aa
    
    for(int cor = AMARELO; cor <= VERMELHO; cor++){
        for(int j = 0; j < 3; j++){
            
            baralho[i].numero = -1; // cartasEspeciais n tem numero
            baralho[i].cor = cor;
            baralho[i].tipo = cartasEspeciais[j];
            baralho[i].validade = 1;
            i++;
            
            baralho[i].numero = -1;
            baralho[i].cor = cor;
            baralho[i].tipo = cartasEspeciais[j];
            baralho[i].validade = 1;
            i++;
        
        }// fim do laço tipo
    }//fim do laço cor
    
    //colocando +4
    for(int j=0; j < 4; j++){
        baralho[i].numero = -1;
        baralho[i].cor = PRETO;
        baralho[i].tipo = MAIS4;// +4 e indo p CORINGA dps;
        baralho[i].validade = 1;
        i++;
    }
    
    //colocando coringa
    for(int j=0; j < 4; j++){
        baralho[i].numero = -1;
        baralho[i].cor = PRETO;
        baralho[i].tipo = CORINGA;
        baralho[i].validade = 1;
        i++;
    }
    return baralho;
}//fim do criarBaralho

void* embaralharBaralho(Carta baralho[], int tam){ // misturando o local das cartas com srand
        srand(time(NULL));
    for (int i = tam - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        Carta temp = baralho[i];
        baralho[i] = baralho[j];
        baralho[j] = temp;
    }
}
// função para capturar as teclas pressionadas, usada pra navegar no menu pelas teclas
char getch(){
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt); //pega as configurações do terminal
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // desativa o modo canônico e o echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // aplica as novas configurações
    ch = getchar(); 
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restaura as configurações anteriores
    return ch;
}

void regritas(){ // coisas sobre o jogo
    
    limparTela();
    usleep(500000);
    printf("\n\nCONHEÇA AGORA AS REGRAS DO  \"Dos\"\"\n");
    printf("\nCONTEÚDO: 108 cartas de jogo, sendo: \n");
    printf("19 Cartas Azuis - Indo de 0 a 9 | 19 Cartas Verdes - Indo de 0 a 9 | 19 Cartas Amarelas - Indo de 0 a 9 e 19 Cartas Vermelhas - Indo de 0 a 9\n");
    printf("8 Cartas Comprar Duas Cartas - 2 de cada cor | 8 Cartas Inverter - 2 de cada cor | 8 Cartas Pular - 2 de cada cor\n");
    printf("4 Cartas Curinga e 4 Cartas Curinga Comprar Quatro Cartas\n\n");
    printf("\nPara sair, digite 'k' no momento da escolha da jogada. \n");
    printf("A cada rodada você irá se deperar com uma carta no montante, pra fazer uma jogada válida, você terá que observar a carta no montante e as cartas em sua mão\nse tiver alguma carta da mesma cor, do mesmo tipo(em caso de cartas especiais), do mesmo número ou um Mais 4/Coringa você terá uma jogava válida.");
    
    printf("\nPressione ENTER para voltar ao menu inicial");
    getchar();
}

int menuInicial(){ // menu "interativo"
    int opcao = 1;  // opção inicial é a 1
    char tecla;

    while (1) {
        limparTela();

        printf("\n");
        printf(" ╔═══════════════════════════════════════╗\n");
        printf("╝               Bv ao Dos                 ╚\n"); 
        printf("╠══|══|══|══|══|══|══|══|══|══|══|══|══|══╣\n");

        // opções do menu
        for (int i = 1; i <= 3; i++) {
            if (i == opcao) {
                // se for a opção selecionada destaca
                printf(" > ");
            } 
            else {
                printf("   ");
            }

            if (i == 1) printf("1. Iniciar Jogo\n");
            if (i == 2) printf("2. Instruções\n");
            if (i == 3) printf("3. Sair\n");
        }
        
        printf(" ╚═══════════════════════════════════════╝\n");
        
        tecla = getch();  // getch para capturar a tecla

        // se a tecla for uma sequência de escape (início das setas)
        if (tecla == 27){  // ESC
            tecla = getch();  // pega o próximo caractere (que será '[')
            if (tecla == 91){  // verifica se é uma tecla de seta
                tecla = getch();  // pega o código da tecla de seta
                if (tecla == 65 && opcao > 1){  // 65 = seta para cima
                    opcao--;
                }
                else if(tecla == 65 && opcao == 1){
                    opcao = 3;
                }
                else if (tecla == 66 && opcao < 3){  // 66 = seta para baixo
                    opcao++;
                }
                else if (tecla == 66 && opcao == 3){ 
                    opcao = 1;
                }
            }
        }

        // caso pressionado enter
        if (tecla == 10) {  // Enter
            if (opcao == 1) {
                return 1;
                break;
            } else if (opcao == 2){
                regritas();
                continue;  
            } else if (opcao == 3){
                return 3;
                break;
            }
        }
    }
}

//mesmo coisa que acima, só que referente ao qtdjogadores dessa vez.
int qtdJogadores(){
    int opcao = 1;
    char tecla;

    while (1){
        limparTela();

 
        printf("\n");
        printf(" ╔═══════════════════════════════════════╗\n");
        printf("╝               Bv ao Dos                 ╚\n"); 
        printf("╠══|══|══|══|══|══|══|══|══|══|══|══|══|══╣\n");

        for (int i = 1; i <= 3; i++){
            if (i == opcao){
                printf(" > ");
            }
            else {
                printf("   ");
            }

            if (i == 1) printf("1. 2 Jogadores\n");
            if (i == 2) printf("2. 3 Jogadores\n");
            if (i == 3) printf("3. 4 Jogadores\n");
        }
        
        printf(" ╚═══════════════════════════════════════╝\n");

        tecla = getch();
        
        if (tecla == 27){ 
            tecla = getch();
            if (tecla == 91){ 
                tecla = getch();  
                if (tecla == 65 && opcao > 1){
                    opcao--;
                }
                else if(tecla == 65 && opcao == 1){
                    opcao = 3;
                }
                else if(tecla == 66 && opcao < 3){
                    opcao++;
                }
                else if (tecla == 66 && opcao == 3){ 
                    opcao = 1;
                }
            }
        }

        if(tecla == 10){ 
            if (opcao == 1){
                return 2;
                break;
            } else if(opcao == 2){
                return 3;
                break;
            } else if(opcao == 3){
                return 4;
                break;
            }
        }
    }
}

Carta** criarMão(int qtdJogadores, Carta* baralho, int** posicaoBaralho){ // criando as mãos dos jogadores
    
    Carta** maos = (Carta**) malloc(qtdJogadores * sizeof(Carta)); //criando um vetor do tamanho da qtdJogadores
    if (maos == NULL) exit(1); // caso falhe
    
    for (int i = 0; i < qtdJogadores; i++){
        
        maos[i] = (Carta*) malloc(7 * sizeof(Carta)); // ta criando um vetor pra cada elemento do vetor de tamanho qtdJogadores
        if (maos[i] == NULL) exit(1);

        //distribuindo
        for (int j = 0; j < 7; j++){
            maos[i][j] = baralho[**posicaoBaralho]; // preenche as 7 cartas iniciais de cada jogador
            (**posicaoBaralho)++;
        }
    }

    return maos;
}

const char* tipoParaString(enum tipoDihCarta tipoo){ // convertendo enum para string, facilita na hora de printar as cartas
    switch(tipoo){
        case NORMAL: return "Normal";
        case MAIS2: return "    +2     ";
        case MAIS4: return "    +4     ";
        case REVERTER: return "  Reverter ";
        case BLOQUEIO: return "  Bloqueio ";
        case CORINGA: return "  Coringa  ";
    }
}

const char* CorParaString(enum cores cor){
    switch(cor){
        case AMARELO: return "  Amarelo  ";
        case VERDE: return "   Verde   ";
        case AZUL: return "    Azul   ";
        case VERMELHO: return "  Vermelho ";
        case PRETO: return "   Preto   ";
    }
}

void colorirTextoCarta(Carta* carta, const char* str){ // se der tempo implemento, é para printar o texto da carta com a cor da mesma
    if(carta->cor==0)
        printf("\033[33;1m%s\033[0m",str);
    else if(carta->cor==1)
        printf("\033[32m%s\033[0m",str);
    else if(carta->cor==2)
        printf("\033[34m%s\033[0m",str);
    else if(carta->cor ==3)
        printf("\033[31;1m%s\033[0m",str);
    else if(carta->cor == 4)
        printf("\033[33;3m%s\033[0m",str);
}

int colorirNumeroCarta(Carta* carta, int num){
    if(carta->cor==0)
        printf("\033[33;1m%d\033[0m",num);
    else if(carta->cor==1)
        printf("\033[32m%d\033[0m",num);
    else if(carta->cor==2)
        printf("\033[34m%d\033[0m",num);
    else if(carta->cor ==3)
        printf("\033[31;1m%d\033[0m",num);
    else if(carta->cor == 4)
        printf("\033[33;3m%d\033[0m",num);
}

void printarCartas(Carta** maos, int* qtdCartas, Carta* noTopo, int* ptrVez){
    
    int limite = qtdCartas[0]+1;
        
        printf("\n");
        // imprime a linha superior de todas as cartas
        for (int i = 0; i < limite; i++){ // qtdCartas[0]+1 pois quero imprimir o montante junto
            if(i==0)
                printf("   Montante       "); 
            else if(*ptrVez==0)
                printf("       %d      ",i); // caso seja a vez do user ele vai printar o número da posição da carta, facilita na hora de escolher
        }
        printf("\n");
        
        for (int i = 0; i < limite; i++) { // topo das cartas
            if(i==0)
                printf(" ___________        ");
            else if(*ptrVez == 0)
                printf(" ___________  ");
        }
        
        // imprime as linhas intermediárias
        for (int linha = 0; linha <=5; linha++){
            for (int i = 0; i < limite; i++){
                if(i==0){ // imprime o montante
                    switch (linha){
                        case 1: 
                            if(noTopo->numero != -1) // caso n seja carta especial
                                printf("|     %d     |       ", noTopo->numero); //numero
                            else                    // caso seja carta especial
                                printf("|%s|       ", tipoParaString(noTopo->tipo)); // tipo
                            break;
                        case 2:
                                printf("|           |       ");
                            break;
                        case 3:
                                printf("|%s|       ", CorParaString(noTopo->cor));//cor
                            break;
                        case 4:
                                printf("|           |       ");
                            break;
                        case 5:
                            if(noTopo->tipo != 0) // caso seja especial
                                printf("|%s|       ", tipoParaString(noTopo->tipo));
                            else                  // caso n seja
                                printf("|     %d     |       ", noTopo->numero); 
                            break;
                    };
                }
                else if(i!=0 && *ptrVez == 0){ // printando as cartas do usuario
                    switch(linha){
                        case 1:
                    // concertando if(qtdCartas[0]%12==0)
                    
                            if(maos[0][i-1].numero != -1)
                                 printf("|     %d     | ", (maos[0][i-1].numero)); // numero
                            else
                                printf("|%s| ", tipoParaString(maos[0][i-1].tipo)); // tipo, no caso das especiais
                            break;
                        case 2:
                                printf("|           | ");
                            break;
                        case 3:
                                printf("|%s| ", CorParaString(maos[0][i-1].cor));
                            break;
                        case 4:
                                printf("|           | ");
                            break;
                        case 5:
                            if(maos[0][i-1].tipo != 0)
                                printf("|%s| ", tipoParaString(maos[0][i-1].tipo));
                            else
                                printf("|     %d     | ", (maos[0][i-1].numero));
                            break;
                    }
                }
        }
            printf("\n");
    }
    
        for (int i = 0; i < limite; i++){
            if(i==0){
                printf("|___________|       ");
            }
            else if(*ptrVez == 0){
                 printf("|___________| ");
            }
        }
        printf("\n");
    
}


void cavar(Carta** maos, Carta* baralho, int* ptrPosicaoBaralho, int* ptrVez, int* qtdCartas){
    Carta* novaMao;
    novaMao = realloc(maos[*ptrVez], (qtdCartas[*ptrVez] + 1) * sizeof(Carta)); 
    
    if (novaMao == NULL){ exit(1); printf("Erro no cavar");}
    
    maos[*ptrVez] = novaMao;
    // adicionar a nova carta ao final da mão
    maos[*ptrVez][qtdCartas[*ptrVez]] = baralho[*ptrPosicaoBaralho]; //qtdCartas é um vetor do tamanho da qtdJogadores que guarda a quantidade de carta de cada "jogador"

    // atualizar contagem de cartas e posição do baralho
    (qtdCartas[*ptrVez])++;
    (*ptrPosicaoBaralho)++;
}

void aplicarEfeito(Carta** maos, Carta*baralho,
                    Carta* noTopo, int* ptrPosicaoBaralho, int* qtdCartas, 
                    int* ptrVez, int escolhaQtd, int* direcao){
                        
    switch (noTopo->tipo){
        //antes do efeito da carta ser aplicado *ptrVez é atualizado pro prox jogador.
        case MAIS2:
        
            for(int i=0; i<2; i++){
                cavar(maos, baralho, ptrPosicaoBaralho, ptrVez, qtdCartas);
                if(*ptrPosicaoBaralho==107) *ptrPosicaoBaralho = 0;
            }
            
            if(*ptrVez!=0) printf("\nO jogador %d cavou 2 cartas e perdeu a vez.", *ptrVez);
            else printf("\nVocê cavou 2 cartas e perdeu a vez.");
            
            *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd; /* "*ptrVez + *direcao" diz quem é o proximo, "+ escolhaQtd" é pra que esse valor não fique negativo 
            e "% escolhaQtd é pra que o valor fique entre 0 e a qtdJogadores.
            ex: *ptrVez = 0 
                *direcao = 1, 
                escolhaQtd = 2,
                0 = 0 + 1 + 2 % 2
                0 = 3 % 2 =  1.
            "
            */break;
            
        case BLOQUEIO:
        
            if(*ptrVez!=0) printf("\nO jogador %d foi bloqueado e perdeu a vez.", *ptrVez);
            else printf("\nVocê foi bloqueado e perdeu a vez.");
            
            *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd; // só passa pro prox ( como passa anteriormente, pula um jogador ex: 1 jogou, prox jogador 3)
            
            break;
            
        case MAIS4:
            
            for(int i=0; i<4; i++){
                cavar(maos, baralho, ptrPosicaoBaralho, ptrVez, qtdCartas);
                if(*ptrPosicaoBaralho==107) *ptrPosicaoBaralho = 0;
            }
            
            if(*ptrVez!=0) printf("\nO jogador %d cavou 4 cartas e perdeu a vez.", *ptrVez);
            else printf("\nVocê cavou 4 cartas e perdeu a vez.");
            
            *direcao = -*direcao; // vira o jogo
            
            *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd; // volta p qm lançou pra escolher a cor
            
          /*AMARELO,
            VERDE,
            AZUL,
            VERMELHO,*/
            
            if(*ptrVez == 0){ 
                printf("\nEscolha a cor: (1 - Amarelo, 2 - VERDE, 3 -  AZUL, 4 - VERMELHO: "); // caso seja a vez do user ele escolhe a cor
                scanf("%ui", &noTopo->cor);
                noTopo->cor -= 1; // enum comeca com 0, ajustando o valor recebido para corresponder ao numero das cores no enum
                
            }
            else{
                noTopo->cor = rand() % 4;// caso seja outro player é escolhido aleatoriamente
            }
            
            *direcao = -*direcao; // volta o jogo a direção que estava anteriormente
            
            *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd; // volta pro jogador que cavou
            
            //printf("Agora o jogador %d tem %d cartas!", *ptrVez, (qtdCartas[*ptrVez]-1));
            
            *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd; // proximo jogador depois do que cavou
            
            break;
            
        case CORINGA:
        
        //mesma coisa que o MAIS4 só que sem cavar
        
        *direcao = -*direcao;                                     // inverte direção
        *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd; // volta pra qm jogou
        
            if(*ptrVez == 0){
                printf("\nEscolha a cor: (1 - Amarelo, 2 - VERDE, 3 -  AZUL, 4 - VERMELHO: ");
                scanf("%ui", &noTopo->cor);
                noTopo->cor -= 1;
            }
            else{
                noTopo->cor = rand() % 4;
            }
        *direcao = -*direcao; // vira o jogo
        *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd; // volta p qm lançou
            break;
        
    }
}

bool teste(Carta carta, Carta* noTopo){ // testa se carta valida ou não

    if (carta.cor == noTopo->cor || // se a carta for igual a que está no topo do montante é valida
        (carta.tipo == noTopo->tipo && carta.tipo !=0) ||
        carta.numero == noTopo->numero) 
        return true;
        
    else if (carta.tipo == CORINGA || carta.tipo == MAIS4) // se for um coringa ou mais 4 não importará a carta que esteja no topo
        return true;
        
    else
        return false;
        
    }
    
Carta CartaJogada(Carta** maos, Carta* noTopo, int* qtdCartas, int* ptrPosicaoBaralho, int* ptrVez, int escolhaQtd, int* direcao, Carta* baralho){
    
    Carta cartaNula = {0, 0, 0, 0}; // retornada pela função caso o jogador não jogue
    Carta aux; // retornada pela função caso o jogador jogue
    int cartaEscolhida = -1; 
    bool temCarta = false;
    int qtdAtualCartas = qtdCartas[*ptrVez]; // quantidade de cartas na mão do jogador atual
    char ck;

    // verifica se o jogador possui cartas válidas para jogar
    for (int j = 0; j < qtdAtualCartas; j++) {
        temCarta = teste(maos[*ptrVez][j], noTopo); //loop p passar 1 carta por vez
        if (temCarta){ // se f(x) retornar true há cartas válidas na mão do jogador
            cartaEscolhida = j; // salvando a carta que foi passada pra f(x), no caso, quando não é a vez do usuario é aqui que os outros decidem a carta que vão jogar
            break;
        }
    }

    if (temCarta){ // se tiver carta
        
        if (*ptrVez == 0){ // vez do usuario
            printf("É a sua vez de jogar! Deseja jogar nesta rodada? (S/N/k): ");
            ck = getchar(); // pegando entrada

            while (ck != 'S' && ck != 's' && ck != 'N' && ck != 'n' && ck != 'k') {
                ck = getchar();
                getchar(); // Consumindo o caractere de nova linha
            }

            if (ck == 'N' || ck == 'n') { // caso não decida jogar
                return cartaNula;
            }

            if (ck == 'k') { // caso decida sair
                printf("Valeuu..");
                exit(1);
            }

            // Loop para escolha da carta válida
            int tentativas = 3;
            while (tentativas > 0){
                printf("Escolha uma carta (1 - %d): ", qtdAtualCartas);
                scanf("%d", &cartaEscolhida);
                getchar(); // Consumindo o caractere de nova linha

                cartaEscolhida -= 1; // ajustando indice
              
                // Verificando se a escolha é válida
                if (cartaEscolhida >= 0 && cartaEscolhida < qtdAtualCartas &&
                    teste(maos[*ptrVez][cartaEscolhida], noTopo)) {
                    break; // quando a carta é válida sai do loop
                }

                printf("Jogada inválida. Tente novamente.\n");

                tentativas--; // Diminuindo o número de tentativas
            }

            if (tentativas == 0) { // Caso o jogador exceda o número de tentativas
                printf("\nSereis vós, por ventura, tão desprovido de inteligência a ponto de cometer tal ignomínia, que até os\nmais humildes pareceriam sábios diante de tal desatino? Terás, pois, tua vez de jogar nesta rodada\nprontamente retirada, como um tolo que não merece nem o direito de prosseguir em tal jogo. E, como\npeso de tua burrice, cavarás uma carta, que não fará senão evidenciar o abismo de tua incapacidade,\ndeixando claro a todos o quão longe estás da dignidade de participar...\n");
                getchar();
                return cartaNula;
            }
        }
        else {
            printf("\nÉ a vez do jogador %d agora!", *ptrVez);
        }

        // Verifica se a carta escolhida é válida para jogar
        if (maos[*ptrVez][cartaEscolhida].cor == noTopo->cor ||
            (maos[*ptrVez][cartaEscolhida].tipo == noTopo->tipo && maos[*ptrVez][cartaEscolhida].tipo != 0)||
            maos[*ptrVez][cartaEscolhida].numero == noTopo->numero||
            maos[*ptrVez][cartaEscolhida].tipo == 4 || // +4
            maos[*ptrVez][cartaEscolhida].tipo == 5) //CORINGA
        { // if carta valida
            
            aux = maos[*ptrVez][cartaEscolhida]; //  guardando a cartaEscolhida

             usleep(500000);
                int tecla=0;
                while(tecla!=10) tecla = getchar();
                
            // exibe a jogada do jogador
            if (*ptrVez != 0){
                if (aux.tipo != 0){
                    printf("\nO jogador %d jogou um: %s %d\n", *ptrVez, tipoParaString(aux.tipo), aux.cor);
                    printf("\nAgora o jogador %d tem %d cartas!\n", *ptrVez, (qtdCartas[*ptrVez]-1));
                    if(qtdCartas[*ptrVez]==0)
                        printf("O jogador %d terminou sua mão!", *ptrVez);
                }
                else{
                    printf("\nO jogador %d jogou um: %d %s\n", *ptrVez, aux.numero, CorParaString(aux.cor));
                    if(qtdCartas[*ptrVez]==0)
                        printf("O jogador %d terminou sua mão!", *ptrVez);
                }
            }
            else {
                if (aux.tipo != 0){
                    //printf("\nVocê jogou um: %s %d\n", tipoParaString(aux.tipo), aux.cor);
                    printf("\nAgora você tem %d cartas!\n", (qtdCartas[*ptrVez]-1));
                    if(qtdCartas[*ptrVez]==0)
                        printf("Você terminou sua mão!");
                }
                else{
                    printf("\nVocê jogou um: %d %s\n", aux.numero, CorParaString(aux.cor));
                    if(qtdCartas[*ptrVez]==0)
                        printf("Você terminou sua mão!");
                }
            }
            // remove a carta da mão do jogador subscrevendo a carta escolhida
            for (int j = cartaEscolhida; j < qtdAtualCartas - 1; j++) {
                maos[*ptrVez][j] = maos[*ptrVez][j + 1];
            }
  
            qtdCartas[*ptrVez]--;

            // é onde tava dando erro, sempre a realocação falhava aqui
            /*
            if (qtdCartas[*ptrVez] > 0) {
                maos[*ptrVez] = realloc(maos[*ptrVez], qtdCartas[*ptrVez] * sizeof(Carta));
                if (maos[*ptrVez]) {
                printf("erro em dimuir o tamanho");
                    exit(1);
                }
            }*/
            
            if(aux.tipo!=2)// caso não seja a carta reveter
                *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd;
            else{       //caso seja
                if(escolhaQtd==2){
                    *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd;
                    *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd;
                }
                else{
                *direcao = -*direcao;
                *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd;
                    
                }
            }
            return aux;
        }
        /*else{ // caso carta n valida
            
            if(aux.tipo!=2)// diferente de reveter
                *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd;
            else{
                *direcao = -*direcao;
                *ptrVez = (*ptrVez + *direcao + escolhaQtd) % escolhaQtd;
            
        return cartaNula;
            
            }
        }*/
    }
    
    if(*ptrVez!=0) printf("O jogador %d passou a vez.",*ptrVez);
    
    else{
        printf("\nÉ a sua vez de jogar! Deseja jogar nesta rodada? (S/N/k): ");
        
        while (ck != 'S' && ck != 's' && ck != 'N' && ck != 'n' && ck != 'k') {
                ck = getchar();
                getchar(); // Consumindo o caractere de nova linha
            }

            if (ck == 'N' || ck == 'n') { // caso não decida jogar
            printf("Você passou a vez!");
                
            
                return cartaNula;
            }

            if (ck == 'k') { // caso decida sair
                printf("Valeuu..");
                exit(1);
            }
            printf("\nDizei-me, como ousais jogar quando não possuís cartas válidas? Acaso ponderais, sequer o que dizeis ou fazeis?");
    }
    getchar();
    return cartaNula;
}

int main(){
    
    int tamanhoBaralho = 108; // tamanho
    int posicaoBaralho = 0;
    int* ptrPosicaoBaralho = &posicaoBaralho;
    int vez = 0, fim = 0;
    int* ptrVez = &vez;
    // sentido do jogo, vai ser necessário na hora de lidar com a carta reverter
    int direcao = 1; // 1 = crescente, -1 = decrescente (1, 2, 3, 4 /  1, 4 , 3 , 2)
    int *ptrDirecao= &direcao;
    int primeiro=0, segundo=0, terceiro=0, quarto=0;
    
    Carta noTopo;
    Carta *baralho = criarBaralho(); /* Só pra eu não esquecer, baralho é um vetor e as mãos dos jogadores uma triz*/
    embaralharBaralho(baralho, tamanhoBaralho); //srand misturando as cartas
    Ranking ranking;
    
    short int escolhaInc = menuInicial();
    
    switch(escolhaInc){
        case 1:
        
            usleep(500000);
            limparTela();
            
            short int escolhaQtd = qtdJogadores();
            int *qtdCartas = malloc(escolhaQtd * sizeof(int)); // como disse antes, para guardar a qtdCartas de cada player
            // malloc pq dentro do switch n da p fzr vetor
            
            //VOU CRIAR UM MONTANTE, CADA CARTA JOGADA VAI IR PRA ELE E VAI SER REMOVIDA DA MÂO DO JOGADOR
            //carta inicial do MONTANTE
            
            while(baralho[posicaoBaralho].tipo!=0){ // enquanto não achar uma carta normal esse while vai mover as cartas para a esquerda
                
                Carta aux = baralho[posicaoBaralho];
                    for(int j = posicaoBaralho; j < (107 - posicaoBaralho); j++){ // deslocando as cartas para a esquerda
                        baralho[j] = baralho[j + 1];
                    
                    baralho[107] = aux; // colocando a carta especial na ultima posição
                }//fim do for
            }//fim do while
            
            noTopo = baralho[posicaoBaralho]; // colocando a carta noTopo do montante
            //printarMontante(ptrPosicaoBaralho);
            posicaoBaralho++; // quando sair vai ser igual a 0, e se isso acontecer o primeiro jogador tera uma carta igual ao do montante
           
            // criando as mãos dos jogadores
            
            Carta** maos = criarMão(escolhaQtd, baralho, &ptrPosicaoBaralho);
            
            // maos[quantidade de jogadores][quantidade de cartas]
            for(int i=0; i<escolhaQtd; i++){ // inicializando o vetor pra cotagem de cartas
                qtdCartas[i] = 7; // 7 cartas iniciais
            }
            
            limparTela();
            Carta buffer; // p validar entrada
            bool c = false;
            while(fim<escolhaQtd && c == false){
                
            while(qtdCartas[*ptrVez]==0) *ptrVez = (*ptrVez + *ptrDirecao + escolhaQtd) % escolhaQtd; 
            
                printarCartas(maos, qtdCartas, &noTopo, ptrVez);
                buffer = CartaJogada(maos, &noTopo, qtdCartas, ptrPosicaoBaralho, ptrVez, escolhaQtd, ptrDirecao, baralho);
                if(buffer.validade != 0){ // caso CartaJogada retorne aux a cartá será valida e entrará aqui, caso retorne cartaNula vai pro else
                    noTopo = buffer;
                    while(qtdCartas[*ptrVez]==0) *ptrVez = (*ptrVez + *ptrDirecao + escolhaQtd) % escolhaQtd; 
                    aplicarEfeito(maos, baralho, &noTopo, ptrPosicaoBaralho, qtdCartas, ptrVez, escolhaQtd, ptrDirecao);
                }
                
                else{

                    usleep(1000000);
                    if(posicaoBaralho==107) posicaoBaralho = 0;
                    cavar(maos, baralho, ptrPosicaoBaralho, ptrVez, qtdCartas); // se cartaNula então atual cava e passa a vez
                    *ptrVez = (*ptrVez + *ptrDirecao + escolhaQtd) % escolhaQtd; 
                }
                
                int ordemSaida[escolhaQtd];
                int cont=0;
                
                for(int i=0; i<escolhaQtd; i++){ // verificando vencedor
                    if(qtdCartas[i]==0){
                        printf("%d",i);
                        ordemSaida[fim] = i;
                        if(fim!=escolhaQtd-1)fim++;
                        
                        if(cont==0)primeiro=i;
                        else if(cont ==1)segundo=i;
                        else if(cont ==2)terceiro=i;
                        cont++;
                        printf("\n");
                    }
                }
                    // contar qts jogadores ainda têm cartas
                    int jogadoresComCartas = 0;
                    for (int i = 0; i < escolhaQtd; i++) {
                        if (qtdCartas[i] > 0) {
                            jogadoresComCartas++;
                        }
                    }
                                    
                    if (jogadoresComCartas == 1){
                    c = true;  // finaliza o loop
                    direcao = -direcao; // inverte a direção
                    *ptrVez = (*ptrVez + direcao + escolhaQtd) % escolhaQtd; // volta pro ultimo jogador
                    
                    printf("Primeiro: %d\n", primeiro);
                    printf("Segundo: %d\n", segundo);
                    if (escolhaQtd == 3){
                        while(*ptrVez != primeiro && *ptrVez != segundo && *ptrVez <=4)
                        *ptrVez = (*ptrVez + *ptrDirecao + escolhaQtd) % escolhaQtd; 
                        printf("Terceiro: %d\n", *ptrVez);
                    }
                    if (escolhaQtd == 4) {
                        // para o caso de 4 jogadores, você já tem o terceiro e quarto definidos
                        printf("Terceiro: %d\n", terceiro);
                        while(*ptrVez != primeiro && *ptrVez != segundo && *ptrVez <=4)
                        *ptrVez = (*ptrVez + *ptrDirecao + escolhaQtd) % escolhaQtd; 
                        printf("Quarto: %d\n", *ptrVez);
                    }

            // atualizando o ranking
            carregarRanking(&ranking, "ranking.txt");
            atualizarRanking(&ranking, primeiro, 1000);
            atualizarRanking(&ranking, segundo, 750);
            
            if (escolhaQtd >= 3) atualizarRanking(&ranking, terceiro, 500);
                
            if (escolhaQtd == 4) atualizarRanking(&ranking, quarto, 250);
            
            printf("\nRanking: ");
            printf("\nVocê: %d", ranking.voce);
            printf("\nJogador 1: %d", ranking.Jgdr1);
            if (escolhaQtd >= 3) printf("\nJogador 2: %d", ranking.Jgdr2);
            if (escolhaQtd == 4) printf("\nJogador 3: %d", ranking.Jgdr3);
            
            salvarRanking(&ranking, "ranking.txt");
            }
                
            if(fim!=escolhaQtd && escolhaQtd==2) fim=0;
        }
            
            for(int i = 0; i < escolhaQtd; i++){ // dando free onde usei malloc
                free(maos[i]);
                }
            free(maos);
            free(baralho);
            
        break;
        
        case 2:
        
            usleep(500000);
            limparTela();
            
            regritas();
            
        break;
        
        case 3:
            
            printf("It's Over...");
            exit(1);
            
        break;
    }
    return 0;
}