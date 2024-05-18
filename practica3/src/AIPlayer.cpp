# include "AIPlayer.h"
# include "Parchis.h"
#include <iomanip>

const double inf = numeric_limits<double>::infinity();;
const double gana = inf, pierde = (-inf);
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta
unsigned char JUGADOR_PRINCIPAL;

bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const
{
    JUGADOR_PRINCIPAL = actual->getCurrentPlayerId();

    movimiento mov = alpha_beta_raiz(*actual);

    c_piece = mov.c;
    id_piece = mov.id;
    dice = mov.dado;
    cout << "Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
}

bool AIPlayer::ordenador_estados::operator()(const Parchis &a, const Parchis &b) const
{
    if (a.getCurrentPlayerId() == JUGADOR_PRINCIPAL)
        return AIPlayer::ValoracionTest(a, a.getCurrentPlayerId()) > AIPlayer::ValoracionTest(b, b.getCurrentPlayerId());
    else
        return AIPlayer::ValoracionTest(a, a.getCurrentPlayerId()) < AIPlayer::ValoracionTest(b, b.getCurrentPlayerId());
}

bool AIPlayer::ordenador_estados::operator()(const pair<Parchis, movimiento> &a, const pair<Parchis, movimiento> &b) const
{
    const Parchis& estadoA = a.first;
    const Parchis& estadoB = b.first;
    if (estadoA.getCurrentPlayerId() == JUGADOR_PRINCIPAL)
        return AIPlayer::ValoracionTest(estadoA, estadoA.getCurrentPlayerId()) > AIPlayer::ValoracionTest(estadoB, estadoB.getCurrentPlayerId());
    else
        return AIPlayer::ValoracionTest(estadoA, estadoA.getCurrentPlayerId()) < AIPlayer::ValoracionTest(estadoB, estadoB.getCurrentPlayerId());
}

priority_queue<pair<Parchis, movimiento>, vector<pair<Parchis, movimiento>>, AIPlayer::ordenador_estados> AIPlayer::get_hijos_ordenados_raiz(const Parchis &estado) const
{
    priority_queue<pair<Parchis, movimiento>, vector<pair<Parchis, movimiento>>, ordenador_estados> hijos;
    ParchisBros hijos_aux = estado.getChildren();
    for (auto it = hijos_aux.begin(); it != hijos_aux.end(); ++it)
        hijos.push({*it, {it.getMovedColor(), it.getMovedPieceId(), it.getMovedDiceValue()}});

    return hijos;
}

priority_queue<Parchis, vector<Parchis>, AIPlayer::ordenador_estados> AIPlayer::get_hijos_ordenados(const Parchis &estado) const
{
    priority_queue<Parchis, vector<Parchis>, ordenador_estados> hijos;
    ParchisBros hijos_aux = estado.getChildren();
    for (const auto &hijo : hijos_aux)
        hijos.push(hijo);

    return hijos;
}

queue<Parchis> AIPlayer::get_hijos_cola(const Parchis &estado) const
{
    queue<Parchis> hijos;
    ParchisBros hijos_aux = estado.getChildren();
    for (const auto &hijo : hijos_aux)
        hijos.push(hijo);

    return hijos;
}

queue<pair<Parchis, movimiento>> AIPlayer::get_hijos_cola_raiz(const Parchis &estado) const
{
    queue<pair<Parchis, movimiento>> hijos;
    ParchisBros hijos_aux = estado.getChildren();
    for (auto it = hijos_aux.begin(); it != hijos_aux.end(); ++it)
        hijos.push({*it, {it.getMovedColor(), it.getMovedPieceId(), it.getMovedDiceValue()}});

    return hijos;
}

movimiento AIPlayer::alpha_beta_raiz(const Parchis &actual) const
{
    double alpha = -inf, beta = inf;
    movimiento mejor_movimiento;

    //priority_queue<pair<Parchis, movimiento>, vector<pair<Parchis, movimiento>>, AIPlayer::ordenador_estados> hijos = get_hijos_ordenados_raiz(actual);
    queue<pair<Parchis, movimiento>> hijos = get_hijos_cola_raiz(actual);
    if (hijos.size() == 1) return hijos.front().second; //return hijos.top().second; 

    while (!hijos.empty())
    {
        pair<Parchis, movimiento> hijo = hijos.front(); //hijos.top(); 
        hijos.pop();

        double nota = actual.getCurrentPlayerId() != hijo.first.getCurrentPlayerId() ? -alpha_beta(hijo.first, -beta, -alpha, PROFUNDIDAD_ALFABETA - 1) : alpha_beta(hijo.first, alpha, beta, PROFUNDIDAD_ALFABETA - 1);
        if (nota > alpha)
        {
            mejor_movimiento = hijo.second;
            alpha = nota;
        }
    }
    return mejor_movimiento;
}

double AIPlayer::alpha_beta(const Parchis &actual, double alpha, double beta, unsigned char profundidad_restante) const
{
    if (profundidad_restante == JUGADOR_PRINCIPAL) return ValoracionTest(actual, actual.getCurrentPlayerId());
    
    priority_queue<Parchis, vector<Parchis>, AIPlayer::ordenador_estados> hijos = get_hijos_ordenados(actual);
    //queue<Parchis> hijos = get_hijos_cola(actual);
    while (!hijos.empty())
    {
        Parchis hijo = hijos.top(); //hijos.front();
        hijos.pop();

        double nota = actual.getCurrentPlayerId() != hijo.getCurrentPlayerId() ? -alpha_beta(hijo, -beta, -alpha, profundidad_restante - 1) : alpha_beta(hijo, alpha, beta, profundidad_restante - 1);
        if (nota >= beta)
            return beta;
        if (nota > alpha)
            alpha = nota;
    }
    return alpha;
}

double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.

    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
        return gana;
    else if (ganador == oponente)
        return pierde;
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                    puntuacion_jugador++;
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                    puntuacion_jugador += 5;
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                    puntuacion_oponente++;
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                    puntuacion_oponente += 5;
            }
        }

        return puntuacion_jugador - puntuacion_oponente;
    }
}

double evaluacion_optima(const Parchis &estado)
{
    
    return 1;
}