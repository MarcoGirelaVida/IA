#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <unordered_map>
#include <set>
#include <map>
#include <queue>
#include <iostream>
#include <stack>
#include <vector>
#include <thread>
using namespace std;

struct state{
  int fil;
  int col;
  Orientacion brujula;
};


const unsigned char walk_stepsize = 1, run_stepsize = 2;

typedef pair<unsigned short, unsigned short> casilla;

struct PairHash {
    std::size_t operator()(const std::pair<unsigned short, unsigned short>& p) const {
        // Combine the hashes of the two unsigned shorts to create a hash for the pair
        return std::hash<unsigned short>()(p.first) ^ (std::hash<unsigned short>()(p.second) << 1);
    }
};

class ComportamientoJugador : public Comportamiento
{
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      zapatillas = false;
      bikini = false;
      recargador_encontrado = false;

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
      recargando = false;
      porcentaje_bateria = 100;
      bateria = bateria_maxima;
      ciclos_desde_ultima_recarga = 0;

      mapa_resultado_temporal = vector< vector< unsigned char> >(mapa_size*3, vector<unsigned char>(mapa_size*3, '?'));
      mapa_potencial_temporal = vector<vector<int>>(mapa_size*3, vector<int>(mapa_size*3, 0));
      mapa_potencial          = vector<vector<int>>(mapa_size, vector<int>(mapa_size, 0));

      mapa_actual = &mapa_resultado_temporal;
      mapa_potencial_actual = &mapa_potencial_temporal;
      
      casillas_interesantes = &casillas_interesantes_falsas;
      poner_bordes_en_matriz();
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    void PonerTerrenoEnMatriz(Sensores sensor);
    
    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    casilla casilla_equivalente(pair<char, char> equivalente);
    template <typename T>
    static bool esta_dentro_de_mapa(const casilla c, const vector<vector<T>> &mapa);
    bool escanear_casilla(const casilla c);
    bool escanear_perimetro(const unsigned char perimetro);
    void generar_mapa_decision(unsigned char *radio);
    float media_entorno(const casilla c);
    void calcular_efecto_entorno(const casilla c, const short offsetfil, const short offsetcol);
    template <typename T>
    void trasladar_mapa(vector< vector<T>>& base, vector< vector<T>>& objetivo, bool mapa_resultado);
    template <typename T>
    static void rotar_mapa_derecha(vector<vector<T>> &mapaOriginal, vector<vector<T>> &mapaGirado);
    static casilla rotar_casilla(const casilla c, const size_t size);
    template <typename T>
    void printea_matriz(const vector<vector<T>>& matriz, int radio = 0);
    void printea_cola(const queue<casilla>& queue);
    void printear_estado_general(int opcion);
    Action decision_optima(size_t decision);
    float penalizacion_decision(size_t decision);
    void poner_bordes_en_matriz();
    void generar_mapa_decision_ponderado();
    bool es_casilla_interesante(const casilla c);
    void actualizar_ultima_accion(Sensores sensores);
    void printea_set(set<casilla>& set);

  private:
  // Declarar aquí las variables de estado
  state current_state, fake_state, *estado; Orientacion brujula;
  Action last_action;
  bool ubicado, zapatillas, bikini;
  const unsigned short mapa_size = mapaResultado.size();
  unsigned char recarga;
  int prioridad_recarga;
  unsigned porcentaje_bateria, bateria_maxima;
  unsigned bateria;
  float mejor_decision;
  casilla recargador;
  bool recargador_encontrado;
  bool recargando;
  size_t ciclos_desde_ultima_recarga;

  set<casilla> *casillas_interesantes;
  set<casilla> casillas_interesantes_reales;
  set<casilla> casillas_interesantes_falsas;
  map<casilla, char> casillas_agentes;
  queue<casilla> casillas_desconocidas;
  stack<Action> cache_acciones;

  vector<vector<unsigned char>> mapa_resultado_temporal;
  vector<vector<int>> mapa_potencial_temporal;
  vector<vector<int>> mapa_potencial;
  vector<vector<int>> mapa_decision;
  vector<vector<float>> mapa_decision_ponderado;
  vector<vector<unsigned char>> *mapa_actual;
  vector<vector<int>> *mapa_potencial_actual;
  vector<vector<float>> mapa_decision_definitivo;
  const vector<int> prioridad_opciones_vertical = {1, 2, 3, 0, 5, 6, 8, 7, 4};
  const vector<int> prioridad_opciones_diagonal = {2, 5, 0, 1, 8, 3, 7, 6, 4};
  const vector<int> *prioridad_opciones;

  vector<std::thread> threads;
};

#endif
