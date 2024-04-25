#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct ubicacion {
  int f;
  int c;
  Orientacion brujula;
  
  bool operator==(const ubicacion &ub) const {
    return (f == ub.f and c == ub.c and brujula == ub.brujula);
  }
};
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
};


class ComportamientoJugador : public Comportamiento
{
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size)
    {
      hayPlan = false;
      estado estado_actual = { {0, 0, norte}, {0, 0, norte}, actIDLE };
      /*
      // Inicializar Variables de Estado
      // Inicializo el estado actual
      actual.fila = 0;
      actual.columna = 0;
      actual.orientacion = 0;

      // Inicializo el vector de objetivos
      objetivos.push_back(actual);

      // Inicializo el plan
      plan.clear();
      hayPlan = false;

      // Inicializo el mapaConPlan
      mapaConPlan.resize(200, vector<unsigned char>(200, 0));

      // Inicializo el mapaResultado
      mapaResultado.resize(200, vector<unsigned char>(200, '?'));

      // Inicializo el mapaVisitados
      mapaVisitados.resize(200, vector<unsigned char>(200, 0));
      */
    }

    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR)
    {
      // Inicializar Variables de Estado
      hayPlan = false;
      estado estado_actual = { {0, 0, norte}, {0, 0, norte}, actIDLE };
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    estado actual, objetivo;
    list<estado> objetivos;
    list<Action> plan;

    bool hayPlan;

    bool esta_en_mapa(const int fil, const int col) const;
    bool esta_en_mapa(const estado &st) const;
    bool es_solucion(const nodo &n) const;
    bool casilla_valida(const unsigned char tile) const;
    pair <int, int> traductor_posicion(const int offset_fil, const int offset_col) const;
    void registra_secuencia(unsigned char decision, nodo &n) const;
    list<nodo> acciones_validas(const nodo& n) const;
    list<Action> breadth_1st_search();

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);

    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);

};

#endif
