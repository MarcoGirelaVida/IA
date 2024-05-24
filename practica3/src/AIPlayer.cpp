# include "AIPlayer.h"
# include "Parchis.h"
#include <iomanip>

const double inf = numeric_limits<double>::infinity();;
const double gana = inf, pierde = (-inf);
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta
unsigned char JUGADOR_PRINCIPAL;
const double PESO_COLOR_ADELANTADO = 1.25;
const double PENALIZACION_COLOR_ATRASADO = 0.75;
const unsigned char NUM_TOTAL_CASILLAS = 73;
const unsigned char BENEFICIO_POSESION_DADO_ESPECIAL = 50;
double (*heuristica)(const Parchis &estado, int jugador);

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

    switch (id)
    {
    case 0: heuristica = &ValoracionTest; break;
    case 1: heuristica = &evaluacion_solo_distancias; break;
    case 10: heuristica = &evaluacion_optima; break;
    default: cerr << "Error en la elección de la heurística" << endl; exit(1);
    }

    movimiento mov = alpha_beta_raiz(*actual);

    c_piece = mov.c;
    id_piece = mov.id;
    dice = mov.dado;
    cout << "Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
}

bool AIPlayer::ordenador_estados::operator()(const Parchis &a, const Parchis &b) const
{
    if (a.getCurrentPlayerId() == JUGADOR_PRINCIPAL)
        return heuristica(a, a.getCurrentPlayerId()) > heuristica(b, b.getCurrentPlayerId());
    else
        return heuristica(a, a.getCurrentPlayerId()) < heuristica(b, b.getCurrentPlayerId());
}

