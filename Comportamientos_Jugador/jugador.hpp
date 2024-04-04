#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <unordered_map>
#include <unordered_set>
#include <queue>
using namespace std;

struct state{
  int fil;
  int col;
  Orientacion brujula;
};

const unsigned char walk_stepsize = 1, run_stepsize = 2;

typedef pair<unsigned char, unsigned char> casilla;

class ComportamientoJugador : public Comportamiento
{
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de estado
      current_state.fil = 99;
      current_state.col = 99;
      current_state.brujula = norte;

      ubicado = false;

      mapa_resultado_temporal[mapaResultado.size()*2][mapaResultado.size()*2];
      mapa_actual = &mapa_resultado_temporal;

      fake_state.fil = mapa_resultado_temporal.size() / 2;
      fake_state.col = fake_state.fil;
      fake_state.brujula = norte;
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    void PonerTerrenoEnMatriz(const vector<unsigned char> terreno, const state current_state, vector< vector< unsigned char> > &mapaResultado, const short nivel);
    
    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    bool es_casilla_interesante(const casilla c);
    bool es_casilla_desconocida(const casilla c);
    bool buscar_casillas_interesantes_perimetro(const unsigned char perimetro);
    void generar_mapa_potencial_parcial(const unsigned char radio);
    template <typename T>
    void trasladar_mapa(vector< vector<T>>& base, const vector< vector<T>>& objetivo);

  private:
  // Declarar aquí las variables de estado
  state current_state, fake_state; Orientacion brujula;
  Action last_action;
  bool ubicado;

/*
  struct PairHash {
    size_t operator()(const casilla& p) const {
      return hash<unsigned char>()(p.first) ^ hash<unsigned char>()(p.second);
    }
  };

  unordered_map<casilla, unsigned char, PairHash> casillas_interesantes;
  unordered_set<casilla> casillas_desconocidas;
*/
  queue<pair<casilla, unsigned char>> casillas_interesantes;
  queue<casilla> casillas_desconocidas;
  vector< vector< unsigned char> > mapa_resultado_temporal;
  vector< vector< unsigned short> > mapa_potencial_temporal;
  vector< vector< unsigned short> > mapa_potencial;
  vector< vector< unsigned short> > mapa_decision;

  vector< vector< unsigned char> > *mapa_actual;
};

#endif
