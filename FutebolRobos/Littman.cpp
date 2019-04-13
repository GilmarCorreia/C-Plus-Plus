// IMPORTANTISSIMO PARA COMPILAR USE
//g++ -lm -lcurses -o jogo Littman.cpp

//install lcurses
// sudo apt-get install libncurses5-dev

//testado em um Ubuntu versao 16.04 LTS
//gcc 5.4
//g++ 5.4

//grafico instalar gnuplot
//sudo apt install gnuplot

#include <stdio.h>
#include <stdlib.h>
//#include <curses.h> rodou sem precisar disto
//#include <values.h> rodou sem precisar disto
#include <math.h>
#include "time.cpp"

void clrscr(void) {
        printf("50\E[H\E[J");
}

void gotoxy(int x, int y) {
        printf("\E[%i%d;%dH", 0, y, x);
}




// Para nao treinar, retirar o define abaixo.
#define TREINANDO

//Para n�o mostrar a telinha de jogo, retire o comentario do define abaixo
//#define IMPRIME_JOGO

#define TRIALS 200
#define JOGADAS_ALTERNADAS
#define MAX_PASSOS_POR_JOGADA 5000
//#define NUMJOGADAS 1000000
#define NUMPARTIDAS 5000     // aqui � o numero real de partidas, dividido por 10

#define TIME1 Qlearning
#define TIME2 Random

//   VARIAVEIS GLOBAIS USADAS PARA RODAR DIVERSAS VEZES.


int trial = 0;
int evol_data[TRIALS][NUMPARTIDAS/10];

int evolgolsA[TRIALS];
int evolgolsB[TRIALS];
int evolganhasA[TRIALS];
int evolganhasB[TRIALS];

float loop;

// FUNCOES PARA CALCULAR MEDIA DAS EXECUCOES ETC.

void record_evolution(int t, int rg, int steps)
{
     evol_data[t][rg] = steps;
}


void write_evol_data(float loop)
{

  FILE *run;
  int j, trial;
  char fname[20];

  int t;
  double sum, media, desvio_quad;




  // Prints cada evolucao individual. retirado.
  /*
  for (trial=0; trial<TRIALS; trial++)
    {

      sprintf(fname, "evolution-%d.txt", trial);
      run = fopen(fname, "w");

      for (j=0;j< reached_goal[trial];j++)
	fprintf(run,"%d %d\n", j, evol_data[trial][j]);
      fclose(run);
    }
  */

  // Prints the average
  {
    //run = fopen("evolution.txt", "w"); original
	
	printf("LOOP: %f", loop);

	if (loop <= 0.0)
		run = fopen("resultado/evolution-0_0.txt", "w");
	else if(loop <=0.15)
		run = fopen("resultado/evolution-0_1.txt", "w");
    else if(loop <=0.25)
		run = fopen("resultado/evolution-0_2.txt", "w");
	else if(loop <=0.35)
		run = fopen("resultado/evolution-0_3.txt", "w");
	else if(loop <=0.45)
		run = fopen("resultado/evolution-0_4.txt", "w");
	else if(loop <=0.55)
		run = fopen("resultado/evolution-0_5.txt", "w");
	else if(loop <=0.65)
		run = fopen("resultado/evolution-0_6.txt", "w");
	else if(loop <=0.75)
		run = fopen("resultado/evolution-0_7.txt", "w");
	else if(loop <=0.85)
		run = fopen("resultado/evolution-0_8.txt", "w");
	else if(loop <=0.95)
		run = fopen("resultado/evolution-0_9.txt", "w");
	else
		run = fopen("resultado/evolution-1_0.txt", "w");


    for (j=0;j< NUMPARTIDAS/10;j++)
      {
	// Calcula a soma para imprimir a media
	sum = 0;
	for (trial=0; trial<TRIALS; trial++)
	  sum += evol_data[trial][j];
	media = sum/(double)TRIALS;
	

	desvio_quad = 0;
	for (t = 0; t < TRIALS; t++)
	  desvio_quad += pow((evol_data[t][j] - media), 2);

	desvio_quad = desvio_quad / (double) (TRIALS );

	// Imprime media e desvio

	//fprintf(run,"%d %f %f \n", j, media , sqrt(desvio_quad)) ;
	fprintf(run,"%f\n", media) ;
      }
    fclose(run);
  }
}