bool AIPlayer::ordenador_estados::operator()(const pair<Parchis, movimiento> &a, const pair<Parchis, movimiento> &b) const
{
    const Parchis& estadoA = a.first;
    const Parchis& estadoB = b.first;
    if (estadoA.getCurrentPlayerId() == JUGADOR_PRINCIPAL)
        return heuristica(estadoA, estadoA.getCurrentPlayerId()) > heuristica(estadoB, estadoB.getCurrentPlayerId());
    else
        return heuristica(estadoA, estadoA.getCurrentPlayerId()) < heuristica(estadoB, estadoB.getCurrentPlayerId());
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
    if (hijos.size() == 1) return hijos.front().second;

    //cerr << "HIJOS DE LA RAIZ: " << hijos.size() << endl;
    //queue<pair<Parchis, movimiento>> hijos_aux = hijos;
    //while (!hijos_aux.empty())
    //{
    //    pair<Parchis, movimiento> hijo = hijos_aux.front();
    //    hijos_aux.pop();
    //    cerr << "Color: " << str(hijo.second.c) << " Ficha: " << hijo.second.id << " Dado: " << hijo.second.dado << " Heuristica: " << heuristica(hijo.first, hijo.first.getCurrentPlayerId()) << endl;
    //}

    while (!hijos.empty())
    {
        pair<Parchis, movimiento> hijo = hijos.front();
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

// Alpha = Lo mínimo que tengo que conseguir para que sea útil
// Beta  = Lo máximo que puedo conseguir en esta rama
// Si beta es menor que alpha, no me interesa seguir por esta rama
// Cuanto mayor sea alpha (haya encontrado mejores movimientos), más posibilidades de que un beta se pase y podes
// Cuando menor sea un beta (haya encontrado movimientos peores), más posibilidades de que aparezca una actualización de alpha que lo sobrepase
// Si la nota de un nodo es mayor que beta me da igual porque aunque sea mejor nunca me voy a meter por ahí, asi que devuelvo beta
double AIPlayer::alpha_beta(const Parchis &actual, double alpha, double beta, unsigned char profundidad_restante) const
{
    if (profundidad_restante == 0 or actual.gameOver()) return heuristica(actual, actual.getCurrentPlayerId());
    
    //priority_queue<Parchis, vector<Parchis>, AIPlayer::ordenador_estados> hijos = get_hijos_ordenados(actual);
    queue<Parchis> hijos = get_hijos_cola(actual);

    while (!hijos.empty())
    {
        Parchis hijo = hijos.front();
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

double AIPlayer::evaluacion_solo_distancias(const Parchis &estado, int jugador)
{
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Retorna ganador / Perdedor si lo hubiese
    if (ganador == jugador)
        return gana;
    else if (ganador == oponente)
        return pierde;

    //cerr << "----------------- EVALUANDO ACCION -----------------" << endl;
    //cerr << "Color: " << get<0>(estado.getLastAction()) << " Ficha: " << get<1>(estado.getLastAction()) << " Dado: " << get<2>(estado.getLastAction()) << endl << endl;
    // Calculo la media de las distancias a la meta de cada color
    vector<color> my_colors = estado.getPlayerColors(jugador);
    vector<color> op_colors = estado.getPlayerColors(oponente);
    vector<double> media_avance_color(4, 0);
    double puntuacion_distancia_jugador = 0, puntuacion_distancia_oponente = 0;

    for (int c = 0; c < 4; c++)
    {
        for (int i = 0; i < num_pieces; i++)
        {
            //cerr << "Distancia a la meta de la ficha " << i << " de color " << c << ": " << estado.distanceToGoal((color) c, i) << endl;
            media_avance_color[c] += estado.distanceToGoal((color) c, i);
        }
        media_avance_color[c] /= num_pieces;
        //cerr << "Media color " << c << ": " << media_avance_color[c] << endl;
    }

    // JUGADOR
    double media_color_adelantado_jug = NUM_TOTAL_CASILLAS - min(media_avance_color[my_colors[0]], media_avance_color[my_colors[1]]);
    double media_color_atrasado_jug = NUM_TOTAL_CASILLAS - max(media_avance_color[my_colors[0]], media_avance_color[my_colors[1]]);
    puntuacion_distancia_jugador = media_color_adelantado_jug*PESO_COLOR_ADELANTADO + media_color_atrasado_jug*PENALIZACION_COLOR_ATRASADO;
    //cerr << "Media jugador adelantado: " << media_color_adelantado_jug << " Media jugador atrasado: " << media_color_atrasado_jug << endl;
    //cerr << "Puntuacion distancia jugador: " << puntuacion_distancia_jugador << endl;

    // OPONENTE
    double media_color_adelantado_opo = NUM_TOTAL_CASILLAS - min(media_avance_color[op_colors[0]], media_avance_color[op_colors[1]]);
    double media_color_atrasado_opo = NUM_TOTAL_CASILLAS - max(media_avance_color[op_colors[0]], media_avance_color[op_colors[1]]);
    puntuacion_distancia_oponente = media_color_adelantado_opo*PESO_COLOR_ADELANTADO + media_color_atrasado_opo*PENALIZACION_COLOR_ATRASADO;
    //cerr << "Media oponente adelantado: " << media_color_adelantado_opo << " Media oponente atrasado: " << media_color_atrasado_opo << endl;
    //cerr << "Puntuacion distancia oponente: " << puntuacion_distancia_oponente << endl;

    //const unsigned char extra_por_barrera = estado
    return (puntuacion_distancia_jugador - puntuacion_distancia_oponente);
}

double AIPlayer::evaluacion_optima(const Parchis &estado, int jugador)
{
    //cerr << endl << endl << "----------------- EVALUANDO ACCION -----------------" << endl;
    //cerr << "Color: " << get<0>(estado.getLastAction()) << " Ficha: " << get<1>(estado.getLastAction()) << " Dado: " << get<2>(estado.getLastAction()) << endl << endl;

    double puntuacion_final = 0;
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;
    vector<color> my_colors = estado.getPlayerColors(jugador);
    vector<color> op_colors = estado.getPlayerColors(oponente);

    // -------------------------------------------------------------------------------------------
    // EN CASO DE GANAR / PERDER O ENERGIA

    if (ganador == jugador) return gana;
    else if (ganador == oponente) return pierde;

    // -------------------------------------------------------------------------------------------
    // SUMO LOS PUNTOS DE LAS CASILLAS DE META
    
    double extra_por_ficha_en_meta = gana / num_pieces;
    for (int i = 0; i < num_pieces; i++)
    {
        if (estado.getBoard().getPiece(my_colors[0], i).get_box().type == goal)
            puntuacion_final += extra_por_ficha_en_meta;
        if (estado.getBoard().getPiece(my_colors[1], i).get_box().type == goal)
            puntuacion_final += extra_por_ficha_en_meta;
        if (estado.getBoard().getPiece(op_colors[0], i).get_box().type == goal)
            puntuacion_final -= extra_por_ficha_en_meta;
        if (estado.getBoard().getPiece(op_colors[1], i).get_box().type == goal)
            puntuacion_final -= extra_por_ficha_en_meta;
    }

    // -------------------------------------------------------------------------------------------
    // PUNTUACIÓN SEGÚN AVANCE

    vector<double> media_avance_color = medias_avance_colores(estado);
    color color_adelantado_jug = media_avance_color[my_colors[0]] < media_avance_color[my_colors[1]] ? my_colors[0] : my_colors[1];
    color color_atrasado_jug = color_adelantado_jug == my_colors[0] ? my_colors[1] : my_colors[0];
    color color_adelantado_opo = media_avance_color[op_colors[0]] < media_avance_color[op_colors[1]] ? op_colors[0] : op_colors[1];
    color color_atrasado_opo = color_adelantado_opo == op_colors[0] ? op_colors[1] : op_colors[0];

    double puntuacion_distancia_jugador = media_avance_color[color_adelantado_jug]*PESO_COLOR_ADELANTADO + media_avance_color[color_atrasado_jug]*PENALIZACION_COLOR_ATRASADO;
    double puntuacion_distancia_oponente = media_avance_color[color_adelantado_opo]*PESO_COLOR_ADELANTADO + media_avance_color[color_atrasado_opo]*PENALIZACION_COLOR_ATRASADO;

    puntuacion_final = (puntuacion_distancia_jugador - puntuacion_distancia_oponente);

    ////cerr << "\n\ttJUG adelantado: " << media_color_adelantado_jug << " JUG atrasado: " << media_color_atrasado_jug << endl;
    //cerr << "\tPuntuacion distancia jugador: " << puntuacion_distancia_jugador << endl;

    ////cerr << "\tOPO adelantado: " << media_color_adelantado_opo << " OPO atrasado: " << media_color_atrasado_opo << endl;
    //cerr << "\tPuntuacion distancia oponente: " << puntuacion_distancia_oponente << endl;

    //cerr << "Puntuación final tras distancia: " << puntuacion_final << endl << endl;

    // -------------------------------------------------------------------------------------------
    // PUNTUACION POR DADOS DISPONIBLES

    const double media_dados_disponibles_jug = media_dados_disponibles(estado, jugador);
    const double media_dados_disponibles_opo = media_dados_disponibles(estado, oponente);

    puntuacion_final += (media_dados_disponibles_jug - media_dados_disponibles_opo);

    //cerr << "\tMedia dados disponibles jugador: " << media_dados_disponibles_jug << endl;
    //cerr << "\tMedia dados disponibles oponente: " << media_dados_disponibles_opo << endl;

    //cerr << "Puntuación final tras dados: " << puntuacion_final << endl << endl;

    // -------------------------------------------------------------------------------------------
    // PUNTUACION RIESGO

    vector<double> media_peligro_color = media_peligro_colores(estado);

    double puntuacion_peligro_jugador = media_peligro_color[color_adelantado_jug]*PESO_COLOR_ADELANTADO + media_peligro_color[color_atrasado_jug]*PENALIZACION_COLOR_ATRASADO;
    double puntuacion_peligro_oponente = media_peligro_color[color_adelantado_opo]*PESO_COLOR_ADELANTADO + media_peligro_color[color_atrasado_opo]*PENALIZACION_COLOR_ATRASADO;

    puntuacion_final = (puntuacion_peligro_jugador - puntuacion_peligro_oponente);

    // -------------------------------------------------------------------------------------------

    // QUIESCENCIA
    // la razón por la que uso el mejor dado del opo es porque le estoy "robando" un turno
    const double extra_por_seis = estado.getLastDice() == 6 ? mejor_dado_disponible(estado, oponente) : 0;
    const unsigned char extra_por_comer = estado.isEatingMove() ? 20 + mejor_dado_disponible(estado, oponente) : 0;
    //const unsigned char extra_por_barrera = estado. ? 10 + mejor_dado_disponible(estado, oponente) : 0;

    puntuacion_final += extra_por_comer + extra_por_seis;

    //if (extra_por_comer)
    //    cerr << ">>>>>>>>>>>>>>>>>>> Puntuacion extra por comer" << endl;
    //if (extra_por_seis)
    //    cerr << ">>>>>>>>>>>>>>>>>>> Puntuacion extra por seis" << endl;

    // -------------------------------------------------------------------------------------------

    //cerr << "Puntuacion final (tras quiescencia): " << puntuacion_final << endl;
    return puntuacion_final;
}

unordered_set<int> AIPlayer::piezas_comibles(const Parchis &estado, const int jugador)
{
    int oponente = (jugador+1) % 2;
    vector<color> my_colors = estado.getPlayerColors(jugador);
    vector<color> op_colors = estado.getPlayerColors(oponente);

    unordered_set<int> comibles;
    ParchisBros hijos = estado.getChildren();

    if ((*hijos.begin()).getCurrentPlayerId() == jugador) return comibles;

    for (auto it = hijos.begin(); it != hijos.end(); ++it)
    {
        if ((*it).isEatingMove())
        {
            int pos_pieza_comida = estado.getBoard().getPiece(it.getMovedColor(), it.getMovedPieceId()).get_box().num;
            comibles.insert(pos_pieza_comida);
        }
    }

    return comibles;
}

double AIPlayer::media_dados_disponibles(const Parchis &estado, const int jugador)
{
    double media = 0;
    vector<int> dados_disponibles = estado.getNormalDices(jugador);
    for (const auto &dado : dados_disponibles)
    {
        //cerr << "\tDado: " << dado << endl;
        media += dado == 100 ? BENEFICIO_POSESION_DADO_ESPECIAL : dado;
    }
    return media / dados_disponibles.size();
}

double AIPlayer::mejor_dado_disponible(const Parchis &estado, const int jugador)
{
    vector<int> dados_disponibles = estado.getNormalDices(jugador);
    return *max_element(dados_disponibles.begin(), dados_disponibles.end());
}

vector<double> AIPlayer::medias_avance_colores(const Parchis &estado)
{
    vector<double> media_avance_color(4, 0);
    unordered_set<int> comibles = piezas_comibles(estado, estado.getCurrentPlayerId());

    for (int c = 0; c < 4; c++)
    {
        for (int i = 0; i < num_pieces; i++)
        {
            ////cerr << "\t - Avance ficha [" << c << ", " << i << "]: " <<  NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, i) << endl;
            if (comibles.find(estado.getBoard().getPiece((color) c, i).get_box().num) == comibles.end())
                media_avance_color[c] += (NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, i));
            else
            {
                media_avance_color[c] -= 20 + mejor_dado_disponible(estado, estado.getCurrentPlayerId());
                //cerr << ">>>>>>>>>>>>>>>> Ficha [" << c << ", " << i << "] es comible" << endl;
            }
        }
        media_avance_color[c] /= num_pieces;
        //cerr << "\tMedia color " << c << ": " << media_avance_color[c] << endl;
    }

    return media_avance_color;
}

vector<double> AIPlayer::media_peligro_colores(const Parchis &estado)
{
    int jugador = estado.getCurrentPlayerId();
    int oponente = (estado.getCurrentPlayerId()+1) % 2;
    vector<color> colores_jug = estado.getPlayerColors(jugador);
    vector<double> media_peligro_color(4, 0);
    unordered_set<int> comibles = piezas_comibles(estado, jugador);

    for (int c_jug = 0; c_jug < 4; c_jug++)
    {
        int jug_actual = c_jug == colores_jug[0] or c_jug == colores_jug[1] ? jugador : oponente;
        int opo_actual = jug_actual == jugador ? oponente : jugador;

        for (int id_jug = 0; id_jug < num_pieces; id_jug++)
        {
            // Si es casilla comible devuelvo su valor íntegro que es lo que voy a perder
            if (comibles.find(estado.getBoard().getPiece((color) c_jug, id_jug).get_box().num) != comibles.end())
            {
                media_peligro_color[c_jug] += NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c_jug, id_jug);
            }
            else
            {
                int distancia_enemigo_mas_peligroso = -1;
                const vector<color> colores_opo = estado.getPlayerColors(opo_actual);
                for (color c_opo : colores_opo)
                {
                    for (int id_opo = 0; id_opo < num_pieces; id_opo++)
                    {
                        int distancia_enemigo_i = estado.distanceBoxtoBox(c_opo, estado.getBoard().getPiece(c_opo, id_opo).get_box(), c_jug, estado.getBoard().getPiece((color) c_jug, id_jug).get_box());
                        if (distancia_enemigo_mas_peligroso > distancia_enemigo_i)
                            distancia_enemigo_mas_peligroso = distancia_enemigo_i;
                    }
                }
                if (distancia_enemigo_mas_peligroso != -1)
                    media_peligro_color[c_jug] += (NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c_jug, id_jug)) * // RATIO * distancia_mas_peligroso
            }
        }
        media_peligro_color[c_jug] /= num_pieces;
    }

    return media_peligro_color;
}