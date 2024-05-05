#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <queue>
#include <set>
#include <limits>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

static const vector<Action> acciones_jugador = {actIDLE, actWALK, actRUN, actTURN_SR, actTURN_L};
static const vector<Action> acciones_jugador_colaborador = {actIDLE, actWALK, actRUN, actTURN_SR, actTURN_L, act_CLB_WALK, act_CLB_TURN_SR, act_CLB_STOP};

struct estado
{
  ubicacion jugador;
  ubicacion colaborador;
  Action ultima_orden_colaborador;
  bool zapatillas, bikini, zapatillas_colab, bikini_colab;

  estado() : bikini(false), zapatillas(false), bikini_colab(false), zapatillas_colab(false), ultima_orden_colaborador(act_CLB_STOP) {}

  bool operator==(const estado& otro) const
  {
    //return (jugador == otro.jugador and colaborador == otro.colaborador and ultima_orden_colaborador == otro.ultima_orden_colaborador);
    //return (jugador.f == otro.jugador.f and jugador.c == otro.jugador.c and colaborador.f == otro.colaborador.f and colaborador.c == otro.colaborador.c);
    return (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab == otro.bikini_colab and zapatillas_colab == otro.zapatillas_colab);
  }

  bool operator!=(const estado& otro) const
  {
    return !(*this == otro);
  }

  bool operator<(const estado& otro) const
  {
    if (jugador.f < otro.jugador.f)
      return true;
    else if (jugador.f == otro.jugador.f and jugador.c < otro.jugador.c)
      return true;
    else if (jugador.f == otro.jugador.f and jugador.c == otro.jugador.c and jugador.brujula < otro.jugador.brujula)
      return true;
    else if (jugador == otro.jugador and colaborador.f < otro.colaborador.f)
      return true;
    else if (jugador == otro.jugador and colaborador.f == otro.colaborador.f and colaborador.c < otro.colaborador.c)
      return true;
    else if (jugador == otro.jugador and colaborador.f == otro.colaborador.f and colaborador.c == otro.colaborador.c and colaborador.brujula < otro.colaborador.brujula)
      return true;
    else if (jugador == otro.jugador and colaborador == otro.colaborador and bikini < otro.bikini)
      return true;
    else if (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas < otro.zapatillas)
      return true;
    else if (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab < otro.bikini_colab)
      return true;
    else if (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab == otro.bikini_colab and zapatillas_colab < otro.zapatillas_colab)
      return true;
    else
      return false;
  }

  bool operator>(const estado& otro) const
  {
    return !(*this < otro) and !(*this == otro);
  }
  
  struct hash_estado
  {
    size_t operator()(const estado& st) const
    {
      return hash<int>()(st.jugador.f) ^ hash<int>()(st.jugador.c) ^ hash<int>()(st.jugador.brujula) ^ hash<int>()(st.colaborador.f) ^ hash<int>()(st.colaborador.c) ^ hash<int>()(st.colaborador.brujula) ^ hash<bool>()(st.bikini) ^ hash<bool>()(st.zapatillas) ^ hash<bool>()(st.bikini_colab) ^ hash<bool>()(st.zapatillas_colab);
    }
  };
};

struct nodo
{
  unsigned short coste;
  estado st;
  queue<Action> secuencia;

  nodo() : coste(0) {}

  bool operator==(const nodo& otro) const
  {
    return (st == otro.st);
  }

  bool operator<(const nodo& otro) const
  {
    if (st < otro.st)
      return true;
    else
      return false;
  }

  bool operator>(const nodo& otro) const
  {
    return !(*this < otro) and !(*this == otro);
  }

  struct mayor_coste
  {
    bool operator()(const nodo& uno, const nodo& otro) const
    {
      return (uno.coste > otro.coste);
    }
  };

};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size)
    {
      // Inicializar Variables de Estado
      hayPlan = false;
      paso = hijos_explorados = nodos_abiertos = nodos_cerrados = consumo_total_bateria = 0;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR)
    {
      // Inicializar Variables de Estado
      hayPlan = false;
      paso = hijos_explorados = nodos_abiertos = nodos_cerrados = consumo_total_bateria = 0;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);


  private:
    // Declarar Variables de Estado
    size_t hijos_explorados, nodos_abiertos, nodos_cerrados, consumo_total_bateria;
    estado c_state;
    ubicacion goal;
    queue<Action> plan, plan_colaborador;
    queue<nodo> plan_nodos;
    unsigned short paso;

    bool hayPlan;

    ubicacion traductor_posicion(const ubicacion &pos, const int offset_fil, const int offset_col) const;
    void VisualizaPlan(const estado &st, const queue<Action> &plan);
    queue<nodo> generar_nodos_secuencia(const queue<Action> &plan, const estado &inicio);
    void mostrar_lista(const queue<nodo> &q, bool completa = false) const;
    void mostrar_lista(const set<nodo> &q, bool completa = false) const;
    void mostrar_lista(const priority_queue<nodo, vector<nodo>, greater<nodo>> &q, bool completa) const;
    void PintaPlan(const queue<Action> &plan) const;
    bool es_solucion (const estado &st, const ubicacion &final, const unsigned char nivel) const;
    void mostrar_ubicacion(const ubicacion &ub) const;
    void mostrar_estado(const estado &st, const unsigned char nivel) const;
    void mostrar_nodo(const nodo &nd, const unsigned char nivel, bool mostrar_secuencia = false) const;
    void accion_string (const Action &a) const;
    void orientacion_string (const Orientacion &o) const;
    void anula_matriz(vector<vector<unsigned char>> & matriz);
    void registrar_estado(const Sensores &sensores, estado &c_state, ubicacion &goal);
    
    //--------------------------------------------------------------------------------
    void generar_hijo(const nodo &padre, const Action a, nodo &hijo, const vector<vector<unsigned char>> &mapa, const unsigned char nivel) const;
    unsigned short coste_accion(const Action a, const estado &st, const vector<vector<unsigned char>> &mapa) const;
    unsigned short coste_accion_total(const Action a, const estado &st, const vector<vector<unsigned char> > &mapa) const;
    bool esta_en_rango_vision(const estado &st) const;
    bool casilla_transitable(const estado &cst, const vector<vector<unsigned char>> &mapa, bool colaborador) const;
    ubicacion next_casilla(const ubicacion &pos) const;
    bool actualizar_frontera(const estado &item, priority_queue<nodo> &cola) const;
    estado apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const;
    estado apply_seguro(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const;
    queue<Action> nivel_2_3(const estado &jugador, const ubicacion &destino, const vector<vector<unsigned char>> &mapa, const unsigned char nivel);
    queue<Action> nivel_0_1(const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa, const unsigned char nivel);

};

#endif