class Simulador {
  Tipo::estado estado;
  char bolaComA;
  int xa, ya, xb, yb;
  int golsA, golsB, numPartidas, numJogada, golsA100, golsB100, ganhasA, ganhasB;
  long numJogadaGeral;
  Time *t1, *t2;
  char avancoComTeclado;
  char AJogaPrimeiro;
  FILE * evoltA;

  public: Simulador(Time *t1, Time *t2) {

    char fname[40];
	// Muda aqui 0 ou 1 para rodar direto ou avancar com enter.
    avancoComTeclado=0;
    this->t1=t1;
    this->t2=t2;
    golsA=golsB=numPartidas=golsA100=golsB100=numJogadaGeral=ganhasA=ganhasB=0;

	sprintf(fname, "resultado/timeA/evolution-tA-%d.txt", trial);
    evoltA = fopen(fname, "w");


	if (evoltA == NULL)
	{
	  printf("Impossivel abrir arquivo de evolucao do time 1.\nBailing out!\n\n");
	  exit(1);
	}

    iniciaJogo();
  }


  public: void desenhaGrade(void) {
    gotoxy(1,1);

#ifdef IMPRIME_JOGO


    printf("    +-------------------+    \n");
    printf("    |   |   |   |   |   |    \n");
    printf("+---------------------------+\n");
    printf("|   |   |   |   |   |   |   |\n");
    printf("+---------------------------+\n");
    printf("|   |   |   |   |   |   |   |\n");
    printf("+---------------------------+\n");
    printf("    |   |   |   |   |   |    \n");
    printf("    +-------------------+    \n");

#endif

	printf("\nTrial: %d; Pontos: %5dx%5d; No. partidas: %5d (%5dx%5d); \nNo. jogada: %9ld",
		   trial, golsA, golsB, numPartidas, ganhasA, ganhasB, numJogadaGeral);

	  if(numPartidas == NUMPARTIDAS)
	   {
			evolgolsA[trial] = golsA;
			evolgolsB[trial] = golsB;
			evolganhasA[trial] = ganhasA;
			evolganhasB[trial] = ganhasB;
	   }
  }

  void mostraEstado(void) {
  #ifdef IMPRIME_JOGO
   gotoxy((5-xa)*4+2, (4-ya)*2);
    if (bolaComA)
      printf("(A)");
    else
      printf(" A ");
    gotoxy((1+xb)*4+2, (1+yb)*2);
    if (bolaComA)
      printf(" B ");
    else
      printf("(B)");
  #endif
  }

  void iniciaJogo(void) {
    numJogada=0;
    numPartidas++;

	// Cada match termina quando sao marcados 10 gols
	//cada partida termina quando � mardcado um gol
    if (numPartidas%10==0)
	{

		// CAlcula quem ganhou

		if( golsA100>golsB100)
			ganhasA++;
		else if( golsA100<golsB100)
			ganhasB++;

		// Inprime o result no arquivo
		fprintf(evoltA, "%d %d %d\n", numPartidas/10, golsA, golsA100);

		// Guaarda o valor para fazer a media.
		record_evolution(trial, numPartidas/10, golsA100-golsB100);

		// reseta vars
        golsA100=golsB100=0;
	}
    bolaComA=random()%2;
    #ifdef JOGADAS_ALTERNADAS
    AJogaPrimeiro=bolaComA;
    #endif
    desenhaGrade();
    estadoB(1, 2);
    estadoA(1, 2);
    t1->inicioDeJogo();
    t2->inicioDeJogo();
  }

  void estadoA(int xa, int ya) {
    //gotoxy((5-this->xa)*4+2, (4-this->ya)*2);
    printf("   ");
    this->xa=xa;
    this->ya=ya;
    mostraEstado();
  }

  void estadoB(int xb, int yb) {
   // gotoxy((1+this->xb)*4+2, (1+this->yb)*2);
    printf("   ");
    this->xb=xb;
    this->yb=yb;
    mostraEstado();
  }

  Tipo::acao escolheAcaoA(void) {
    estado.comABola=bolaComA;
    estado.x=xa;
    estado.y=ya;
    estado.xo=4-xb;
    estado.yo=3-yb;
    return t1->acao(estado);
  }

  Tipo::acao escolheAcaoB(void) {
    estado.comABola=!bolaComA;
    estado.xo=4-xa;
    estado.yo=3-ya;
    estado.x=xb;
    estado.y=yb;
    return t2->acao(estado);
  }

