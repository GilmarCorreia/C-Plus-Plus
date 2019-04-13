#define MAXFLOAT 200000000

class Random: public Time {
  public: virtual void inicioDeJogo(void) {
  }

  public: virtual Tipo::acao acao(Tipo::estado estado) {
    return (Tipo::acao)(random()%5);
  }

  public: virtual void recompensa(Tipo::estado estado, float rew) {
  }

  public: virtual ~Random(void) {
  }
};
