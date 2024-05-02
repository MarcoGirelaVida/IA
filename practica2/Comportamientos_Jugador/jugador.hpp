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
    void anula_matriz(vector<vector<unsigned char>> & matriz);
    // Implementación anchura un solo jugador
    bool casilla_transitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa) const;
    ubicacion next_casilla(const ubicacion &pos, const int offset = 1) const;
    bool Find(const estado &item, const list<estado> &lista) const;
    estado apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const;
    bool anchura_solo_jugador (const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa);


};

#endif
