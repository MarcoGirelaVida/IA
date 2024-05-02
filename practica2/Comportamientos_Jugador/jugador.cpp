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

bool ComportamientoJugador::es_solucion (const ubicacion &ub, const ubicacion &final) const
{
	return (ub.f == final.f and ub.c == final.c);
}

//template <typename T>
void ComportamientoJugador::mostrar_lista(const list<nodo> &l) const
{
	auto it = l.begin();
	while (it != l.end())
	{
		cerr << "{";
		PintaPlan(it->secuencia);
		cerr << "}, ";
		it++;
	}
}

void ComportamientoJugador::mostrar_ubicacion(const ubicacion &ub) const
{
	cerr << "Pos: [" << ub.f << "][" << ub.c << "]  Orientación: ";
	orientacion_string(ub.brujula);
	cerr << endl;
}

void ComportamientoJugador::mostrar_estado(const estado &st) const
{
	cerr << "Jugador: " << endl << "\t";
	mostrar_ubicacion(st.jugador);
	cerr << "Colaborador: " << endl << "\t";
	mostrar_ubicacion(st.colaborador);

	cerr << "Ultima orden colaborador: ";
	accion_string(st.ultima_orden_colaborador);
	cerr << endl;
}

void ComportamientoJugador::mostrar_nodo(const nodo &nd) const
{
	mostrar_estado(nd.st);
	cerr << endl << "Secuencia: ";
	PintaPlan(nd.secuencia);
}

void ComportamientoJugador::PintaPlan(const list<Action> &plan) const
{
	auto it = plan.begin();
	while (it != plan.end())
	{
		accion_string(*it);
		it++;
	}

	cout << "(size: " << plan.size() << ")";
}

void ComportamientoJugador::orientacion_string (const Orientacion &o) const
{
	switch (o)
	{
	case norte: 	cerr << "Norte"; 	break;
	case noreste: 	cerr << "Noreste"; 	break;
	case este: 		cerr << "Este"; 	break;
	case sureste: 	cerr << "Sureste"; 	break;
	case sur: 		cerr << "Sur"; 		break;
	case suroeste: 	cerr << "Suroeste"; break;
	case oeste: 	cerr << "Oeste"; 	break;
	case noroeste: 	cerr << "Noroeste"; break;
	}
}

void ComportamientoJugador::accion_string (const Action &a) const
{
	//switch (a)
	//{
	//case actWALK:			cout << "WALK " << endl; 		break;
	//case actTURN_L: 		cout << "TURN_L " << endl; 		break;
	//case actTURN_SR: 		cout << "TURN_SR " << endl; 	break;
	//case actRUN: 			cout << "RUN " << endl; 		break;
	//case act_CLB_WALK: 		cout << "CLB_WALK " << endl; 	break;
	//case act_CLB_TURN_SR: 	cout << "CLB_TURN_SR " << endl; break;
	//case act_CLB_STOP: 		cout << "CLB_STOP " << endl; 	break;
	//case actIDLE: 			cout << "IDLE " << endl; 		break;
	//default: 				cout << "-_ " << endl; 			break;
	//}

	switch (a)
	{
	case actWALK:			cout << "WALK, "; 		break;
	case actTURN_L: 		cout << "TURN_L, "; 	break;
	case actTURN_SR: 		cout << "TURN_SR, "; 	break;
	case actRUN: 			cout << "RUN, "; 		break;
	case act_CLB_WALK: 		cout << "CLB_WALK, "; 	break;
	case act_CLB_TURN_SR: 	cout << "CLB_TURN_SR, "; break;
	case act_CLB_STOP: 		cout << "CLB_STOP, "; 	break;
	case actIDLE: 			cout << "IDLE, "; 		break;
	default: 				cout << "-_ "; 			break;
	}
}

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

		plan = anchura_solo_jugador_V2(c_state, goal, mapaResultado);
		VisualizaPlan(c_state, plan);
		hayPlan = true;
		cerr << "----" << endl;
	}
	else if (plan.size())
	{
		cerr << "Ejecutando plan" << endl;
		accion = plan.front();
		plan.pop_front();
	}
	else // if (plan.size() == 0)
	{
		cerr << "Plan finalizado" << endl;
		hayPlan = false;
	}

	return accion;
}

//---------------------- Implementación de la busqueda en anchura ---------------------------
bool ComportamientoJugador::casilla_transitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa, const ubicacion &ub_curr, const ubicacion &ub_colab, bool examinar_colab) const
{
	bool salida = (mapa[x.f][x.c] != 'P' and mapa[x.f][x.c] != 'M');

	if (examinar_colab)
		salida = salida and ub_curr.f != ub_colab.f and ub_curr.c != ub_colab.c;

	return salida;
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

bool ComportamientoJugador::Find(const estado &item, const list<nodo> &lista) const
{
	auto it = lista.begin();
	while (it != lista.end() and !(it->st == item))
		it++;

	return (!(it == lista.end()));

}

estado ComportamientoJugador::apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const
{
	estado st_result = st;
	ubicacion sig_ubicacion, sig_ubicacion2;
	sig_ubicacion = next_casilla(st.jugador);
	sig_ubicacion2 = next_casilla(sig_ubicacion);

	switch (a)
	{
	case actWALK:
		if (casilla_transitable(sig_ubicacion, mapa, st.jugador, st.colaborador, true))
			st_result.jugador = sig_ubicacion;
		break;
	
	case actRUN:
		if (casilla_transitable(sig_ubicacion, mapa, st.jugador, st.colaborador, true) && casilla_transitable(sig_ubicacion2, mapa, st.jugador, st.colaborador, true))
			st_result.jugador = sig_ubicacion2;
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

list<Action> ComportamientoJugador::anchura_solo_jugador_V2 (const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa)
{
	list<nodo> frontier;
	set<nodo> explored;
	list<Action> plan, current_plan;
	nodo current_node, child;
	current_node.st = inicio;
	frontier.push_back(current_node);
	bool solution_found = es_solucion(current_node.st.jugador, final);

	while (!frontier.empty() and !solution_found)
	{
		current_plan = current_node.secuencia;
		frontier.pop_front();
		explored.insert(current_node);

		for (size_t action = 0; action < 4 && !solution_found; action++)
		{
			//Actualizo los valores del nodo hijo
			child.secuencia = current_plan;
			child.secuencia.push_back((Action) action);
			child.st = apply((Action) action, current_node.st, mapa);

			//cerr << "------ NODO HIJO ------" << endl;
			//mostrar_nodo(child);
			//cerr << endl;

			// Si el hijo es solución, guardo el plan y lo muestro
			if ((solution_found = es_solucion(child.st.jugador, final)))
			{
				plan = child.secuencia;
				//cout << "PLAN ENCONTRADO" << endl;
				//PintaPlan(plan);
				//cout << endl;
			}

			// Si no, lo añado a la frontera (si no ha sido explorado ya)
			else if (explored.find(child) == explored.end())
				frontier.push_back(child);

			//cerr << "FRONTIER ACTUAL: ";
			//mostrar_lista(frontier);
			//cerr << endl;
			//cerr << "-----------------------" << endl << endl;
		}

		// Seleccionamos le siguiente nodo a explorar
		if (!solution_found and !frontier.empty())
		{
			current_node = frontier.front();
			while (!frontier.empty() and explored.find(current_node) != explored.end())
			{
				frontier.pop_front();
				if (!frontier.empty())
					current_node = frontier.front();
			}
		}
	}

	return plan;
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