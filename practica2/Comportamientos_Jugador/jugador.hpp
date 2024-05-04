#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <queue>
#include <set>
#include <limits>

struct estado
{
  ubicacion jugador;
  ubicacion colaborador;
  Action ultima_orden_colaborador;

  bool operator==(const estado& otro) const
  {
    return (jugador == otro.jugador and colaborador == otro.colaborador and ultima_orden_colaborador == otro.ultima_orden_colaborador);
    //return (jugador.f == otro.jugador.f and jugador.c == otro.jugador.c and colaborador.f == otro.colaborador.f and colaborador.c == otro.colaborador.c);
    //return (jugador == otro.jugador and colaborador.f == otro.colaborador.f and colaborador.c == otro.colaborador.c);
  }

  bool operator!=(const estado& otro) const
  {
    return !(*this == otro);
  }
};


struct nodo
{
  unsigned short coste_acumulado;
  estado st;
  queue<Action> secuencia;
  bool zapatillas, bikini;

  nodo() : coste_acumulado(0), bikini(false), zapatillas(false) {}

  bool operator==(const nodo& otro) const
  {
    return (st == otro.st);
  }

  bool operator<(const nodo& otro) const
  {
    if (coste_acumulado < otro.coste_acumulado)
      return true;
    else if (coste_acumulado == otro.coste_acumulado and st.jugador.f < otro.st.jugador.f)
      return true;
    else if (coste_acumulado == otro.coste_acumulado and st.jugador.f == otro.st.jugador.f and st.jugador.c < otro.st.jugador.c)
      return true;
    else if (coste_acumulado == otro.coste_acumulado and st.jugador.f == otro.st.jugador.f and st.jugador.c == otro.st.jugador.c and st.jugador.brujula < otro.st.jugador.brujula)
      return true;
    else if (coste_acumulado == otro.coste_acumulado and st.jugador.f == otro.st.jugador.f and st.jugador.c == otro.st.jugador.c and st.jugador.brujula == otro.st.jugador.brujula and st.colaborador.f < otro.st.colaborador.f)
      return true;
    else if (coste_acumulado == otro.coste_acumulado and st.jugador.f == otro.st.jugador.f and st.jugador.c == otro.st.jugador.c and st.jugador.brujula == otro.st.jugador.brujula and st.colaborador.f == otro.st.colaborador.f and st.colaborador.c < otro.st.colaborador.c)
      return true;
    else if (coste_acumulado == otro.coste_acumulado and st.jugador.f == otro.st.jugador.f and st.jugador.c == otro.st.jugador.c and st.jugador.brujula == otro.st.jugador.brujula and st.colaborador.f == otro.st.colaborador.f and st.colaborador.c == otro.st.colaborador.c and st.colaborador.brujula < otro.st.colaborador.brujula)
      return true;
    else
      return false;
  }

  bool operator>(const nodo& otro) const
  {
    return !(*this < otro) and !(*this == otro);
  }

};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size)
    {
      // Inicializar Variables de Estado
      hayPlan = false;
      hijos_explorados = nodos_abiertos = nodos_cerrados = consumo_total_bateria = 0;
      c_state = { {0, 0, norte}, {0, 0, norte}, act_CLB_STOP };
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR)
    {
      // Inicializar Variables de Estado
      hayPlan = false;
      hijos_explorados = nodos_abiertos = nodos_cerrados = consumo_total_bateria = 0;
      c_state = { {0, 0, norte}, {0, 0, norte}, act_CLB_STOP };
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

    bool hayPlan;

    ubicacion traductor_posicion(const ubicacion &pos, const int offset_fil, const int offset_col) const;
    void VisualizaPlan(const estado &st, const queue<Action> &plan);
    queue<nodo> generar_nodos_secuencia(const queue<Action> &plan, const estado &inicio);
    //template <typename T>
    void mostrar_lista(const queue<nodo> &q, bool completa = false) const;
    void mostrar_lista(const set<nodo> &q, bool completa = false) const;
    void mostrar_lista(const priority_queue<nodo, vector<nodo>, greater<nodo>> &q, bool completa) const;
    void PintaPlan(const queue<Action> &plan) const;
    bool es_solucion (const estado &st, const ubicacion &final, const unsigned char nivel = 1) const;
    void mostrar_ubicacion(const ubicacion &ub) const;
    void mostrar_estado(const estado &st, const unsigned char nivel) const;
    void mostrar_nodo(const nodo &nd, const unsigned char nivel, bool mostrar_secuencia = false) const;
    void accion_string (const Action &a) const;
    void orientacion_string (const Orientacion &o) const;
    void anula_matriz(vector<vector<unsigned char>> & matriz);
    void registrar_estado(const Sensores &sensores, estado &c_state, ubicacion &goal);
    
    //--------------------------------------------------------------------------------
    void generar_hijo(const nodo &padre, const Action a, nodo &hijo, const vector<vector<unsigned char>> &mapa, const unsigned char nivel = 0) const;
    unsigned short coste_casilla(const Action a, const nodo &n, const vector<vector<unsigned char>> &mapa) const;
    bool esta_en_rango_vision(const estado &st) const;
    bool casilla_transitable(const estado &cst, const vector<vector<unsigned char>> &mapa, bool colaborador = false) const;
    ubicacion next_casilla(const ubicacion &pos) const;
    bool Find(const estado &item, const queue<estado> &lista) const;
    bool Find(const estado &item, const queue<nodo> &lista) const;
    bool actualizar_frontera(const estado &item, priority_queue<nodo> &cola) const;
    estado apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const;
    estado apply_seguro(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const;
    queue<Action> nivel_2(const estado &jugador, const ubicacion &destino, const vector<vector<unsigned char>> &mapa);
    queue<Action> nivel_0_1(const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa, const unsigned char nivel = 0);

};

#endif
