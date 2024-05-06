#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <queue>
#include <set>
#include <vector>
#include <iostream>
#include <cmath>
#include <list>
#include <map>
#include <unordered_set>
#include <stack>

static const vector<Action> acciones_jugador = {actWALK, actRUN, actTURN_L, actTURN_SR, actIDLE};
static const vector<Action> acciones_jugador_colaborador = {actWALK, actRUN, actTURN_L, actTURN_SR, act_CLB_WALK, act_CLB_TURN_SR, act_CLB_STOP, actIDLE};
struct nodo_accion
{
  const Action accion;
  const nodo_accion* padre;
  nodo_accion() : accion(actIDLE), padre(nullptr) {}
  nodo_accion(const Action a, const nodo_accion* p) : accion(a), padre(p) {}
  nodo_accion& operator=(const nodo_accion& otro)
  {
    if (this != &otro)
    {
      const_cast<Action&>(accion) = otro.accion;
      const_cast<nodo_accion*&>(padre) = const_cast<nodo_accion*>(otro.padre);
    }
    return *this;
  }
};
struct estado
{
  ubicacion jugador, colaborador;
  Action ultima_orden_colaborador;
  bool zapatillas, bikini, zapatillas_colab, bikini_colab;

  estado() : bikini(false), zapatillas(false), bikini_colab(false), zapatillas_colab(false), ultima_orden_colaborador(act_CLB_STOP) {}

  bool operator!=(const estado& otro) const { return !(*this == otro); }
  bool operator==(const estado& otro) const
  {
    return (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab == otro.bikini_colab and zapatillas_colab == otro.zapatillas_colab and ultima_orden_colaborador == otro.ultima_orden_colaborador);
  }
  
  bool operator<(const estado& otro) const
  {
    if (jugador.f < otro.jugador.f) return true;
    else if (jugador.f == otro.jugador.f and jugador.c < otro.jugador.c) return true;
    else if (jugador.f == otro.jugador.f and jugador.c == otro.jugador.c and jugador.brujula < otro.jugador.brujula) return true;
    else if (jugador == otro.jugador and colaborador.f < otro.colaborador.f) return true;
    else if (jugador == otro.jugador and colaborador.f == otro.colaborador.f and colaborador.c < otro.colaborador.c) return true;
    else if (jugador == otro.jugador and colaborador.f == otro.colaborador.f and colaborador.c == otro.colaborador.c and colaborador.brujula < otro.colaborador.brujula) return true;
    else if (jugador == otro.jugador and colaborador == otro.colaborador and bikini < otro.bikini) return true;
    else if (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas < otro.zapatillas) return true;
    else if (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab < otro.bikini_colab) return true;
    else if (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab == otro.bikini_colab and zapatillas_colab < otro.zapatillas_colab) return true;
    else if (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab == otro.bikini_colab and zapatillas_colab == otro.zapatillas_colab and ultima_orden_colaborador < otro.ultima_orden_colaborador) return true;
    else return false;
  }
};
struct nodo
{
  unsigned short coste, coste_con_heuristica;
  estado st;
  //queue<Action> secuencia;
  const nodo_accion *p_nodo_accion;

  nodo() : coste(0), coste_con_heuristica(0), st(), p_nodo_accion(nullptr) {}

  bool operator==(const nodo& otro) const { return (st == otro.st); }
  bool operator<(const nodo& otro) const { return st < otro.st; }
  bool operator>(const nodo& otro) const { return !(*this < otro) and !(*this == otro); }
  struct mayor_coste
  {
    bool operator()(const nodo& uno, const nodo& otro) const
    {
      return (uno.coste_con_heuristica > otro.coste_con_heuristica);
    }
  };

  queue<Action> secuencia() const
  {
    stack<Action> pila;
    nodo_accion actual = *p_nodo_accion;
    while (actual.padre != nullptr)
    {
      pila.push(actual.accion);
      actual = *actual.padre;
    }
    queue<Action> secuencia;
    while (!pila.empty())
    {
      secuencia.push(pila.top());
      pila.pop();
    }
    return secuencia;
  }
};

class ComportamientoJugador : public Comportamiento {
  public:

