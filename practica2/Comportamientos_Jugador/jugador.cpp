#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>

pair<int, int> ComportamientoJugador::traductor_posicion(const ubicacion &pos, const int offset_fil, const int offset_col) const
{
	short posfil, poscol;
	bool swap;

	switch (pos.brujula)
	{
	case norte: 	posfil = 1; 	poscol = 1;		swap = false;	break;
	case noreste: 	posfil = 1; 	poscol = 1; 	swap = false;	break;
	case oeste: 	posfil = 1; 	poscol = -1; 	swap = true;	break;
	case noroeste: 	posfil = 1; 	poscol = -1; 	swap = true;	break;
	case este: 		posfil = -1; 	poscol = 1;		swap = true;	break;
	case sureste: 	posfil = -1; 	poscol = 1; 	swap = true;	break;
	case sur: 		posfil = -1; 	poscol = -1; 	swap = false;	break;
	case suroeste: 	posfil = -1; 	poscol = -1; 	swap = false;	break;
	}

	short posfil_tmp = swap ? offset_col * poscol : offset_fil * poscol;
	short poscol_tmp = swap ? offset_fil * posfil : offset_col * posfil;

	return make_pair(pos.f + posfil_tmp, pos.c + poscol_tmp);
}

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;

	if (!hayPlan)
	{
		// Invocar al método de búsqueda
		cerr << "Calculamos un nuevo plan" << endl;
		c_state.jugador.f = sensores.posF;
		c_state.jugador.c = sensores.posC;
		c_state.jugador.brujula = sensores.sentido;
		c_state.colaborador.f = sensores.CLBposF;
		c_state.colaborador.c = sensores.CLBposC;
		c_state.colaborador.brujula = sensores.CLBsentido;
		goal.f = sensores.destinoF;
		goal.c = sensores.destinoC;

		hayPlan = anchura_solo_jugador(c_state, goal, mapaResultado);
		if (hayPlan) cout << "Se encontró un plan" << endl;
	}

	if (hayPlan and plan.size() > 0)
	{
		accion = plan.front();
		plan.pop_front();
	}

	if (plan.size() == 0)
	{
		cerr << "He terminado el plan" << endl;
		hayPlan = false;
	}

	return accion;
}

//---------------------- Implementación de la busqueda en anchura ---------------------------
bool ComportamientoJugador::casilla_transitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa) const
{
	return (mapa[x.f][x.c] != 'P' and mapa[x.f][x.c] != 'M');
}

ubicacion ComportamientoJugador::next_casilla(const ubicacion &pos, const int offset) const
{
	return {traductor_posicion(pos, -1*offset, 0).first, traductor_posicion(pos, -1*offset, 0).second, pos.brujula};
}

bool ComportamientoJugador::Find(const estado &item, const list<estado> &lista) const
{
	auto it = lista.begin();
	while (it != lista.end() and !((*it) == item))
		it++;

	return (!(it == lista.end()));
}

estado ComportamientoJugador::apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const
{
	estado st_result = st;
	ubicacion sig_ubicacion, sig_ubicacion2;
	switch (a)
	{
	case actWALK:
		sig_ubicacion = next_casilla(st.jugador);
		if (!casilla_transitable(sig_ubicacion, mapa) and !(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c))
			st_result.jugador = sig_ubicacion;
		break;
	
	case actRUN:
		sig_ubicacion = next_casilla(st.jugador);
		if (!casilla_transitable(sig_ubicacion, mapa) and !(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c))
		{
			sig_ubicacion2 = next_casilla(sig_ubicacion);
			if (!casilla_transitable(sig_ubicacion2, mapa) and !(sig_ubicacion2.f == st.colaborador.f and sig_ubicacion2.c == st.colaborador.c))
				st_result.jugador = sig_ubicacion2;
		}
		break;

	case actTURN_L:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+6)%8);
		break;

	case actTURN_SR:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+1)%8);
		break;
	}

	return st_result;
}

bool ComportamientoJugador::anchura_solo_jugador (const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa)
{
	estado current_state = inicio;
	list<estado> frontier;
	list<estado> explored;
	bool solution_found = current_state.jugador.f == final.f and current_state.jugador.c == final.c;

	while (!frontier.empty() and !solution_found)
	{
		frontier.pop_front();
		explored.push_back(current_state);

		// Hijo de andar
		vector<Action> actions = {actWALK, actTURN_L, actTURN_SR};

		estado child;
		for (const auto &action : actions)
		{
			child = apply(action, current_state, mapa);

			if (child.jugador.f == final.f and child.jugador.c == final.c)
			{
				current_state = child;
				solution_found = true;
				break;
			}
			else if (!Find(child, frontier) and !Find(child, explored))
				frontier.push_back(child);
		}

		if (!solution_found and !frontier.empty())
			current_state = frontier.front();
	}	

	return solution_found;
}

void ComportamientoJugador::anula_matriz(vector<vector<unsigned char>> & matriz)
{
	for (int i = 0; i < matriz.size(); i++)
	{
		for (int j = 0; j < matriz[0].size(); j++)
		{
			matriz[i][j] = 0;
		}
	}
	
}

void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	anula_matriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if ((*it != act_CLB_WALK) and (*it != act_CLB_TURN_SR) and (*it != act_CLB_STOP))
		{
			switch (cst.ultima_orden_colaborador)
			{
			case act_CLB_WALK:
				cst.colaborador = next_casilla(cst.colaborador);
				mapaConPlan[cst.colaborador.f][cst.colaborador.c] = 2;
				break;
			
			case act_CLB_TURN_SR:
				cst.colaborador.brujula = (Orientacion)((cst.colaborador.brujula + 1) % 8);
				break;
			default:
				break;
			}
		}

		switch (*it)
		{
		case actRUN:
			cst.jugador = next_casilla(cst.jugador);
			mapaConPlan[cst.jugador.f][cst.jugador.c] = 3;
			cst.jugador = next_casilla(cst.jugador);
			mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
			break;
		
		case actWALK:
			cst.jugador = next_casilla(cst.jugador);
			mapaConPlan[cst.jugador.f][cst.jugador.c] = 3;
			cst.jugador = next_casilla(cst.jugador);
			mapaConPlan[cst.jugador.f][cst.jugadro.c] = 1;
			break;

		case actTURN_SR:
			cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 1) % 8);
			break;

		case actTURN_L:
			cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 6) % 8);
			break;

		case act_CLB_TURN_SR:
			cst.colaborador.brujula = (Orientacion)((cst.colaborador.brujula + 1) % 8);
			cst.ultima_orden_colaborador = act_CLB_TURN_SR;
			break;

		case act_CLB_STOP:
			cst.ultima_orden_colaborador = act_CLB_STOP;
			break;
		}
		it++;
	}
	
}
int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
