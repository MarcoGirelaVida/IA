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
      zapatillas = false;
      bikini = false;

      current_state.fil = 99;
      current_state.col = 99;
      current_state.brujula = norte;

      fake_state.fil = mapa_size / 2;
      fake_state.col = fake_state.fil;
      fake_state.brujula = norte;

      estado = &fake_state;
      ubicado = false;
      prioridad_recarga = -1;
      bateria_maxima = 15000;
      recarga = 0;

      mapa_resultado_temporal = vector< vector< unsigned char> >(mapa_size*2, vector<unsigned char>(mapa_size, '?'));
      mapa_potencial_temporal = vector<vector<int>>(mapa_size*2, vector<int>(mapa_size, 1));
      mapa_potencial =          vector<vector<int>>(mapa_size, vector<int>(mapa_size, 1));

      mapa_actual = &mapa_resultado_temporal;
      mapa_potencial_actual = &mapa_potencial_temporal;
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    void PonerTerrenoEnMatriz(const vector<unsigned char> terreno, const state current_state, vector< vector< unsigned char> > &mapaResultado, const short nivel);
    
    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    bool escanear_casilla(const casilla c);
    bool escanear_perimetro(const unsigned char perimetro);
    void generar_mapa_decision(const unsigned char radio, Sensores sensor);
    void calcular_efecto_entorno(const casilla c, const int bateria);
    template <typename T>
    void trasladar_mapa(vector< vector<T>>& base, const vector< vector<T>>& objetivo);

  private:
  // Declarar aquí las variables de estado
  state current_state, fake_state, *estado; Orientacion brujula;
  Action last_action;
  bool ubicado, zapatillas, bikini;
  const unsigned short mapa_size = mapaResultado.size();
  unsigned char recarga;
  int prioridad_recarga, bateria_maxima;

  queue<casilla> casillas_interesantes;
  queue<casilla> casillas_desconocidas;

  vector<vector<unsigned char>> mapa_resultado_temporal;
  vector<vector<int>> mapa_potencial_temporal;
  vector<vector<int>> mapa_potencial;
  vector<vector<int>> mapa_decision;
  vector<vector<unsigned char>> *mapa_actual;
  vector<vector<int>> *mapa_potencial_actual;
};

#endif