    ComportamientoJugador(unsigned int size) : Comportamiento(size),
    hayPlan(false), paso(0), hijos_explorados(0), nodos_abiertos(0), nodos_cerrados(0), consumo_total_bateria(0), ultima_accion(actIDLE)
    {
      poner_bordes_en_matriz();
      reset();
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR),
    hayPlan(false), paso(0), hijos_explorados(0), nodos_abiertos(0), nodos_cerrados(0), consumo_total_bateria(0), ultima_accion(actIDLE)
    {

      poner_bordes_en_matriz();
      reset();
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

  private:
    size_t hijos_explorados, nodos_abiertos, nodos_cerrados, consumo_total_bateria, paso;
    estado c_state;
    ubicacion goal, debug = {44, 1, norte};
    queue<Action> plan, plan_colaborador;
    queue<nodo> plan_nodos;
    bool hayPlan;

    // ----------- VARIABLES DE LA BUSQUEDA REACTIVA -----------
    Action ultima_accion;
    bool ubicado, recargador_encontrado, recargando;
    unsigned short prioridad_recarga;
    unsigned short ciclos_desde_ultima_recarga;
    //map<ubicacion, char, comparador_ubicaciones> casillas_agentes;
    //priority_queue<ubicacion> recargadores;
//--------------------------------------------------------------------------------------------------------------

// ----------------- FUNCIONES DE LA DEBUGGING ----------------- 

    void mostrar_lista(const queue<nodo> &q, bool completa = false) const;
    void mostrar_lista(const priority_queue<nodo, vector<nodo>, nodo::mayor_coste> &q, bool completa) const;
    void mostrar_ubicacion(const ubicacion &ub) const;
    void mostrar_estado(const estado &st, const unsigned char nivel) const;
    void mostrar_nodo(const nodo &nd, const unsigned char nivel, bool mostrar_secuencia = false) const;
    void accion_string (const Action &a) const;
    void orientacion_string (const Orientacion &o) const;
    void anula_matriz(vector<vector<unsigned char>> & matriz);
    void PintaPlan(const queue<Action> &plan) const;
    void VisualizaPlan(const estado &st, const queue<Action> &plan);
    queue<nodo> generar_nodos_secuencia(const queue<Action> &plan, const estado &inicio);
    
// ----------------- FUNCIONES MAIN ----------------------------

    void reset();
    void poner_bordes_en_matriz();
    void registrar_sensores(Sensores sensor);
    void registrar_estado(const Sensores &sensores, const Action a);
    Action think(Sensores sensores);

// ----------------- FUNCIONES DE LA CONSULTA ------------------ 

    ubicacion next_casilla(const ubicacion &pos) const;
    ubicacion traductor_posicion(const ubicacion &pos, const int offset_fil, const int offset_col) const;
    bool es_solucion (const estado &st, const ubicacion &final, const unsigned char nivel) const;
    bool esta_en_rango_vision(const estado &st) const;
    bool casilla_transitable(const estado &cst, bool colaborador) const;
    unsigned short coste_accion_total(const Action a, const estado &st) const;
    unsigned short coste_accion(const Action a, const estado &st) const;

// ----------------- FUNCIONES DE LA AUXILIARES ---------------- 

    estado apply(const Action &a, const estado &st) const;
    unsigned short distancia_manhattan(const ubicacion &ub, const ubicacion &final) const;
    unsigned short distancia_chebyshev(const ubicacion &ub, const ubicacion &final) const;
    nodo generar_nodo(const Action a, const nodo &padre, const unsigned char nivel, const ubicacion &final = {0,0}) const;
    estado generar_estado(const Action a, const estado &st, const unsigned char nivel) const;
    void liberar_memoria_grafo(queue<nodo> &frontier) const;
    void liberar_memoria_grafo(priority_queue<nodo, vector<nodo>, nodo::mayor_coste> &frontier) const;
    void liberar_memoria_nodo(const nodo_accion *nodo) const;

// ----------------- FUNCIONES DE LA BÚSQUEDA ------------------ 

    queue<Action> nivel_2_3_4(const estado &jugador, const ubicacion &destino, const unsigned char nivel);
    queue<Action> nivel_0_1(const estado &inicio, const ubicacion &final, const unsigned char nivel);

// ----------------- EL INTERACT ----------------- 

    int interact(Action accion, int valor);
};
struct ubicacion_hash {
    std::size_t operator()(const ubicacion& u) const {
        std::size_t hash_val = std::hash<unsigned char>()(u.f);
        hash_val ^= std::hash<unsigned char>()(u.c) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
        hash_val ^= std::hash<unsigned char>()(u.brujula) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
        return hash_val;
    }
};
struct estado_hash {
    std::size_t operator()(const estado& e) const {
        std::size_t hash_val = ubicacion_hash()(e.jugador);
        hash_val ^= ubicacion_hash()(e.colaborador) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
        hash_val ^= std::hash<unsigned char>()(e.ultima_orden_colaborador) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
        hash_val ^= std::hash<bool>()(e.zapatillas) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
        hash_val ^= std::hash<bool>()(e.bikini) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
        hash_val ^= std::hash<bool>()(e.zapatillas_colab) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
        hash_val ^= std::hash<bool>()(e.bikini_colab) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
        return hash_val;
    }
};

#endif
