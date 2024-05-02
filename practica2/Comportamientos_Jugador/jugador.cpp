#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>

ubicacion ComportamientoJugador::traductor_posicion(const ubicacion &pos, const int offset_fil, const int offset_col) const
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

	return {pos.f + posfil_tmp, pos.c + poscol_tmp, pos.brujula};
}

list<Action> ComportamientoJugador::traductor_plan_colaborador(const list<Action> &plan) const
{
	list<Action> plan_colab;
	for(const auto &a : plan)
	{
		switch (a)
		{
		case actRUN:
			plan_colab.push_back(act_CLB_WALK);	plan_colab.push_back(act_CLB_WALK);
			break;

		case actWALK:
			plan_colab.push_back(act_CLB_WALK);
			break;

		case actTURN_SR:
			plan_colab.push_back(act_CLB_TURN_SR);
			break;

		case actTURN_L:
			plan_colab.push_back(act_CLB_TURN_SR);	plan_colab.push_back(act_CLB_TURN_SR);	plan_colab.push_back(act_CLB_TURN_SR);
			plan_colab.push_back(act_CLB_TURN_SR);	plan_colab.push_back(act_CLB_TURN_SR);	plan_colab.push_back(act_CLB_TURN_SR);	
			break;

		case actIDLE:
			plan_colab.push_back(act_CLB_STOP);
			break;

		default:
			cerr << "ERROR EN EL TRADUCTOR DE ACCIONES DEL COLABORADOR" << endl;
			cerr << "ACCION INTRODUCIDA: ";
			accion_string(a);
			cerr << endl;
			exit(1);
		}
	}

	return plan_colab;
}

bool ComportamientoJugador::es_solucion (const estado &st, const ubicacion &final, bool colaborador) const
{
	ubicacion ub = colaborador ? st.colaborador : st.jugador;
	return (ub.f == final.f and ub.c == final.c);
}

void ComportamientoJugador::mostrar_lista(const list<nodo> &l, bool completa) const
{
	size_t num_paso = 1;
	auto it = l.begin();
	while (it != l.end())
	{
		if (completa)
		{
			cerr << "------ PASO " << num_paso << " ------" << endl;
			mostrar_nodo(*it, false);
			cerr << endl << endl;
		}
		else
		{
			cerr << "{";
			PintaPlan(it->secuencia);
			cerr << "}, ";
		}
		it++;
		num_paso++;
	}
}

void ComportamientoJugador::mostrar_ubicacion(const ubicacion &ub) const
{
	cerr << "Pos: [" << ub.f << "][" << ub.c << "] = " << mapaResultado[ub.f][ub.c] << ", Orientación: ";
	orientacion_string(ub.brujula);
	cerr << endl;
}

void ComportamientoJugador::mostrar_estado(const estado &st) const
{
	cerr << "Jugador " << "\t";
	mostrar_ubicacion(st.jugador);
	cerr << "Colaborador " << "\t";
	mostrar_ubicacion(st.colaborador);

	cerr << "Ultima orden colaborador: ";
	accion_string(st.ultima_orden_colaborador);
	cerr << endl;
}

void ComportamientoJugador::mostrar_nodo(const nodo &nd, bool mostrar_secuencia) const
{
	mostrar_estado(nd.st);
	if (mostrar_secuencia)
	{
		cerr << endl << "Secuencia: ";
		PintaPlan(nd.secuencia);
	}
	else
	{
		cerr << "Accion: ";
		accion_string(nd.secuencia.back());
		cerr << endl;
	}
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
	default: 		cerr << "ERROR EN EL TRADUCTOR DE ORIENTACIONES";
					cerr << "Orientación introducida: " << (int) o << endl;
					exit(1);
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
	default: 				cerr << "ERROR EN EL TRADUCTOR DE ACCIONES" << endl;
							cerr << "Acción introducida: " << (int) a << endl;
							exit(1);
	}
}

