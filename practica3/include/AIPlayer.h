#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

# include "Attributes.h"
# include "Player.h"
#include <limits>
#include <queue>
#include <utility>
#include <vector>
#include <unordered_set>
#include <list>

using namespace std;

struct movimiento
{
    color c;
    int id;
    int dado;

    bool operator==(const movimiento &m) const
    {
        return c == m.c && id == m.id && dado == m.dado;
    }
};
/*
struct ordenador_estados
{
    bool operator()(const Parchis &a, const Parchis &b) const;
    bool operator()(const pair<Parchis, movimiento> &a, const pair<Parchis, movimiento> &b) const;
    bool operator()(const tuple<double, Parchis, movimiento> &a, const tuple<double, Parchis, movimiento> &b) const;
};

typedef priority_queue<tuple<double, Parchis, movimiento>, vector<tuple<double, Parchis, movimiento>>, ordenador_estados> cola_hijos_raiz;
*/
class AIPlayer: public Player{
    protected:
        //Id identificativo del jugador
        const int id;

    public:
        /**
         * @brief Constructor de un objeto AIPlayer
         * 
         * @param name Nombre del jugador
         */
        inline AIPlayer(const string & name):Player(name), id(1){};
        
        /**
         * @brief Constructor de un objeto AIPlayer 
         * 
         * @param name Nombre  del jugador
         * @param id Id del jugador
         */
        inline AIPlayer(const string & name, const int id):Player(name), id(id){};

        /**
         * @brief Función que percibe el el parchís y al jugador actual.
         * Asigna el tablero en actual y el id del jugador.
         * 
         * @param p Instancia Parchis que se quiere percibir
         */
        inline virtual void perceive(Parchis &p){Player::perceive(p);}

        /**
         * @brief Función abstracta que define el movimiento devuelto por el jugador.
         * Llama a la función movePiece con el valor asignado a los parámetros pasados 
         * por referencia.
         * 
         * @return true
         * @return false 
         */
        virtual bool move();
        
        /**
         * @brief Función que se encarga de decidir el mejor movimiento posible a 
         * partir del estado actual del tablero. Asigna a las variables pasadas por
         * referencia el valor de color de ficha, id de ficha y dado del mejor movimiento.
         * 
         * @param c_piece Color de la ficha
         * @param id_piece Id de la ficha
         * @param dice Número de dado
         */
        virtual void think(color & c_piece,  int & id_piece, int & dice) const;

        /**
         * @brief Método que determina si el player es inteligente (decide el mejor movimiento)
         * o no. True para AIPlayer.
         * 
         * @return true 
         * @return false 
         */
        inline virtual bool canThink() const{return true;}

        /**
         * @brief Heurística de prueba para validar el algoritmo de búsqueda.
         * 
         * @param estado Instancia de Parchis con el estado actual de la partida.
         * @param jugador Id del jugador actual (0 o 1)
         * @return double 
         */
        static double ValoracionTest(const Parchis &estado, int jugador);

        /**
         * @brief Heurística óptima.
         * 
         * @param estado Instancia de Parchis con el estado actual de la partida.
         * @return nota asignada al estado
         */
        static double evaluacion_optima(const Parchis &estado, int jugador);
        //static double evaluacion_solo_distancias(const Parchis &estado, int jugador);
        //static double evaluacion_excesivamente_compleja(const Parchis &estado, int jugador);


        /**
         * @brief Propuesta de declaración de la función poda alfa-beta.
         * La propuesta es solo sugerencia, los parámetros de la declaración podrían variar.
         */
        double alpha_beta(const Parchis &actual, double alpha, double beta, unsigned char profundidad_restante) const;
        movimiento alpha_beta_raiz(const Parchis &actual, const unsigned char profundidad) const;


        //static double media_dados_disponibles(const Parchis &estad, const int jugador);
        //static double mejor_dado_disponible(const Parchis &estado, const int jugador);

        //static vector<pair<color, int>> piezas_comibles(const Parchis &estado);
        //static vector<double> medias_avance_colores(const Parchis &estado);
        static double avance_color(const Parchis &estado, const int c);
        static double media_avance_color(const Parchis &estado, const int c);
        static vector<double> penalizacion_por_riesgo(const Parchis &estado, const list<pair<color, int>> &piezas_jug_amenaza, const list<pair<color, int>> &piezas_jug_vulnerable, const list<pair<color, int>> &piezas_opo_amenaza, const list<pair<color, int>> &piezas_opo_vulnerable);
        //static vector<double> media_peligro_colores(const Parchis &estado);
        //static int distancia_enemigo_mas_cercano(const Parchis &estado, const color c_jug, const int id_jug);



        //queue<Parchis> get_hijos_orden_dados(const Parchis &estado) const;
        //priority_queue<Parchis, vector<Parchis>, ordenador_estados> get_hijos_orden_heuristica(const Parchis &estado) const;
        //cola_hijos_raiz get_hijos_orden_dados_raiz(const Parchis &estado) const;

        static bool is_goal(const Parchis &estado, const int c, const int id);
        //queue<pair<Parchis, movimiento>> get_hijos_orden_heristica_raiz(const Parchis &estado) const;

        //static void print_movimientos(const cola_hijos_raiz &ultimos_mejores_movimientos);
};
#endif