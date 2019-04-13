#define MAXFLOAT 200000000

class Qa {
  FILE *arq;
  public: float Q[Tipo::NUM_ACOES];
  public: float V;

  public: Qa(FILE *arq) {
    this->arq=arq;
    #ifdef TREINANDO
    for (int i=0; i<Tipo::NUM_ACOES; i++) {
      Q[i] = (random()%100)/100.0; // 1.0;
    }
    V=1.0;
    #else
    fread(Q, sizeof Q, 1, arq);
    fread(&V, sizeof V, 1, arq);
    #endif
  }

  public: void atualizaQ(int a, float rew, float alpha, float gamma, float VSLinha) {
	  
	// Q-Learning   

	//float max = Q[0];
	//for (int i = 1; i < Tipo::NUM_ACOES; i++){
	//	if (max < Q[i])
	//		max = Q[i];
	//}

	//Q[a] = Q[a] + (alpha*(rew + gamma * (max) - Q[a] ));


	// SARSA
  
	Q[a] = Q[a] + (alpha*(rew+gamma*(VSLinha)-Q[a]));
	//Q[a] = Q[a] + (alpha*(rew+gamma*(VSLinha-Q[a])));
	//Q[a] = Q[a] + (alpha*(rew+gamma*(VSLinha)));

  }

  public: Tipo::acao calculaAcao(void) {
    float min, max;
    Tipo::acao amMelhor;
    max=-MAXFLOAT;
	
	max = Q[0];
	amMelhor = (Tipo::acao)(0);

	for (int a = 1; a < Tipo::NUM_ACOES; a++) {
		if (Q[a] > max) {
			max = Q[a];
			amMelhor = (Tipo::acao)(a);
		}
	}

    V = max;
    return amMelhor;

    //return (Tipo::acao)(random()%5); //apagar essa linha

  }

  public: ~Qa(void) {
    static long l;
    #ifdef TREINANDO
    l=fwrite(Q, sizeof Q, 1, arq);
    l=fwrite(&V, sizeof V, 1, arq);
    #endif
  }
};

class Qlearning: public Time {
  Qa *S[4][5][4][5][2]; //tamanho
  float alpha;
  float gamma;
  float explor;
  float decay;
  float acaoUsada;
  Tipo::estado SAntesEscolhaAcao;
  FILE *arq;

  
  public: Qlearning(void) {
	  
    decay=0.9999954;
    gamma=0.3;

    #ifdef TREINANDO
		alpha=0.1;
		explor=0.3;
		
		//printf("EXPLORAÇÃO: %f", explor);
		
		if ((arq=fopen("QQ.dat", "wb"))==NULL) {
		  printf("O arquivo QQ.dat nao pode ser iniciado.\n");
		  exit(0);
		}
    #else
		alpha=0.0;
		explor=0.0;
		if ((arq=fopen("QQ.dat", "rb"))==NULL) {
		  printf("O arquivo QQ.dat nao pode ser aberto.\n");
		  exit(0);
		}
    #endif
	
    for (int ya=0; ya<4; ya++) {
		for (int xa=0; xa<5; xa++) {
			for (int yb=0; yb<4; yb++) {
				for (int xb=0; xb<5; xb++) {
					for (int comABola=0; comABola<2; comABola++) {
						S[ya][xa][yb][xb][comABola]=new Qa(arq);
						if (S[ya][xa][yb][xb][comABola]==NULL) {
							printf("Nao existe memoria suficiente.\n");
							exit(0);
						}
					}
				}
			}
		}
    }
  }

  public: virtual void inicioDeJogo() {
  }

  public: virtual Tipo::acao acao(Tipo::estado estado) {
    SAntesEscolhaAcao=estado;
    acaoUsada=S[estado.y][estado.x]
	       [estado.yo][estado.xo][estado.comABola]->calculaAcao();
    alpha*=decay;
    if ((random()%100)/100.0>explor)
      return (Tipo::acao)acaoUsada;
    else
      return (Tipo::acao)(acaoUsada=(random()%5));
  }

  public: virtual void recompensa(Tipo::estado estado, float rew) {
    float VSLinha;

    int dxo=SAntesEscolhaAcao.xo-estado.xo;
    int dyo=SAntesEscolhaAcao.yo-estado.yo;

    if (estado.x<0 || estado.x>4)	//algum tipo de gol
      VSLinha=rew;
    else
      VSLinha=S[estado.y][estado.x][estado.yo][estado.xo][estado.comABola]->V;
    S[SAntesEscolhaAcao.y][SAntesEscolhaAcao.x]
     [SAntesEscolhaAcao.yo][SAntesEscolhaAcao.xo]
     [SAntesEscolhaAcao.comABola]->
	atualizaQ(acaoUsada, rew, alpha, gamma, VSLinha);
  }

  public: virtual ~Qlearning(void) {
    for (int ya=0; ya<4; ya++) {
      for (int xa=0; xa<5; xa++) {
	for (int yb=0; yb<4; yb++) {
	  for (int xb=0; xb<5; xb++) {
	    for (int comABola=0; comABola<2; comABola++) {
	      delete S[ya][xa][yb][xb][comABola];
	    }
	  }
	}
      }
    }
    fclose(arq);
  }
};
