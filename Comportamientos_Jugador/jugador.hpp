#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
using namespace std;

struct state{
  int fil;
  int col;
  Orientacion brujula;
};

const unsigned char walk_stepsize = 1, run_stepsize = 2;

typedef pair<unsigned short, unsigned short> casilla;

class ComportamientoJugador : public Comportamiento
{
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      zapatillas = false;
      bikini = false;

      current_state.fil = 99;
      current_state.col = 99;
      current_state.brujula = norte;

      fake_state.fil = (mapa_size*3) / 2;
      fake_state.col = fake_state.fil;
      fake_state.brujula = norte;

      estado = &fake_state;
      ubicado = false;
      prioridad_recarga = -1;
      bateria_maxima = 5000;
      recarga = 0;
      porcentaje_bateria = 100;
      bateria = bateria_maxima;

      mapa_resultado_temporal = vector< vector< unsigned char> >(mapa_size*3, vector<unsigned char>(mapa_size*3, '?'));
      mapa_potencial_temporal = vector<vector<int>>(mapa_size*3, vector<int>(mapa_size*3, 0));
      mapa_potencial          = vector<vector<int>>(mapa_size, vector<int>(mapa_size, 0));

      mapa_actual = &mapa_resultado_temporal;
      mapa_potencial_actual = &mapa_potencial_temporal;
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    void PonerTerrenoEnMatriz(Sensores sensor);
    
    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    casilla casilla_equivalente(casilla Equivalente);
    bool escanear_casilla(const casilla c);
    bool escanear_perimetro(const unsigned char perimetro);
    void generar_mapa_decision(unsigned char *radio);
    void calcular_efecto_entorno(const casilla c, const short offsetfil, const short offsetcol);
    template <typename T>
    void trasladar_mapa(const vector< vector<T>>& base, vector< vector<T>>& objetivo);
    template <typename T>
    void rotar_mapa_derecha(const vector<vector<T>> &base, vector<vector<T>> &objetivo);
    template <typename T>
    void printea_matriz(const vector<vector<T>>& matriz);
    void printea_cola(const queue<casilla>& queue);
    void printear_estado_general();
    Action decision_optima(const size_t decision);
    int penalizacion_decision(const size_t decision);

  private:
  // Declarar aquí las variables de estado
  state current_state, fake_state, *estado; Orientacion brujula;
  Action last_action;
  bool ubicado, zapatillas, bikini;
  const unsigned short mapa_size = mapaResultado.size();
  unsigned char recarga;
  int prioridad_recarga, bateria_maxima;
  unsigned porcentaje_bateria;
  unsigned bateria;

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
