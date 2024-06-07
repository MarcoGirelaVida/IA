# include "AIPlayer.h"
# include "Parchis.h"
#include <iomanip>

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;

const int num_pieces = 3;
const int num_colores = 4;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta
double (*heuristica)(const Parchis &estado, int jugador);

const unsigned char NUM_TOTAL_CASILLAS = 74;

const double PESO_C_GOOD = 1.5;
const double PESO_C_BAD = 1;
const double PUNTOS_FICHA_EN_META = gana / (num_pieces*2.0*PESO_C_GOOD);
const double BENEFICIO_COMER = 40;
const double PENALIZACION_HOME = 20;
const double BENEFICIO_CASILLA_SEGURA = 0.25;
const double PENALIZACION_MORIR = PENALIZACION_HOME + BENEFICIO_COMER;
//unsigned char JUGADOR_PRINCIPAL;
//const double PESO_RIESGO = 0.3;
//const double PESO_RIEGO_DADOS = 0.15;
//const double BENEFICIO_POSESION_DADO_ESPECIAL = 50;
//const double BENEFICIO_POSESION_DADO_6 = 6 + PESO_C_GOOD*5;
/*
void AIPlayer::print_movimientos(const cola_hijos_raiz &ultimos_mejores_movimientos)
{
    cola_hijos_raiz ultimos_mejores_movimientos_tmp = ultimos_mejores_movimientos;
    while (!ultimos_mejores_movimientos_tmp.empty())
    {
        cerr << "\tNota: " << get<0>(ultimos_mejores_movimientos_tmp.top()) << " Accion: " << str(get<2>(ultimos_mejores_movimientos_tmp.top()).c) << " " << get<2>(ultimos_mejores_movimientos_tmp.top()).id << " " << get<2>(ultimos_mejores_movimientos_tmp.top()).dado << endl;
        ultimos_mejores_movimientos_tmp.pop();
    }
}
*/
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
    //JUGADOR_PRINCIPAL = actual->getCurrentPlayerId();

    switch (id)
    {
    case 0: heuristica = &ValoracionTest; break;
    //case 1: heuristica = &evaluacion_solo_distancias; break;
    case 10: heuristica = &evaluacion_optima; break;
    default: cerr << "Error en la elección de la heurística" << endl; exit(1);
    }

    //cola_hijos_raiz ultimos_mejores_movimientos = get_hijos_orden_dados_raiz(*actual);
    //for (int i = 6; i <= PROFUNDIDAD_ALFABETA; i++)
    //{
    //    cerr << "Profundidad: " << i << endl;
    //    ultimos_mejores_movimientos = alpha_beta_raiz(*actual, i, ultimos_mejores_movimientos);
    //    print_movimientos(ultimos_mejores_movimientos);
    //}
    //movimiento mov = get<2>(ultimos_mejores_movimientos.top());

    movimiento mov = alpha_beta_raiz(*actual, PROFUNDIDAD_ALFABETA);
    c_piece = mov.c;
    id_piece = mov.id;
    dice = mov.dado;
    cout << "Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
}

movimiento AIPlayer::alpha_beta_raiz(const Parchis &actual, const unsigned char profundidad) const
{
    double alpha = menosinf, beta = masinf;
    movimiento mejor_movimiento;
    ParchisBros hijos = actual.getChildren();
    for (auto it = hijos.begin(); it != hijos.end(); ++it)
    {
        double nota = actual.getCurrentPlayerId() != (*it).getCurrentPlayerId() ? -alpha_beta(*it, -beta, -alpha, profundidad - 1) : alpha_beta(*it, alpha, beta, profundidad - 1);
        if (nota > alpha)
        {
            alpha = nota;
            mejor_movimiento = {it.getMovedColor(), it.getMovedPieceId(), it.getMovedDiceValue()};
        }
        cerr << "NOTA HIJO: " << nota << " ACCION: " << str(it.getMovedColor()) << " " << it.getMovedPieceId() << " " << it.getMovedDiceValue() << endl;
    }
    return mejor_movimiento;


    //cola_hijos_raiz mejores_movimientos;
//
    //while (!ultimos_mejores_movimientos.empty())
    //{
    //    tuple<double, Parchis, movimiento> hijo = ultimos_mejores_movimientos.top();
    //    ultimos_mejores_movimientos.pop();
//
    //    double nota = actual.getCurrentPlayerId() != get<1>(hijo).getCurrentPlayerId() ? -alpha_beta(get<1>(hijo), -beta, -alpha, profundidad - 1) : alpha_beta(get<1>(hijo), alpha, beta, profundidad - 1);
    //    if (nota > alpha)
    //        alpha = nota;
//
    //    get<0>(hijo) = nota;
    //    mejores_movimientos.push(hijo);
    //}
    //return mejores_movimientos;
}