void ComportamientoJugador::registrar_estado(const Sensores &sensores, estado &c_state, ubicacion &goal)
{
	c_state.jugador.f = sensores.posF;
	c_state.jugador.c = sensores.posC;
	c_state.jugador.brujula = sensores.sentido;
	c_state.colaborador.f = sensores.CLBposF;
	c_state.colaborador.c = sensores.CLBposC;
	c_state.colaborador.brujula = sensores.CLBsentido;
	goal.f = sensores.destinoF;
	goal.c = sensores.destinoC;
}

Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;

	if (!hayPlan)
	{
		cerr << "Calculamos un nuevo plan" << endl;
		registrar_estado(sensores, c_state, goal);

		if (sensores.nivel == 0)
			plan = buscar_objetivo_anchura(c_state, goal, mapaResultado, false);
		else if (sensores.nivel == 1)
			plan = nivel_1(c_state, goal, mapaResultado);
		else
		{
			cerr << "ERROR: Nivel no implementado" << endl;
			exit(1);
		}
		
		if (plan.empty())
		{
			cerr << "ERROR: No se ha encontrado un plan" << endl;
			exit(1);
		}
		
		VisualizaPlan(c_state, plan);
		hayPlan = true;
		cerr << "---------------------" << endl;
	}
	else if (plan.size())
	{
		accion = plan.front();
		plan.pop_front();
	}
	else
	{
		cerr << "Plan finalizado" << endl;
		hayPlan = false;
	}

	return accion;
}

//---------------------- Implementación de la busqueda en anchura ---------------------------
bool ComportamientoJugador::esta_en_rango_vision(const estado &st) const
{
	//cerr << "Estoy en poner terreno en matriz" << endl;
	const vector<pair<short, short>> pos_norte = { {0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, {-3, -3}, {-3, -2}, {-3, -1}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3} };
	const vector<pair<short, short>> pos_noreste = { {0, 0}, {-1, 0}, {-1, 1}, {0, 1}, {-2, 0}, {-2, 1}, {-2, 2}, {-1, 2}, {0, 2}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3}, {-2, 3}, {-1, 3}, {0, 3} };
	vector<pair<short, short>> vector_pos = pos_noreste;

	switch (st.jugador.brujula)
	{
	case norte: 	vector_pos = pos_norte; 	break;
	case noreste: 	vector_pos = pos_noreste; 	break;
	case oeste: 	vector_pos = pos_norte;		break;
	case noroeste: 	vector_pos = pos_noreste; 	break;
	case este: 		vector_pos = pos_norte;		break;
	case sureste: 	vector_pos = pos_noreste;	break;
	case sur: 		vector_pos = pos_norte;		break;
	case suroeste: 	vector_pos = pos_noreste;	break;
	}

	for (size_t i = 0; i < 16; i++)
	{
		ubicacion ub = traductor_posicion(st.jugador, vector_pos[i].first, vector_pos[i].second);
		if (ub.f == st.colaborador.f and ub.c == st.colaborador.c)
			return true;
	}

	return false;
}

bool ComportamientoJugador::casilla_transitable(const estado &cst, const vector<vector<unsigned char>> &mapa, bool colaborador) const
{
	ubicacion pos = colaborador ? cst.colaborador : cst.jugador;

	return mapa[pos.f][pos.c] != 'P' and mapa[pos.f][pos.c] != 'M' and !(cst.jugador.f == cst.colaborador.f and cst.jugador.c == cst.colaborador.c);
}

bool ComportamientoJugador::casilla_transitable(const estado &cst, const vector<vector<unsigned char>> &mapa, const list<estado> &plan) const
{
	return casilla_transitable(cst, mapa) and !Find(cst, plan);
}

