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

      girar_derecha = false;
      bien_situado = false;
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    void PonerTerrenoEnMatriz(const vector<unsigned char> terreno, const state current_state, vector< vector< unsigned char> > &mapaResultado, const short nivel);
    
    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    bool es_casilla_interesante(const casilla c, const vector< vector< unsigned char> > &mapaResultado);
    bool es_casilla_desconocida(const casilla c, const vector< vector< unsigned char> > &mapaResultado);
    bool buscar_casillas_interesantes_perimetro(const unsigned char perimetro, const casilla posicion, const vector< vector<unsigned char>>& mapaResultado);
    void generar_mapa_potencial_parcial(const unsigned char radio, const casilla posicion, const vector< vector<unsigned char>>& mapaPotencial);
    template <typename T>
    void trasladar_mapa(vector< vector<T>>& base, const vector< vector<T>>& objetivo);

  private:
  // Declarar aquí las variables de estado
  state current_state; Orientacion brujula;
  Action last_action;
  bool girar_derecha, bien_situado;

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

};

#endif