// Alpha = Lo mínimo que tengo que conseguir para que sea útil
// Beta  = Lo máximo que puedo conseguir en esta rama
// Si beta es menor que alpha, no me interesa seguir por esta rama
// Cuanto mayor sea alpha (haya encontrado mejores movimientos), más posibilidades de que un beta se pase y podes
// Cuando menor sea un beta (haya encontrado movimientos peores), más posibilidades de que aparezca una actualización de alpha que lo sobrepase
// Si la nota de un nodo es mayor que beta me da igual porque aunque sea mejor nunca me voy a meter por ahí, asi que devuelvo beta
double AIPlayer::alpha_beta(const Parchis &actual, double alpha, double beta, unsigned char profundidad_restante) const
{
    if (profundidad_restante == 0 or actual.gameOver())
        return heuristica(actual, actual.getCurrentPlayerId());

    ParchisBros hijos = actual.getChildren();
    for (auto it = hijos.begin(); it != hijos.end(); ++it)
    {
        double nota = actual.getCurrentPlayerId() != (*it).getCurrentPlayerId() ? -alpha_beta(*it, -beta, -alpha, profundidad_restante - 1) : alpha_beta(*it, alpha, beta, profundidad_restante - 1);
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
        vector<color> colores_jug = estado.getPlayerColors(jugador);
        vector<color> colores_opo = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        for (int i = 0; i < colores_jug.size(); i++)
        {
            color c = colores_jug[i];
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
        for (int i = 0; i < colores_opo.size(); i++)
        {
            color c = colores_opo[i];
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
/*
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
    vector<color> colores_jug = estado.getPlayerColors(jugador);
    vector<color> colores_opo = estado.getPlayerColors(oponente);
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
    double media_color_adelantado_jug = NUM_TOTAL_CASILLAS - min(media_avance_color[colores_jug[0]], media_avance_color[colores_jug[1]]);
    double media_color_atrasado_jug = NUM_TOTAL_CASILLAS - max(media_avance_color[colores_jug[0]], media_avance_color[colores_jug[1]]);
    puntuacion_distancia_jugador = media_color_adelantado_jug*PESO_C_GOOD + media_color_atrasado_jug*PESO_C_BAD;
    //cerr << "Media jugador adelantado: " << media_color_adelantado_jug << " Media jugador atrasado: " << media_color_atrasado_jug << endl;
    //cerr << "Puntuacion distancia jugador: " << puntuacion_distancia_jugador << endl;

    // OPONENTE
    double media_color_adelantado_opo = NUM_TOTAL_CASILLAS - min(media_avance_color[colores_opo[0]], media_avance_color[colores_opo[1]]);
    double media_color_atrasado_opo = NUM_TOTAL_CASILLAS - max(media_avance_color[colores_opo[0]], media_avance_color[colores_opo[1]]);
    puntuacion_distancia_oponente = media_color_adelantado_opo*PESO_C_GOOD + media_color_atrasado_opo*PESO_C_BAD;
    //cerr << "Media oponente adelantado: " << media_color_adelantado_opo << " Media oponente atrasado: " << media_color_atrasado_opo << endl;
    //cerr << "Puntuacion distancia oponente: " << puntuacion_distancia_oponente << endl;

    //const unsigned char extra_por_barrera = estado
    return (puntuacion_distancia_jugador - puntuacion_distancia_oponente);
}

double AIPlayer::evaluacion_excesivamente_compleja(const Parchis &estado, int jugador)
{
    //cerr << endl << endl << "----------------- EVALUANDO ACCION -----------------" << endl;
    //cerr << "Color: " << get<0>(estado.getLastAction()) << " Ficha: " << get<1>(estado.getLastAction()) << " Dado: " << get<2>(estado.getLastAction()) << endl << endl;

    double puntuacion_final = 0;
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;
    vector<color> colores_jug = estado.getPlayerColors(jugador);
    vector<color> colores_opo = estado.getPlayerColors(oponente);

    // -------------------------------------------------------------------------------------------
    // EN CASO DE GANAR / PERDER O ENERGIA

    if (ganador == jugador) return gana;
    else if (ganador == oponente) return pierde;

    //cerr << "Puntuación final tras ganador/perdedor: " << puntuacion_final << endl;

    // -------------------------------------------------------------------------------------------
    // SUMO LOS PUNTOS DE LAS CASILLAS DE META

    unsigned char fichas_en_meta_total = 0;
    for (int i = 0; i < num_pieces; i++)
    {
        if (estado.getBoard().getPiece(colores_jug[0], i).get_box().type == goal)
            fichas_en_meta_total++;
        if (estado.getBoard().getPiece(colores_jug[1], i).get_box().type == goal)
            fichas_en_meta_total++;
        if (estado.getBoard().getPiece(colores_opo[0], i).get_box().type == goal)
            fichas_en_meta_total--;
        if (estado.getBoard().getPiece(colores_opo[1], i).get_box().type == goal)
            fichas_en_meta_total--;
    }

    puntuacion_final += fichas_en_meta_total * PUNTOS_FICHA_EN_META;

    // -------------------------------------------------------------------------------------------
    // PUNTUACIÓN SEGÚN AVANCE

    vector<double> media_avance_color = medias_avance_colores(estado);
    color color_adelantado_jug = media_avance_color[colores_jug[0]] > media_avance_color[colores_jug[1]] ? colores_jug[0] : colores_jug[1];
    color color_atrasado_jug = color_adelantado_jug == colores_jug[0] ? colores_jug[1] : colores_jug[0];
    color color_adelantado_opo = media_avance_color[colores_opo[0]] > media_avance_color[colores_opo[1]] ? colores_opo[0] : colores_opo[1];
    color color_atrasado_opo = color_adelantado_opo == colores_opo[0] ? colores_opo[1] : colores_opo[0];

    double puntuacion_distancia_jugador = media_avance_color[color_adelantado_jug]*PESO_C_GOOD + media_avance_color[color_atrasado_jug]*PESO_C_BAD;
    double puntuacion_distancia_oponente = media_avance_color[color_adelantado_opo]*PESO_C_GOOD + media_avance_color[color_atrasado_opo]*PESO_C_BAD;

    puntuacion_final += puntuacion_distancia_jugador - puntuacion_distancia_oponente;

    ////cerr << "\n\ttJUG adelantado: " << media_color_adelantado_jug << " JUG atrasado: " << media_color_atrasado_jug << endl;
    //cerr << "\tPuntuacion distancia jugador: " << puntuacion_distancia_jugador << endl;

    ////cerr << "\tOPO adelantado: " << media_color_adelantado_opo << " OPO atrasado: " << media_color_atrasado_opo << endl;
    //cerr << "\tPuntuacion distancia oponente: " << puntuacion_distancia_oponente << endl;

    //cerr << "Puntuación final tras distancia: " << puntuacion_final << endl << endl;

    // -------------------------------------------------------------------------------------------
    // PUNTUACION RIESGO

    //vector<double> media_peligro_color = media_peligro_colores(estado);
    //double puntuacion_peligro_jugador = media_peligro_color[color_adelantado_jug]*PESO_C_GOOD + media_peligro_color[color_atrasado_jug]*PESO_C_BAD;
    //double puntuacion_peligro_oponente = media_peligro_color[color_adelantado_opo]*PESO_C_GOOD + media_peligro_color[color_atrasado_opo]*PESO_C_BAD;
    //puntuacion_final -= (puntuacion_peligro_jugador - puntuacion_peligro_oponente);

    //cerr << "\\tPuntuacion peligro jugador: " << puntuacion_peligro_jugador << endl;
    //cerr << "\tPuntuacion peligro oponente: " << puntuacion_peligro_oponente << endl;

    //cerr << "Puntuación final tras peligro: " << puntuacion_final << endl << endl;

    // -------------------------------------------------------------------------------------------
    // PUNTUACION POR DADOS DISPONIBLES

    //const double media_dados_disponibles_jug = media_dados_disponibles(estado, jugador);
    //const double media_dados_disponibles_opo = media_dados_disponibles(estado, oponente);
    //puntuacion_final += (media_dados_disponibles_jug - media_dados_disponibles_opo);

    //cerr << "\tMedia dados disponibles jugador: " << media_dados_disponibles_jug << endl;
    //cerr << "\tMedia dados disponibles oponente: " << media_dados_disponibles_opo << endl;

    //cerr << "Puntuación final tras dados: " << puntuacion_final << endl << endl;

    // -------------------------------------------------------------------------------------------

    // QUIESCENCIA
    // la razón por la que uso el mejor dado del opo es porque le estoy "robando" un turno
    //const unsigned char extra_por_barrera = estado.w ? 10 + mejor_dado_disponible(estado, oponente) : 0;
    //const double extra_por_seis = estado.getLastDice() == 6 ? mejor_dado_disponible(estado, oponente) : 0;
    //const unsigned char extra_por_comer = estado.isEatingMove() ? 20 + mejor_dado_disponible(estado, oponente) : 0;
    //puntuacion_final += extra_por_comer + extra_por_seis;

    //if (extra_por_comer)
    //    cerr << ">>>>>>>>>>>>>>>>>>> Puntuacion extra por comer" << endl;
    //if (extra_por_seis)
    //    cerr << ">>>>>>>>>>>>>>>>>>> Puntuacion extra por seis" << endl;

    // -------------------------------------------------------------------------------------------

    //cerr << "Puntuacion final (tras quiescencia): " << puntuacion_final << endl;
    return puntuacion_final;
}
*/
double AIPlayer::evaluacion_optima(const Parchis &estado, int jugador)
{
    //cerr << endl << endl << "----------------- EVALUANDO ACCION -----------------" << endl;
    //cerr << "Color: " << str(get<0>(estado.getLastAction())) << " Ficha: " << estado.getBoard().getPiece(get<0>(estado.getLastAction()), get<1>(estado.getLastAction())).get_box().num << " Dado: " << get<2>(estado.getLastAction()) << endl << endl;

    double puntuacion_final = 0;
    double puntuaciones_colores[4] = {0, 0, 0, 0};
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;
    vector<color> colores_jug = estado.getPlayerColors(jugador);
    vector<color> colores_opo = estado.getPlayerColors(oponente);

    // -------------------------------------------------------------------------------------------
    // EN CASO DE GANAR / PERDER O ENERGIA

    if (ganador == jugador) return gana;
    else if (ganador == oponente) return pierde;

    //cerr << "Puntuación final tras ganador/perdedor: " << puntuacion_final << endl;

    // -------------------------------------------------------------------------------------------
    // SUMO LOS PUNTOS DE LAS CASILLAS DE META PARA CADA COLOR

    list<pair<color, int>> piezas_jug, piezas_jug_vulnerables;
    for (auto &c : colores_jug)
    {
        piezas_jug.push_back(make_pair(c, 0));
        piezas_jug.push_back(make_pair(c, 1));
        piezas_jug.push_back(make_pair(c, 2));
    }
    piezas_jug_vulnerables = piezas_jug;

    list<pair<color, int>> piezas_opo, piezas_opo_vulnerables;
    for (auto &c : colores_opo)
    {
        piezas_opo.push_back(make_pair(c, 0));
        piezas_opo.push_back(make_pair(c, 1));
        piezas_opo.push_back(make_pair(c, 2));
    }
    piezas_opo_vulnerables = piezas_opo;

    //for (int c = 0; c < num_colores; c++)
    //    puntuaciones_colores[c] += estado.piecesAtGoal((color) c) * PUNTOS_FICHA_EN_META;

    for (auto p = piezas_jug_vulnerables.begin(); p != piezas_jug_vulnerables.end();)
    {
        Box casilla = estado.getBoard().getPiece(p->first, p->second).get_box();
        if (casilla.type == goal)
        {
            puntuaciones_colores[p->first] += PUNTOS_FICHA_EN_META;
            p = piezas_jug_vulnerables.erase(p);
        }
        else if (casilla.type == home)
        {
            puntuaciones_colores[p->first] -= PENALIZACION_HOME;
            p = piezas_jug_vulnerables.erase(p);
        }
        else
            ++p;
    }

    for (auto p = piezas_opo_vulnerables.begin(); p != piezas_opo_vulnerables.end();)
    {
        Box casilla = estado.getBoard().getPiece(p->first, p->second).get_box();
        if (casilla.type == goal)
        {
            puntuaciones_colores[p->first] += PUNTOS_FICHA_EN_META;
            p = piezas_opo_vulnerables.erase(p);
        }
        else if (casilla.type == home)
        {
            puntuaciones_colores[p->first] -= PENALIZACION_HOME;
            p = piezas_opo_vulnerables.erase(p);
        }
        else
            ++p;
    }

    // -------------------------------------------------------------------------------------------
    // PUNTUACIÓN SEGÚN AVANCE

    for (int c = 0; c < num_colores; c++)
        puntuaciones_colores[c] += avance_color(estado, c);

    // -------------------------------------------------------------------------------------------
    // PUNTUACIÓN SEGÚN RIESGO
    
    // Sumo el beneficio de tener casilla segura y elimino dichas casillas de las piezas
    for (auto p = piezas_jug_vulnerables.begin(); p != piezas_jug_vulnerables.end();)
    {
        Box casilla = estado.getBoard().getPiece(p->first, p->second).get_box();
        if (estado.isSafeBox(casilla) or casilla.type != normal or estado.isWall(casilla) != none or estado.getBoard().getPiece(p->first, p->second).get_type() == star_piece)
        {
            puntuaciones_colores[p->first] += (NUM_TOTAL_CASILLAS - estado.distanceToGoal(p->first, p->first))*BENEFICIO_CASILLA_SEGURA;
            p = piezas_jug_vulnerables.erase(p);
        }
        else
            ++p;
    }

    for (auto p = piezas_opo_vulnerables.begin(); p != piezas_opo_vulnerables.end();)
    {
        Box casilla = estado.getBoard().getPiece(p->first, p->second).get_box();
        if (estado.isSafeBox(casilla) or casilla.type != normal or estado.isWall(casilla) != none or estado.getBoard().getPiece(p->first, p->second).get_type() == star_piece)
        {
            puntuaciones_colores[p->first] += (NUM_TOTAL_CASILLAS - estado.distanceToGoal(p->first, p->first))*BENEFICIO_CASILLA_SEGURA;
            p = piezas_opo_vulnerables.erase(p);
        }
        else
            ++p;
    }


    // Si repito turno en el siguiente, no hay peligro
    if (estado.isGoalMove() or estado.isEatingMove())
    {
        // 20 por la energía y otros 20 por el boost de comer
        puntuaciones_colores[colores_jug[0]] += estado.isEatingMove() ? BENEFICIO_COMER : 40;
        puntuaciones_colores[colores_jug[1]] += estado.isEatingMove() ? BENEFICIO_COMER : 40;
    }
    //Sino calculo el riesgo de cada color según las piezas comibles
    //else
    //{
    //    vector<double> riesgo_colores = penalizacion_por_riesgo(estado, piezas_jug, piezas_jug_vulnerables, piezas_opo, piezas_opo_vulnerables);
    //    for (int c = 0; c < num_colores; c++)
    //        puntuaciones_colores[c] -= riesgo_colores[c];
    //}
    
    // -------------------------------------------------------------------------------------------
    // PONDERACIÓN POR JUGADORES

    color c_good_jug    = puntuaciones_colores[colores_jug[0]] > puntuaciones_colores[colores_jug[1]] ? colores_jug[0] : colores_jug[1];
    color c_bad_jug     = c_good_jug == colores_jug[0] ? colores_jug[1] : colores_jug[0];

    color c_good_opo    = puntuaciones_colores[colores_opo[0]] > puntuaciones_colores[colores_opo[1]] ? colores_opo[0] : colores_opo[1];
    color c_bad_opo     = c_good_opo == colores_opo[0] ? colores_opo[1] : colores_opo[0];

    double puntos_jug   = puntuaciones_colores[c_good_jug]*PESO_C_GOOD + puntuaciones_colores[c_bad_jug]*PESO_C_BAD;
    double puntos_opo   = puntuaciones_colores[c_good_opo]*PESO_C_GOOD + puntuaciones_colores[c_bad_opo]*PESO_C_BAD;

    // Sumo y resto la energía
    vector<int> dados_jug = estado.getAvailableNormalDices(jugador);
    int power_jug = estado.getPower(jugador);
    bool solo_un_dado_y_bomba = (dados_jug.size() == 1 and dados_jug[0] == 100) and (power == 100 or (power >= 90 and power < 95) or (power >= 80 and power < 85) or (power >= 60 and power < 65));
    puntos_jug += estado.getPower(jugador) == 100 or solo_un_dado_y_bomba ? -(estado.getPower(jugador)*5) : estado.getPower(jugador) * 0.5;

    vector<int> dados_opo = estado.getAvailableNormalDices(oponente);
    int power_opo = estado.getPower(oponente);
    bool solo_un_dado_y_bomba_opo = (dados_opo.size() == 1 and dados_opo[0] == 100) and (power_opo == 100 or (power_opo >= 90 and power_opo < 95) or (power_opo >= 80 and power_opo < 85) or (power_opo >= 60 and power_opo < 65));
    puntos_opo += estado.getPower(oponente) == 100 or solo_un_dado_y_bomba_opo ? -(estado.getPower(oponente)*5) : estado.getPower(oponente) * 0.5;

    if (puntos_jug >= gana or puntos_opo <= pierde)
    {
        cerr << "ERROR: PUNTUACION EXCESIVA" << endl;
        exit(1);
    }

    return puntos_jug - puntos_opo;
}
/*
double AIPlayer::penalizacion_por_riesgo(const Parchis &estado, const int c, const vector<pair<color, int>> &p_comibles)
{
    // 1. Si es segura devuelvo o estrella o meta devuelvo 0
    // 2. Si es comible devuelvo su valor íntegro
    // 3. Si no es comible su valor reducido por el número de dados restantes
    double penalizacion = 0;
    vector<color> colores_jug = estado.getPlayerColors(c);
    int oponente = c == colores_jug[0] or c == colores_jug[1] ? (estado.getCurrentPlayerId()+1) % 2 : estado.getCurrentPlayerId();

    for (int i = 0; i < num_pieces; i++)
    {
        double valor_ficha = NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, 0);
        for (const auto &p : p_comibles)
        {
            if (p.first == (color) c and p.second == i)
            {
                penalizacion += valor_ficha;
                break;
            }
        }
        //else
        //    penalizacion += valor_ficha * PESO_RIESGO; //((7-estado.getAvailableNormalDices(oponente).size())*PESO_RIEGO_DADOS);
    }


    //penalizacion += estado.isSafePiece((color) c, 0) ? 0 : (NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, 0))*PESO_RIESGO;
    //penalizacion += estado.isSafePiece((color) c, 1) ? 0 : (NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, 1))*PESO_RIESGO;
    //penalizacion += estado.isSafePiece((color) c, 2) ? 0 : (NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, 2))*PESO_RIESGO;
    return penalizacion;
}
*/
double AIPlayer::avance_color(const Parchis &estado, const int c)
{
    double avance_color = 0;
    avance_color += (NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, 0));
    avance_color += (NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, 1));
    avance_color += (NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, 2));
    return avance_color;
}

double AIPlayer::media_avance_color(const Parchis &estado, const int c)
{
    return (avance_color(estado, c) / num_pieces);
}

vector<double> AIPlayer::penalizacion_por_riesgo(const Parchis &estado, const list<pair<color, int>> &piezas_jug, const list<pair<color, int>> &piezas_jug_vulnerables, const list<pair<color, int>> &piezas_opo, const list<pair<color, int>> &piezas_opo_vulnerables)
{
    vector<double> riesgo_color = {0, 0, 0, 0};
    list<pair<color, int>> piezas_comibles_opo, piezas_jug_amenaza = piezas_jug, piezas_opo_amenaza = piezas_opo;

    int jugador = estado.getCurrentPlayerId();
    vector<color> colores_jug = estado.getPlayerColors(jugador);
    vector<int> mov_jugador = estado.getAvailableNormalDices(jugador);
    bool tiene_dado_6 = false;
    for (auto &m : mov_jugador)
        tiene_dado_6 = m == 6 or tiene_dado_6;

    int oponente = (jugador+1) % 2;
    vector<color> colores_opo = estado.getPlayerColors(oponente);
    vector<int> mov_oponente = estado.getAvailableNormalDices(oponente);
    bool tiene_dado_6_opo = false;
    for (auto &m : mov_oponente)
        tiene_dado_6_opo = m == 6 or tiene_dado_6_opo;

    // Piezas comibles del oponente por el jugador en el estado actual
    for (auto p_opo : piezas_opo_vulnerables)
    {
        bool comible = false;
        double valor_ficha = NUM_TOTAL_CASILLAS - estado.distanceToGoal(p_opo.first, p_opo.second);
        Box casilla_oponente = estado.getBoard().getPiece(p_opo.first, p_opo.second).get_box();

        for (auto p_jug : piezas_jug_amenaza)
        {
            Box casilla_jugador = estado.getBoard().getPiece(p_jug.first, p_jug.second).get_box();
            int distancia = estado.distanceBoxtoBox(p_jug.first, casilla_jugador, casilla_oponente);
            if (distancia > 0)
            {
                for (auto &mov : mov_jugador)
                {
                    Box casilla_resultante = estado.computeSpecialMove(p_jug.first, casilla_jugador, mov);
                    int distancia_tras_mov = estado.distanceBoxtoBox(p_jug.first, casilla_jugador, casilla_resultante);
                    bool asesinable_por_estrella = distancia_tras_mov >= distancia and estado.getBoard().getPiece(p_jug.first, p_jug.second).get_type() == star_piece;
                    if (comible = (distancia == distancia_tras_mov or (tiene_dado_6 and mov != 6 and distancia == (distancia_tras_mov + 6)) or asesinable_por_estrella))
                    {
                        //cerr << str(p_opo.first) << " " << casilla_oponente.num << " <--------- " << str(p_jug.first) << " " << casilla_jugador.num << endl;
                        riesgo_color[p_opo.first] += valor_ficha + PENALIZACION_MORIR;
                        piezas_comibles_opo.push_back(p_opo);
                        break;
                    }
                }
            }
            if (comible) break;
        }
    }


    // Piezas comibles del jugador por el oponente en el estado actual
    // Saco de las piezas_opo_amenaza aquella que he movido en el último turno, puesto que ya no puede comer
    for (auto p = piezas_opo_amenaza.begin(); p != piezas_opo_amenaza.end(); )
    {
        if (p->first == get<0>(estado.getLastAction()) and p->second == get<1>(estado.getLastAction()))
            p = piezas_opo_amenaza.erase(p);
        else
            ++p;
    }
    // Elimino de la lista de piezas de oponente (y por tanto lista de amenazas) aquellas piezas que son comibles
    // Puesto que para cuando me intente comer ya me lo habré comido yo a él, por esta razón es importante que se haga primero la comprobación de las piezas comibles del oponente
    for (auto p_comible_opo : piezas_comibles_opo)
    {
        for (auto p_opo = piezas_opo_amenaza.begin(); p_opo != piezas_opo_amenaza.end(); )
        {
            if (*p_opo == p_comible_opo)
                p_opo = piezas_opo_amenaza.erase(p_opo);
            else
                ++p_opo;
        }
    }
    for (auto& p_jug : piezas_jug_vulnerables)
    {
        bool comible = false;
        double valor_ficha = NUM_TOTAL_CASILLAS - estado.distanceToGoal(p_jug.first, p_jug.second);
        Box casilla_jugador = estado.getBoard().getPiece(p_jug.first, p_jug.second).get_box();

        for (auto& p_opo : piezas_opo_amenaza)
        {
            Box casilla_oponente = estado.getBoard().getPiece(p_opo.first, p_opo.second).get_box();
            int distancia = estado.distanceBoxtoBox(p_opo.first, casilla_oponente, casilla_jugador);
            if (distancia > 0)
            {
                for (auto &mov : mov_oponente)
                {
                    Box casilla_resultante = estado.computeSpecialMove(p_opo.first, casilla_oponente, mov);
                    int distancia_tras_mov = estado.distanceBoxtoBox(p_opo.first, casilla_oponente, casilla_resultante);
                    bool asesinable_por_estrella = distancia_tras_mov >= distancia and estado.getBoard().getPiece(p_opo.first, p_opo.second).get_type() == star_piece;
                    if (comible = (distancia == distancia_tras_mov or (tiene_dado_6_opo and mov != 6 and distancia == (distancia_tras_mov + 6)) or asesinable_por_estrella))
                    {
                        //cerr << str(p_jug.first) << " " << casilla_jugador.num << " <--------- " << str(p_opo.first) << " " << casilla_oponente.num << endl;
                        riesgo_color[p_jug.first] += valor_ficha + PENALIZACION_MORIR;
                        break;
                    }
                }
            }
            if (comible) break;
        }
    }

    return riesgo_color;
}
/*
double AIPlayer::media_dados_disponibles(const Parchis &estado, const int jugador)
{
    int oponente = (jugador+1) % 2;
    double media = 0;
    vector<int> dados_disponibles = estado.getNormalDices(jugador);
    for (const auto &dado : dados_disponibles)
    {
        //cerr << "\tDado: " << dado << endl;
        if (dado == 6)
            media += 6 + mejor_dado_disponible(estado, oponente);
        else if (dado == 100)
            media += BENEFICIO_POSESION_DADO_ESPECIAL;
        else
            media += dado;
    }
    return media; // dados_disponibles.size();
}

double AIPlayer::mejor_dado_disponible(const Parchis &estado, const int jugador)
{
    int oponente = (jugador+1) % 2;
    vector<int> dados_disponibles = estado.getNormalDices(jugador);
    double mejor_hasta_ahora = *max_element(dados_disponibles.begin(), dados_disponibles.end());
    for (const auto &dado : dados_disponibles)
    {
        if (dado == 100)
            return BENEFICIO_POSESION_DADO_ESPECIAL;
        else if (dado == 6)
            mejor_hasta_ahora = BENEFICIO_POSESION_DADO_6;
    }

    return mejor_hasta_ahora;
}
*/
/*
vector<double> AIPlayer::medias_avance_colores(const Parchis &estado)
{
    vector<double> media_avance_color(4, 0);
    for (int c = 0; c < 4; c++)
    {
        for (int i = 0; i < num_pieces; i++)
            media_avance_color[c] += (NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, i));

        media_avance_color[c] /= num_pieces;
        //cerr << "\tMedia color " << c << ": " << media_avance_color[c] << endl;
    }
    return media_avance_color;
}

int AIPlayer::distancia_enemigo_mas_cercano(const Parchis &estado, const color c_jug, const int id_jug)
{
    int distancia_enemigo_mas_cercano = -1;
    const vector<color> colores_opo = c_jug % 2 != 0 ? vector<color>{blue, green} : vector<color>{red, yellow};

    for (color c_opo : colores_opo)
    {
        for (int id_opo = 0; id_opo < num_pieces; id_opo++)
        {
            const Box &box_jug = estado.getBoard().getPiece(c_jug, id_jug).get_box();
            const Box &box_opo = estado.getBoard().getPiece(c_opo, id_opo).get_box();
            int distancia_enemigo_i = estado.distanceBoxtoBox(c_jug, box_opo, box_jug);

            if (distancia_enemigo_mas_cercano > distancia_enemigo_i or distancia_enemigo_mas_cercano == -1)
                distancia_enemigo_mas_cercano = distancia_enemigo_i;
        }
    }

    return distancia_enemigo_mas_cercano;
}
*/
bool AIPlayer::is_goal(const Parchis &estado, const int c, const int id)
{
    return estado.getBoard().getPiece((color) c, id).get_box().type == goal;
}

/*
double AIPlayer::MiValoracion1(const Parchis &estado, int jugador){
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;
    const int NUMCASILLAS = 68 + 7; //numero de casillas hasta la meta

    pair<color, int> piezaComida; //para saber que pieza se ha comido
    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador){
        return gana;
    }
    else if (ganador == oponente){
        return pierde;
    }
    else{ //Si no hay ganador valoramos los 2 jugadores
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        //para mi jugador
        double puntuacion_jugador = 0.0;
        //si es el que se ha movido
        if(estado.getCurrentPlayerId() == jugador){
            if(estado.isEatingMove()){ //si ha comido ficha
                pair<color, int> piezaComida = estado.eatenPiece();
                //si ha comido una pieza de su jugador y ese color estaba más cerca de la meta
                if(piezaComida.first == my_colors[0] || piezaComida.first == my_colors[1]){
                    if(piezaComida.first == my_colors[0]){ // si se ha comido a la primera ficha
                        if(estado.piecesAtGoal(op_colors[0]) >0 ) {//Caso: Si se ha comido al color que tiene piezas en la meta 
                            puntuacion_jugador-=3;
                        }else {
                            puntuacion_jugador+=1;
                        } 
                           //QUIŹAS habría que hacer un máximo estado.piecesAtGoal(op_colors[0]) > estado.piecesAtGoal(op_colors[1]) 
                            //en ese caso por ejemplo quitar 5 Idk
                    }
                    else if (piezaComida.first == my_colors[1]){ //si se ha comido a la 2º ficha
                       if(estado.piecesAtGoal(op_colors[1])>0 ) {//se comprueba que el que esté comiendo sea el que esté más adelantado
                            puntuacion_jugador-=3;
                        }else {
                            puntuacion_jugador+=1;
                        }  //puedo meter que lo contrario sí le de puntos, ya que de esta forma si se come así mismo por beneficio
                        // no le incrementa puntos.
                    }
                }
                else
                    puntuacion_jugador += 80;
            }
            else if(estado.isGoalMove()) //si ha metido ficha
                puntuacion_jugador += 40;
            else{
                //si ha destruido pieza del rival suma, si destruye suyas resta
                auto piezasDestruidas = estado.piecesDestroyedLastMove();
                if(piezasDestruidas.size() > 0)
                    for(auto it = piezasDestruidas.begin(); it != piezasDestruidas.end(); ++it){
                        if((*it).first == my_colors[0] || (*it).first == my_colors[1])
                            puntuacion_jugador -= 30;
                        else
                            puntuacion_jugador += 30;
                    }
                //si ha cogido objeto suma
                else if(estado.getItemAcquired() != -1)
                    puntuacion_jugador += 15;
                //si revota en la meta resta
                else if(estado.goalBounce())
                    puntuacion_jugador -= 2;
            }
        }
        //recorremos las fichas
        for (int i = 0; i < my_colors.size(); i++){
            color c = my_colors[i];
            //valoramos negativamente piezas en la casa
            puntuacion_jugador -= estado.piecesAtHome(c) * 2;
            // Recorro las fichas de ese color, valoramos positivamente tener casillas de ese color en casa
            for (int j = 0; j < num_pieces; j++) {
                puntuacion_jugador += NUMCASILLAS - estado.distanceToGoal(c,j) + estado.piecesAtGoal(c) * 6;
            }
        }

        //  for (int i = 0; i < my_colors.size(); i++)
        // {
        //     color c = my_colors[i];
        //     // Recorro las fichas de ese color.
        //     for (int j = 0; j < num_pieces; j++)
        //     {
        //         // Valoro positivamente que la ficha esté en casilla segura o meta.
        //         if (estado.isSafePiece(c, j))
        //         {
        //             puntuacion_jugador++;
        //         }
        //         else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
        //         {
        //             puntuacion_jugador += 5;
        //         }
        //     }
        // }


        // Recorro todas las fichas del oponente, se valora igual que el jugador
        double puntuacion_oponente = 0.0;
        if(estado.getCurrentPlayerId() == oponente){
            if(estado.isEatingMove()){
                pair<color, int> piezaComida = estado.eatenPiece();
               if(piezaComida.first == op_colors[0] || piezaComida.first == op_colors[1]){

                    if(piezaComida.first == op_colors[0]){ // si se ha comido a la primera ficha
                        if(estado.piecesAtGoal(op_colors[0]) >0 ) {//se comprueba que el que esté comiendo sea el que esté más adelantado
                            puntuacion_oponente-=3;
                        }       
                    }
                    else if (piezaComida.first == op_colors[1]){ //si se ha comido a la 2º ficha
                       if(estado.piecesAtGoal(op_colors[1]) >0 ) {//se comprueba que el que esté comiendo sea el que esté más adelantado
                            puntuacion_oponente-=3;
                        }  
                    }
                }
                else
                    puntuacion_oponente += 80;
            }
            else if(estado.isGoalMove())
                puntuacion_oponente += 40;
            else{
                auto piezasDestruidas = estado.piecesDestroyedLastMove();
                if(piezasDestruidas.size() > 0)
                    for(auto it = piezasDestruidas.begin(); it != piezasDestruidas.end(); ++it){
                        if((*it).first == my_colors[0] || (*it).first == my_colors[1])
                            puntuacion_oponente -= 30;
                        else
                            puntuacion_oponente += 30;
                    }
                else if(estado.getItemAcquired() != -1)
                    puntuacion_oponente += 20;   //SI LO COJE EL OPONENTE LE SUMO 20, pq lo considero todavía más listo que yo xd
                else if(estado.goalBounce())
                    puntuacion_oponente -= 2;
            }
        }
        for (int i = 0; i < op_colors.size(); i++){
            color c = op_colors[i];
            puntuacion_oponente -= estado.piecesAtHome(c) * 2;
            for (int j = 0; j < num_pieces; j++){
                puntuacion_oponente += NUMCASILLAS - estado.distanceToGoal(c,j) + estado.piecesAtGoal(c) * 6;
            }
        }

        // for (int i = 0; i < op_colors.size(); i++)
        // {
        //     color c = op_colors[i];
        //     // Recorro las fichas de ese color.
        //     for (int j = 0; j < num_pieces; j++)
        //     {
        //         if (estado.isSafePiece(c, j))
        //         {
        //             // Valoro negativamente que la ficha esté en casilla segura o meta.
        //             puntuacion_oponente++;
        //         }
        //         else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
        //         {
        //             puntuacion_oponente += 5;
        //         }
        //     }
        // }
        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}
*/

/*
priority_queue<Parchis, vector<Parchis>, ordenador_estados> AIPlayer::get_hijos_orden_heuristica(const Parchis &estado) const
{
    priority_queue<Parchis, vector<Parchis>, ordenador_estados> hijos;
    ParchisBros hijos_aux = estado.getChildren();
    for (const auto &hijo : hijos_aux)
        hijos.push(hijo);

    return hijos;
}

queue<Parchis> AIPlayer::get_hijos_orden_dados(const Parchis &estado) const
{
    queue<Parchis> hijos;
    ParchisBros hijos_aux = estado.getChildren();
    for (auto it = hijos_aux.begin(); it != hijos_aux.end(); ++it)
        hijos.push(*it);
    return hijos;
}

cola_hijos_raiz AIPlayer::get_hijos_orden_dados_raiz(const Parchis &estado) const
{
    cola_hijos_raiz hijos;
    ParchisBros hijos_aux = estado.getChildren();
    for (auto it = hijos_aux.begin(); it != hijos_aux.end(); ++it)
        hijos.push({(double) it.getMovedDiceValue(), (*it), {it.getMovedColor(), it.getMovedPieceId(), it.getMovedDiceValue()}});

    return hijos;
}
*/

/*
bool ordenador_estados::operator()(const Parchis &a, const Parchis &b) const
{
    if (a.getCurrentPlayerId() == JUGADOR_PRINCIPAL)
        return heuristica(a, a.getCurrentPlayerId()) > heuristica(b, b.getCurrentPlayerId());
    else
        return heuristica(a, a.getCurrentPlayerId()) < heuristica(b, b.getCurrentPlayerId());
}

bool ordenador_estados::operator()(const pair<Parchis, movimiento> &a, const pair<Parchis, movimiento> &b) const
{
    const Parchis& estadoA = a.first;
    const Parchis& estadoB = b.first;
    if (estadoA.getCurrentPlayerId() == JUGADOR_PRINCIPAL)
        return heuristica(estadoA, estadoA.getCurrentPlayerId()) > heuristica(estadoB, estadoB.getCurrentPlayerId());
    else
        return heuristica(estadoA, estadoA.getCurrentPlayerId()) < heuristica(estadoB, estadoB.getCurrentPlayerId());
}

bool ordenador_estados::operator()(const tuple<double, Parchis, movimiento>& a, const tuple<double, Parchis, movimiento>& b) const
{
    return get<0>(a) < get<0>(b);
}
*/

/*
vector<double> AIPlayer::media_peligro_colores(const Parchis &estado)
{
    int jugador = estado.getCurrentPlayerId();
    int oponente = (jugador+1) % 2;
    vector<double> media_peligro_color(4, 0);
    unordered_set<int> comibles = piezas_comibles(estado);
    int dist_opo_mas_peligroso;

    for (int c = 0; c < 4; c++)
    {
        for (int id = 0; id < num_pieces; id++)
        {
            int valor_ficha = NUM_TOTAL_CASILLAS - estado.distanceToGoal((color) c, id);

            if (comibles.find(estado.getBoard().getPiece((color) c, id).get_box().num) != comibles.end())
            {
                media_peligro_color[c] += valor_ficha + 20 + mejor_dado_disponible(estado, jugador);
            }
            else if ((dist_opo_mas_peligroso = distancia_enemigo_mas_cercano(estado, (color) c, id)) != -1)
            {
                double turnos_opo_mas_peligroso = max(5.0, dist_opo_mas_peligroso / 5.0);
                media_peligro_color[c] += valor_ficha - (valor_ficha * 0.2 * turnos_opo_mas_peligroso);
            }
        }
        media_peligro_color[c] /= num_pieces;
    }
    return media_peligro_color;
}
*/