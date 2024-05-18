#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

# include "Attributes.h"
# include "Player.h"
#include <limits>
#include <queue>
#include <utility>
#include <vector>
using namespace std;

struct movimiento
{
    color c;
    int id;
    int dado;
};
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

        /**
         * @brief Propuesta de declaración de la función poda alfa-beta.
         * La propuesta es solo sugerencia, los parámetros de la declaración podrían variar.
         */
        double alpha_beta(const Parchis &actual, double alpha, double beta, unsigned char profundidad_restante) const;
        movimiento alpha_beta_raiz(const Parchis &actual) const;

        struct ordenador_estados
        {
            bool operator()(const Parchis &a, const Parchis &b) const;
            bool operator()(const std::pair<Parchis, movimiento> &a, const std::pair<Parchis, movimiento> &b) const;
        };
        queue<Parchis> get_hijos_cola(const Parchis &estado) const;
        queue<pair<Parchis, movimiento>> get_hijos_cola_raiz(const Parchis &estado) const;
        priority_queue<pair<Parchis, movimiento>, vector<pair<Parchis, movimiento>>, ordenador_estados> get_hijos_ordenados_raiz(const Parchis &estado) const;
        priority_queue<Parchis, vector<Parchis>, ordenador_estados> get_hijos_ordenados(const Parchis &estado) const;
};
#endif