  void executaAcaoA(Tipo::acao acao) {
    estado.comABola=bolaComA;
    estado.x=xa;
    estado.y=ya;
    estado.xo=4-xb;
    estado.yo=3-yb;
    executaAcao(acao, estado);
    bolaComA=estado.comABola;
    estadoA(estado.x, estado.y);
  }

  void executaAcaoB(Tipo::acao acao) {
    estado.comABola=!bolaComA;
    estado.x=xb;
    estado.y=yb;
    estado.xo=4-xa;
    estado.yo=3-ya;
    executaAcao(acao, estado);
    bolaComA=!estado.comABola;
    estadoB(estado.x, estado.y);
  }

  void executaAcao(Tipo::acao acao, Tipo::estado &estado) {
    Tipo::estado estadoAux=estado;
    switch (acao) {
      case Tipo::N:
	estado.x++;
	if (!estado.comABola) { 	//se NAO esta' com a bola
	  if (estado.x>4)		//limita em 4
	    estado.x=4;
	} else {			//se esta' com a bola
	  if (estado.x>4)
	    if (estado.y==0 || estado.y==3)
	      estado.x=4;		//fora do gol
	    else
	      estado.x=5;		//no do gol
	}
	break;
      case Tipo::S:
	estado.x--;
	if (estado.x<0)			//limita sempre em 1
	  estado.x=0;
	break;
      case Tipo::E:
	estado.y++;
	if (estado.y>3)
	  estado.y=3;
	break;
      case Tipo::W:
	estado.y--;
	if (estado.y<0)
	  estado.y=0;
	break;
    }
    if (estado.comABola) { 		//se esta' com a bola
      if (estado.x==estado.xo && estado.y==estado.yo) {
	estado=estadoAux;
	estado.comABola=0;		//perde a bola
      }
    } else {
      if (estado.x==estado.xo && estado.y==estado.yo) {
	estado=estadoAux;
      }
    }
  }

  void executaAcoes() {
    Tipo::acao acaoA, acaoB;

    for (;;) {


      /*
      else {
	if (kbhit()) {
	  if (getch()==27)
	    break;
	  else
	    avancoComTeclado=1;
	}

      }
      */
      numJogada++;
      numJogadaGeral++;
      // gotoxy(1,1); printf("%4d", numJogada);
      #ifdef JOGADAS_ALTERNADAS
      if (AJogaPrimeiro) {
	acaoA=escolheAcaoA();
	executaAcaoA(acaoA);
	acaoB=escolheAcaoB();
	executaAcaoB(acaoB);
      } else {
	acaoB=escolheAcaoB();
	executaAcaoB(acaoB);
	acaoA=escolheAcaoA();
	executaAcaoA(acaoA);
      }
      #else
      acaoA=escolheAcaoA();
      acaoB=escolheAcaoB();
      if (random()%2) {
	executaAcaoA(acaoA);
	executaAcaoB(acaoB);
      } else {
	executaAcaoB(acaoB);
	executaAcaoA(acaoA);
      }
      #endif


      if(numJogada == MAX_PASSOS_POR_JOGADA)
		  iniciaJogo();
	  else
		  if (bolaComA) {
	if (this->xa==5) {
	  recompensaA(60000);
	  //recompensaB(-10000);
	  golsA++;
	  golsA100++;
	  iniciaJogo();
	} else {
	  recompensaA(-10);
	  //recompensaB(0);
	}
      } else {
	if (this->xb==5) {
	  recompensaA(-(90000));
	  //recompensaB(10000);
	  golsB++;
	  golsB100++;
	  iniciaJogo();
	} else {
	  recompensaA(-10);
	  //recompensaB(0);
	}
      }
      //if (numJogadaGeral>=NUMJOGADAS) {
	  if (numPartidas>=NUMPARTIDAS) {
	break;
      }
    }
  }

  void recompensaA(float rew) {
    estado.comABola=bolaComA;
    estado.x=xa;
    estado.y=ya;
    estado.xo=4-xb;
    estado.yo=3-yb;
    t1->recompensa(estado, rew);
  }

  void recompensaB(float rew) {
    estado.comABola=!bolaComA;
    estado.xo=4-xa;
    estado.yo=3-ya;
    estado.x=xb;
    estado.y=yb;
    t2->recompensa(estado, rew);
  }

  ~Simulador(void) {
    delete t1;
    delete t2;
	fflush(evoltA);
	fclose(evoltA);
  }

};