ubicacion ComportamientoJugador::next_casilla(const ubicacion &pos) const
{
	unsigned char offsetcol = 0;
	if (pos.brujula == noreste || pos.brujula == sureste || pos.brujula == suroeste || pos.brujula == noroeste)
		offsetcol = 1;
	return traductor_posicion(pos, -1, offsetcol);
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

estado ComportamientoJugador::apply_seguro(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const
{
	estado st_tras_ultima_accion, st_result = st;
	bool veo_al_colaborador = esta_en_rango_vision(st);
	if (a == act_CLB_STOP or a == act_CLB_TURN_SR or a == act_CLB_WALK)
	{
		if(veo_al_colaborador)
			st_result = apply(a, st, mapa);
	}
	else 
	{
		st_tras_ultima_accion = apply(st.ultima_orden_colaborador, st, mapa);
		// Si hubo exito al aplicar la última acción del colaborador, aplico la acción actual
		if (st_tras_ultima_accion != st or st.ultima_orden_colaborador == act_CLB_STOP)
		{
			st_result = apply(a, st_tras_ultima_accion, mapa);
			// Si hay error al aplicar la acción actual, anulo el proceso volviendo al estado inicial
			if (st_result == st_tras_ultima_accion and a != actIDLE)
				st_result = st;
		}
	}

	return st_result;
}

estado ComportamientoJugador::apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const
{
	estado st_result, st_sig, st_sig2;
	st_result = st_sig = st_sig2 = st;
	//cerr << "Aplicando accion: ";
	//accion_string(a);
	//cerr << endl;

	if (a == act_CLB_WALK)
		st_sig.colaborador = next_casilla(st.colaborador);
	else
	{
		st_sig.jugador = next_casilla(st.jugador);
		st_sig2.jugador = next_casilla(st_sig.jugador);
	}

	switch (a)
	{
	case actWALK:
		if (casilla_transitable(st_sig, mapa))
			st_result = st_sig;
		break;
	
	case act_CLB_WALK:
		if (casilla_transitable(st_sig, mapa, true))
		{
			st_result = st_sig;
			st_result.ultima_orden_colaborador = act_CLB_WALK;
		}
		break;

	case actRUN:
		if (casilla_transitable(st_sig, mapa) && casilla_transitable(st_sig2, mapa))
			st_result = st_sig2;
		break;

	case actTURN_L:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+6)%8);
		break;

	case actTURN_SR:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+1)%8);
		break;

	case act_CLB_TURN_SR:
		st_result.colaborador.brujula = static_cast<Orientacion>((st_result.colaborador.brujula+1)%8);
		st_result.ultima_orden_colaborador = act_CLB_TURN_SR;
		break;

	case act_CLB_STOP:
		st_result.ultima_orden_colaborador = act_CLB_STOP;
		break;

	case actIDLE: break;

	default:
		cerr << "ERROR EN LA APLICACIÓN DE LA ACCIÓN" << endl;
		cerr << "Acción introducida: ";
		accion_string(a);
		cerr << endl;
		exit(1);
	}

	return st_result;
}

