#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <unordered_map>
#include <queue>
using namespace std;

struct state{
  int fil;
  int col;
  Orientacion brujula;
};

const unsigned short walk_stepsize = 1, run_stepsize = 2;
class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de estado
      current_state.fil = 99;
      current_state.col = 99;
      current_state.brujula = norte;

      girar_derecha = false;
      bien_situado = false;

      for (size_t i = 0; i < mapaResultado.size(); i++)
        for (size_t j = 0; j < mapaResultado.size(); j++)
          if (mapaResultado[i][j] == '?')
            casillas_objetivo.insert({{i, j}, '?'});
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    void PonerTerrenoEnMatriz(const vector<unsigned char> terreno, const state current_state, vector< vector< unsigned char> > &mapaResultado, const short nivel);
    
    Action think(Sensores sensores);
    int interact(Action accion, int valor);

  private:
  // Declarar aquí las variables de estado
  state current_state; Orientacion brujula;
  Action last_action;
  bool girar_derecha, bien_situado;

  struct PairHash {
    size_t operator()(const std::pair<unsigned int, unsigned int>& p) const {
      // Combine hash values using bitwise XOR or other suitable method
      return std::hash<unsigned int>()(p.first) ^ std::hash<unsigned int>()(p.second);
    }
  };

  std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned char, PairHash> casillas_objetivo;
};
#endif
