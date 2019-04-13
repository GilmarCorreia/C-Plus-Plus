class Tipo {
  public: typedef enum {stand, N, S, E, W, NUM_ACOES} acao;
  public: typedef
	    struct {
	      int x, y;		//posicao do jogador do time
	      int xo, yo;       //posicao do jogador oponente
	      char comABola;
	    } estado;
};

class Time {
  public: virtual void inicioDeJogo(void) {
  }

  public: virtual Tipo::acao acao(Tipo::estado estado) {
    return Tipo::stand;
  }

  public: virtual void recompensa(Tipo::estado estado, float rew) {
  }

  public: virtual ~Time(void) {
  }
};

