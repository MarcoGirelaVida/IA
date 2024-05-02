#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>


struct estado
{
  ubicacion jugador;
  ubicacion colaborador;
  Action ultima_orden_colaborador;

  bool operator==(const estado& otro) const
  {
    return (jugador == otro.jugador and colaborador.f == otro.colaborador.f and colaborador.c == otro.colaborador.c);
  }
};

struct nodo
{
  estado st;
  list<Action> secuencia;

  bool operator==(const nodo& otro) const
  {
    return (st == otro.st);
  }

  bool operator<(const nodo& otro) const
  {
    if (st.jugador.f < otro.st.jugador.f)
      return true;
    else if (st.jugador.f == otro.st.jugador.f and st.jugador.c < otro.st.jugador.c)
      return true;
    else if (st.jugador.f == otro.st.jugador.f and st.jugador.c == otro.st.jugador.c and st.jugador.brujula < otro.st.jugador.brujula)
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
      c_state = { {0, 0, norte}, {0, 0, norte}, actIDLE };
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR)
    {
      // Inicializar Variables de Estado
      hayPlan = false;
      c_state = { {0, 0, norte}, {0, 0, norte}, actIDLE };
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);


  private:
    // Declarar Variables de Estado
    estado c_state;
    ubicacion goal;
    list<estado> objetivos;
    list<Action> plan;

    bool hayPlan;

    pair <int, int> traductor_posicion(const ubicacion &pos, const int offset_fil, const int offset_col) const;
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    //template <typename T>
    void mostrar_lista(const list<nodo> &l) const;
    void PintaPlan(const list<Action> &plan) const;
    bool es_solucion (const ubicacion &ub, const ubicacion &final) const;
    void mostrar_ubicacion(const ubicacion &ub) const;
    void mostrar_estado(const estado &st) const;
    void mostrar_nodo(const nodo &nd) const;
    void accion_string (const Action &a) const;
    void orientacion_string (const Orientacion &o) const;
    void anula_matriz(vector<vector<unsigned char>> & matriz);
    
    // Implementación anchura un solo jugador
    bool casilla_transitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa, const ubicacion &ub_curr = {0,0}, const ubicacion &ub_colaborador = {0,0}, bool examinar_colaborador = false) const;
    ubicacion next_casilla(const ubicacion &pos, const int offset = 1) const;
    bool Find(const estado &item, const list<estado> &lista) const;
    bool Find(const estado &item, const list<nodo> &lista) const;
    estado apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const;
    bool anchura_solo_jugador (const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa);
    list<Action> anchura_solo_jugador_V2 (const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa);

};

#endif