#if (_TIME1==_Random || _TIME2==_Random)
  #include "Random.cpp"
#endif
#if (_TIME1==_Qlearning || _TIME2==_Qlearning)
  #include "Qlearnin.cpp"
#endif




int rodaTrial() {

  clrscr();
  Simulador *c=new Simulador(new TIME1(), new TIME2());
  c->mostraEstado();
  c->executaAcoes();
  delete c;
  return 0;
}

int main(void) {
	//for(loop = 0.0;loop<1.05;loop+=0.1){
		
		trial = 0;
		
		for(int m=0;m<sizeof(evol_data);m++){
			for(int n;n<sizeof(evol_data[m]);n++)
				evol_data[m][n] = 0;
		}

		for (int m=0;m<TRIALS;m++){
			evolgolsA[m] = 0;
			evolgolsB[m] = 0;
			evolganhasA[m] = 0;
			evolganhasB[m] = 0;
		}
		
		int t;

		int somagolsA=0, somagolsB=0, somaganhasA=0, somaganhasB =0;
		float mediagolsA, mediagolsB, mediaganhasA, mediaganhasB ;

		float desviogolsA=0, desviogolsB=0, desvioganhasA=0, desvioganhasB=0;


		// Executa as rodadas todas.
		for (trial = 0; trial < TRIALS; trial++)
			rodaTrial();

		//Escreve a evolucao.
		write_evol_data(loop);

		printf("\n\nResultado Final de %d trials.\n", TRIALS);


		// Calcula a soma e a media para imprimir a media

		for (t=0; t<TRIALS; t++)
			somagolsA += evolgolsA[t];
		mediagolsA = somagolsA/(double)TRIALS;

		for (t=0; t<TRIALS; t++)
			somagolsB += evolgolsB[t];
		mediagolsB = somagolsB/(double)TRIALS;

		for (t=0; t<TRIALS; t++)
			somaganhasA += evolganhasA[t];
		mediaganhasA = somaganhasA/(double)TRIALS;

		for (t=0; t<TRIALS; t++)
			somaganhasB += evolganhasB[t];
		mediaganhasB = somaganhasB/(double)TRIALS;


		// Calcula o desvio PG 16 DO OTAVIANO HELENE,eh o sigma ^ 2)


		for (t = 0; t < TRIALS; t++)
		  desviogolsA += pow((evolgolsA[t] - mediagolsA), 2);
		desviogolsA = sqrt(desviogolsA / (double) (TRIALS));

		for (t = 0; t < TRIALS; t++)
		  desviogolsB += pow((evolgolsB[t] - mediagolsB), 2);
		desviogolsB = sqrt(desviogolsB / (double) (TRIALS));

		for (t = 0; t < TRIALS; t++)
		  desvioganhasA += pow((evolganhasA[t] - mediaganhasA), 2);
		desvioganhasA = sqrt(desvioganhasA / (double) (TRIALS));

		for (t = 0; t < TRIALS; t++)
		  desvioganhasB += pow((evolganhasB[t] - mediaganhasB), 2);
		desvioganhasB = sqrt(desvioganhasB / (double) (TRIALS));


		printf("Total de gols Time A: %d\n", somagolsA);
		printf("Total de gols Time B: %d\n", somagolsB);
		printf("Total de vitorias Time A: %d\n", somaganhasA);
		printf("Total de vitorias Time B: %d\n", somaganhasB);

		printf("Media de gols Time A: %f desvio %f\n", mediagolsA, desviogolsA);
		printf("Media de gols Time B: %f desvio %f\n", mediagolsB, desviogolsB);
		printf("Media de vitorias Time A: %f desvio %f\n", mediaganhasA, desvioganhasA);
		printf("Media de vitorias Time B: %f desvio %f\n", mediaganhasB, desvioganhasB);
		printf("GOLS: (%d $\\pm$ %d) $\\times$ (%d $\\pm$ %d)\n", (int)mediagolsA, (int)desviogolsA, (int)mediagolsB, (int)desviogolsB);
		printf("Partidas: (%d $\\pm$ %d) $\\times$ (%d $\\pm$ %d)\n", (int)mediaganhasA, (int)desvioganhasA, (int)mediaganhasB, (int)desvioganhasB);

		printf("\nTerminado !!!\n");
	//}
	
	printf("ACABOU\n");

}
