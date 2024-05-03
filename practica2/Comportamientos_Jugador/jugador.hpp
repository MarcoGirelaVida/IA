#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <queue>

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
  size_t coste_acumulado;
  estado st;
  queue<Action> secuencia;

  nodo() : coste_acumulado(0) {}

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
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size)
    {
      // Inicializar Variables de Estado
      hayPlan = false;
      c_state = { {0, 0, norte}, {0, 0, norte}, act_CLB_STOP };
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR)
    {
      // Inicializar Variables de Estado
      hayPlan = false;
      c_state = { {0, 0, norte}, {0, 0, norte}, act_CLB_STOP };
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);


  private:
    // Declarar Variables de Estado
    estado c_state;
    ubicacion goal, ubicacion_recargador;
    queue<Action> plan, plan_colaborador;

    bool hayPlan, bikini, zapatillas, recargador_encontrado;

    ubicacion traductor_posicion(const ubicacion &pos, const int offset_fil, const int offset_col) const;
    void VisualizaPlan(const estado &st, const queue<Action> &plan);
    queue<nodo> generar_nodos_secuencia(const queue<Action> &plan) const;
    //template <typename T>
    void mostrar_lista(const queue<nodo> &q, bool completa = false) const;
    void mostrar_lista(const priority_queue<nodo> &q, bool completa = false) const;
    void PintaPlan(const queue<Action> &plan) const;
    bool es_solucion (const estado &st, const ubicacion &final, bool colaborador = true) const;
    void mostrar_ubicacion(const ubicacion &ub) const;
    void mostrar_estado(const estado &st) const;
    void mostrar_nodo(const nodo &nd, bool mostrar_secuencia = true) const;
    void accion_string (const Action &a) const;
    void orientacion_string (const Orientacion &o) const;
    void anula_matriz(vector<vector<unsigned char>> & matriz);
    void registrar_estado(const Sensores &sensores, estado &c_state, ubicacion &goal);
    
    // Implementación anchura un solo jugador
    int coste_casilla(const Action &a, const ubicacion &ub, const vector<vector<unsigned char>> &mapa);
    bool esta_en_rango_vision(const estado &st) const;
    bool casilla_transitable(const estado &cst, const vector<vector<unsigned char>> &mapa, bool colaborador = false) const;
    ubicacion next_casilla(const ubicacion &pos) const;
    bool Find(const estado &item, const queue<estado> &lista) const;
    bool Find(const estado &item, const queue<nodo> &lista) const;
    bool actualizar_frontera(const estado &item, priority_queue<nodo> &cola) const;
    estado apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const;
    estado apply_seguro(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const;
    queue<Action> nivel_1(const estado &jugador, const ubicacion &destino, const vector<vector<unsigned char>> &mapa);
    queue<Action> nivel_2(const estado &jugador, const ubicacion &destino, const vector<vector<unsigned char>> &mapa);
    queue<Action> buscar_objetivo_anchura(const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa, bool colaborador = false);

};

#endif
