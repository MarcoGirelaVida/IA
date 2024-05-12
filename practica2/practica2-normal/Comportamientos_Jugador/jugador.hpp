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
  //unsigned char valor_casilla_jugador, valor_casilla_colaborador;

  estado() : bikini(false), zapatillas(false), bikini_colab(false), zapatillas_colab(false), ultima_orden_colaborador(act_CLB_STOP) {}

  bool operator!=(const estado& otro) const { return !(*this == otro); }
  bool operator==(const estado& otro) const
  {
    return (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab == otro.bikini_colab and zapatillas_colab == otro.zapatillas_colab and ultima_orden_colaborador == otro.ultima_orden_colaborador);
  }
  
  bool operator<(const estado& otro) const
  {
    if ((jugador.f < otro.jugador.f)
    or (jugador.f == otro.jugador.f and jugador.c < otro.jugador.c) 
    or (jugador.f == otro.jugador.f and jugador.c == otro.jugador.c and jugador.brujula < otro.jugador.brujula) 
    or (jugador == otro.jugador and colaborador.f < otro.colaborador.f) 
    or (jugador == otro.jugador and colaborador.f == otro.colaborador.f and colaborador.c < otro.colaborador.c) 
    or (jugador == otro.jugador and colaborador.f == otro.colaborador.f and colaborador.c == otro.colaborador.c and colaborador.brujula < otro.colaborador.brujula) 
    or (jugador == otro.jugador and colaborador == otro.colaborador and bikini < otro.bikini) 
    or (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas < otro.zapatillas) 
    or (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab < otro.bikini_colab) 
    or (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab == otro.bikini_colab and zapatillas_colab < otro.zapatillas_colab) 
    or (jugador == otro.jugador and colaborador == otro.colaborador and bikini == otro.bikini and zapatillas == otro.zapatillas and bikini_colab == otro.bikini_colab and zapatillas_colab == otro.zapatillas_colab and ultima_orden_colaborador < otro.ultima_orden_colaborador))
      return true;
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

  queue<Action> secuencia_nivel_4() const
  {
    queue<Action> secuencia;
    nodo_accion actual = *p_nodo_accion;
    while (actual.padre != nullptr)
    {
      secuencia.push(actual.accion);
      actual = *actual.padre;
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

      umbral_porcentaje_bateria_maximo = 1;
      umbral_porcentaje_bateria = 0.48538461538461538462;
      umbral_ciclos_recarga = 0.14615384615384615386;
      umbral_vida = 0.17307692307692307695;

      umbral_porcentaje_bateria *= mapaResultado.size() / 100.0; 
      umbral_ciclos_recarga *= mapaResultado.size()*20.0;
      umbral_vida *= mapaResultado.size()*20.0;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR),
    hayPlan(false), paso(0), hijos_explorados(0), nodos_abiertos(0), nodos_cerrados(0), consumo_total_bateria(0), ultima_accion(actIDLE)
    {
      reset();
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

  private:
    size_t hijos_explorados, nodos_abiertos, nodos_cerrados, consumo_total_bateria, paso;
    estado c_state;
    ubicacion goal;
    queue<Action> plan, plan_colaborador;
    //queue<nodo> plan_nodos;
    bool hayPlan, plan_con_colaborador;
    unsigned char iteracion = 0;

    // ----------- VARIABLES DE LA BUSQUEDA REACTIVA -----------
    bool comparador_ubicaciones(const ubicacion& a, const ubicacion& b)
    {
      return (a.f < b.f) or (a.f == b.f and a.c < b.c);
    }
    Action ultima_accion;
    bool ubicado, buscando_recargador, recargando, colaborador_encontrado;
    float umbral_porcentaje_bateria, umbral_porcentaje_bateria_maximo, umbral_ciclos_recarga, umbral_vida;
    unsigned short ciclos_desde_ultima_recarga;
    queue<ubicacion> recargadores;
    //queue<pair<ubicacion, unsigned char>> casillas_agentes;

    queue<estado> estados_teoricos;
      
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
    queue<nodo> generar_nodos_secuencia(const queue<Action> &plan, const estado &inicio, const unsigned char nivel) const;
    queue<estado> generar_estados_secuencia(queue<Action> &plan, const estado &inicio, const unsigned char nivel) const;
    
// ----------------- FUNCIONES MAIN ----------------------------

    void reset();
    void poner_bordes_en_matriz();
    queue<Action> generar_plan(const estado &origen, const ubicacion &destino, const unsigned char nivel);
    ubicacion determinar_objetivo(const Sensores &sensores);
    void registrar_sensor_terreno(const Sensores &sensor);
    void registrar_sensor_ubicacion(const Sensores &sensor);
    void registrar_estado(const Sensores &sensores, const Action a);
    Action think(Sensores sensores);

// ----------------- FUNCIONES DE LA CONSULTA ------------------ 

    ubicacion recargador_mas_cercano() const;
    ubicacion next_casilla(const ubicacion &pos) const;
    ubicacion traductor_posicion(const ubicacion &pos, const int offset_fil, const int offset_col) const;
    bool hay_que_recargar(const unsigned short bateria_actual, const unsigned short vida_actual) const;
    bool accion_costosa(const Action a, const estado &st, const estado &st_antiguo) const;
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