list<Action> ComportamientoJugador::nivel_1(const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa)
{
	size_t accion_inicio, accion_fin;
	accion_inicio = (size_t) actWALK;
	accion_fin = (size_t) actIDLE;
	list<nodo> frontier;
	set<nodo> explored;
	list<Action> plan, current_plan;
	nodo current_node, child;
	current_node.st = inicio;
	frontier.push_back(current_node);
	bool solution_found = es_solucion(current_node.st, final);


	while (!frontier.empty() and !solution_found)
	{
		current_plan = current_node.secuencia;
		frontier.pop_front();
		explored.insert(current_node);

		for (size_t action = accion_inicio; action <= accion_fin && !solution_found; action++)
		{
			if (action == (size_t) actWHEREIS) continue;

			//Actualizo los valores del nodo hijo
			child.secuencia = current_plan;
			child.st = apply_seguro((Action) action, current_node.st, mapa);
			child.secuencia.push_back((Action) action);

			// Realmente esto se hace implicitamente al hacer la búsqueda en explorados
			if (child.st == current_node.st)
				continue;
			

		//if (
		//	(child.st.jugador.f == 13 and child.st.jugador.c == 19 and
		//	child.st.colaborador.f == 14 and child.st.colaborador.c == 19 and 
		//	child.st.jugador.brujula == sur and child.st.colaborador.brujula == sureste and
		//	child.st.ultima_orden_colaborador == act_CLB_TURN_SR)
		//	//and
		//	//(current_node.st.jugador.f == 13 and current_node.st.jugador.c == 19 and 
		//	//current_node.st.colaborador.f == 14 and current_node.st.colaborador.c == 19
		//	//and current_node.st.jugador.brujula == sur and current_node.st.colaborador.brujula == sureste)
		//)
		//	{
		//		cerr << "------ NODO ACTUAL ------" << endl;
		//		mostrar_nodo(current_node, false);
		//		cerr << endl;
//
		//		cerr << "------ NODO HIJO ------" << endl;
		//		mostrar_nodo(child, false);
		//		cerr << endl;
		//	}

		// EL PROBLEMA ES QUE SI TOMA UNA DECISIÓN QUE LLEVA UN CHOQUE, EL APPLY DEVUELVE COMO SI NO PASARA NADA Y SE APLICASE LA ACCIÓN DEL COLABORADOR ANTERIOR
			// Si el hijo es solución, guardo el plan y lo muestro
			if ((solution_found = es_solucion(child.st, final)))
			{
				plan = child.secuencia;
				cout << "PLAN ENCONTRADO" << endl;
				//PintaPlan(plan);
				mostrar_lista(generar_nodos_secuencia(child.secuencia), true);
				cout << endl;
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

list<Action> ComportamientoJugador::buscar_objetivo_anchura(const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa, bool colaborador)
{// if (plan.size() == 0)
	size_t accion_inicio, accion_fin;
	accion_inicio = colaborador ? (size_t) act_CLB_WALK : (size_t) actWALK;
	accion_fin = colaborador ? (size_t) act_CLB_TURN_SR : (size_t) actTURN_SR;
	list<nodo> frontier;
	set<nodo> explored;
	list<Action> plan, current_plan;
	nodo current_node, child;
	current_node.st = inicio;
	frontier.push_back(current_node);
	bool solution_found = es_solucion(current_node.st, final, colaborador);

	while (!frontier.empty() and !solution_found)
	{
		current_plan = current_node.secuencia;
		frontier.pop_front();
		explored.insert(current_node);

		for (size_t action = accion_inicio; action <= accion_fin && !solution_found; action++)
		{
			//Actualizo los valores del nodo hijo
			child.secuencia = current_plan;
			child.secuencia.push_back((Action) action);
			child.st = apply((Action) action, current_node.st, mapa);

			//cerr << "------ NODO HIJO ------" << endl;
			//mostrar_nodo(child);
			//cerr << endl;

			// Si el hijo es solución, guardo el plan y lo muestro
			if ((solution_found = es_solucion(child.st, final, colaborador)))
			{
				plan = child.secuencia;
				cout << "PLAN ENCONTRADO" << endl;
				PintaPlan(plan);
				cout << endl;
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

list<nodo> ComportamientoJugador::generar_nodos_secuencia(const list<Action> &plan) const
{
	list<nodo> nodos;
	nodo nd;
	estado st = c_state;

	auto it = plan.begin();
	while (it != plan.end())
	{
		nd.st = apply_seguro(*it, st, mapaResultado);
		nd.secuencia.push_back(*it);
		nodos.push_back(nd);
		st = nd.st;
		it++;
	}

	return nodos;
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

		case act_CLB_WALK:
			cst.colaborador = next_casilla(cst.colaborador);
			cst.ultima_orden_colaborador = act_CLB_WALK;
			mapaConPlan[cst.colaborador.f][cst.colaborador.c] = 2;
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