#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
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

bool ComportamientoJugador::es_solucion (const estado &st, const ubicacion &final, unsigned char nivel) const
{
	ubicacion ub = nivel == 1 or nivel == 3 ? st.colaborador : st.jugador;
	return (ub.f == final.f and ub.c == final.c);
}

void ComportamientoJugador::mostrar_lista(const queue<nodo> &q, bool completa) const
{
	size_t num_paso = 1;
	queue<nodo> copy = q;
	while (!copy.empty())
	{
		if (completa)
		{
			cerr << "------ PASO " << num_paso << " ------" << endl;
			mostrar_nodo(copy.front(), false);
			cerr << endl << endl;
		}
		else
		{
			cerr << "{";
			PintaPlan((copy.front()).secuencia);
			cerr << "}, ";
		}
		copy.pop();
		num_paso++;
	}
}

void ComportamientoJugador::mostrar_lista(const priority_queue<nodo, vector<nodo>, greater<nodo>> &q, bool completa) const
{
	size_t num_paso = 1;
	priority_queue<nodo, vector<nodo>, greater<nodo>> copy = q;
	while (!copy.empty())
	{
		if (completa)
		{
			cerr << "------ PASO " << num_paso << " ------" << endl;
			mostrar_nodo(copy.top(), false);
			cerr << endl << endl;
		}
		else
		{
			cerr << "{";
			PintaPlan((copy.top()).secuencia);
			cerr << "}, ";
		}
		copy.pop();
		num_paso++;
	}
}

void ComportamientoJugador::mostrar_lista(const set<nodo> &q, bool completa) const
{
	auto it = q.begin();
	size_t num_paso = 1;
	while (it != q.end())
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
			PintaPlan((*it).secuencia);
			cerr << "}, ";
		}
		it++;
		num_paso++;
	}
	cerr << endl;
}

void ComportamientoJugador::mostrar_ubicacion(const ubicacion &ub) const
{
	cerr << "Pos: [" << ub.f << "][" << ub.c << "] = " << mapaResultado[ub.f][ub.c] << ", Orientación: ";
	orientacion_string(ub.brujula);
	cerr << endl;
}

void ComportamientoJugador::mostrar_estado(const estado &st, const unsigned char nivel) const
{
	cerr << "Jugador " << "\t";
	mostrar_ubicacion(st.jugador);

	if (nivel != 0 && nivel != 2)
	{
		cerr << "Colaborador " << "\t";
		mostrar_ubicacion(st.colaborador);

		cerr << "Ultima orden colaborador: ";
		accion_string(st.ultima_orden_colaborador);
	}
}

void ComportamientoJugador::mostrar_nodo(const nodo &nd, const unsigned char nivel, bool mostrar_secuencia) const
{
	mostrar_estado(nd.st, nivel);
	if (nivel > 1)
	{
		cerr << "Coste acumulado: " << nd.coste_acumulado << endl;
		cerr << "Bikini:" << boolalpha << nd.bikini << ", Zapatillas:" <<  boolalpha << nd.zapatillas << endl;
	}
	if (mostrar_secuencia)
	{
		cerr << endl << "Secuencia: ";
		PintaPlan(nd.secuencia);
	}
	else
	{
		cerr << endl << "Accion: ";
		if(!nd.secuencia.empty()) accion_string(nd.secuencia.back());
		cerr << endl;
	}
	cerr << endl;
}

//unsigned short ComportamientoJugador::heuristica(const estado &st, const ubicacion &final) const
//{
//	return abs(st.jugador.f - final.f) + abs(st.jugador.c - final.c);
//}

unsigned short coste_total_bateria(const queue<nodo> &plan)
{
	unsigned short coste = 0;
	queue<nodo> copy = plan;
	while (!copy.empty())
	{
		coste += copy.front().coste_acumulado;
		copy.pop();
	}

	return coste;
}

void ComportamientoJugador::PintaPlan(const queue<Action> &plan) const
{
	queue<Action> copy = plan;
	while (!copy.empty())
	{
		accion_string(copy.front());
		copy.pop();
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
		registrar_estado(sensores, c_state, goal);

		if (sensores.nivel == 0)
			plan = nivel_0_1(c_state, goal, mapaResultado, 0);
		else if (sensores.nivel == 1)
			plan = nivel_0_1(c_state, goal, mapaResultado, 1);
		else if (sensores.nivel == 2)
			plan = nivel_2(c_state, goal, mapaResultado);
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
		else
		{
			VisualizaPlan(c_state, plan);
			hayPlan = true;

			cout << "--------PLAN ENCONTRADO--------" << endl;
			//PintaPlan(plan); cerr << endl;
			//mostrar_lista(generar_nodos_secuencia(plan), true);
			generar_nodos_secuencia(plan, c_state);
			cerr << "BATERIA FINAL ESPERADA: " << sensores.bateria - consumo_total_bateria << endl;
			cerr << "COSTE BATERIA: " << consumo_total_bateria << endl;
			cerr << "PASOS TOTALES: " << plan.size() << endl;
			cerr << "INSTANTES ESPERADOS: " << 3000 - plan.size() << endl;
			cerr << "HIJOS TOTALES EXPLORADOS: " << hijos_explorados << endl;
			cerr << "NODOS ABIERTOS: " << nodos_abiertos << endl;
			cerr << "NODOS CERRADOS: " << nodos_cerrados << endl;
			cerr << "---------------------" << endl;

			accion = plan.front();
			plan.pop();
		}

	}
	else if (plan.size())
	{
		accion = plan.front();
		plan.pop();
	}
	else
		hayPlan = false;

	return accion;
}

//-------------------------------------------------
void ComportamientoJugador::generar_hijo(const nodo &padre, const Action a, nodo &hijo, const vector<vector<unsigned char>> &mapa, const unsigned char nivel) const
{
	hijo.secuencia = padre.secuencia;
	hijo.st = nivel == 1 or nivel == 3 ? apply_seguro((Action) a, padre.st, mapa) : apply((Action) a, padre.st, mapa);
	hijo.secuencia.push(a);

	if (nivel > 1)
	{
		hijo.bikini = padre.bikini || mapa[hijo.st.jugador.f][hijo.st.jugador.c] == 'K';
		hijo.zapatillas = padre.zapatillas || mapa[hijo.st.jugador.f][hijo.st.jugador.c] == 'D';
		hijo.coste_acumulado = padre.coste_acumulado + coste_casilla(a, padre, mapa);
	}
}

unsigned short ComportamientoJugador::coste_casilla(const Action a, const nodo &n, const vector<vector<unsigned char>> &mapa) const
{
	ubicacion ub = n.st.jugador;
	if (a == actIDLE or a == act_CLB_STOP)
		return 0;
	
	switch (mapa[ub.f][ub.c])
	{
	// Agua
	case 'A':
		switch (a)
		{
		case actWALK: return n.bikini ? 10 : 100;
		case act_CLB_WALK: return n.bikini ? 10 : 100;
		case actRUN: return n.bikini ? 15 : 150;
		case actTURN_L: return n.bikini ? 5 : 30;
		case actTURN_SR: return n.bikini ? 2 : 10;
		case act_CLB_TURN_SR: return n.bikini ? 2 : 10;
		}
	// Bosque
	case 'B':
		switch (a)
		{
		case actWALK: return n.zapatillas ? 15 : 50;
		case act_CLB_WALK: return n.zapatillas ? 15 : 50;
		case actRUN: return n.zapatillas ? 25 : 75;
		case actTURN_L: return n.zapatillas ? 1 : 7;
		case actTURN_SR: return n.zapatillas ? 1 : 5;
		case act_CLB_TURN_SR: return n.zapatillas ? 1 : 5;
		}
	// Arena
	case 'T':
		switch (a)
		{
		case actWALK: return 2;
		case act_CLB_WALK: return 2;
		case actRUN: return 3;
		case actTURN_L: return 2;
		case actTURN_SR: return 1;
		case act_CLB_TURN_SR: return 1;
		}

	// Piedra
	case 'S': return 1;
	// Casilla de ubicación
	case 'G': return 1;
	// Bikini
	case 'K': return 1;
	// Zapatillas
	case 'D': return 1;
	// Recarga
	case 'X': return 1;
	// Muro
	//case 'M': return 0;
	// Precipicio
	//case 'P': return 0;
	// Desconocida
 	//case '?': return 1;
	// Jugador
	//case 'a': return 0;
	default:
		cerr << "ERROR EN ASIGNACION DE COSTE DE CASILLA" << endl;
		cerr << "Casilla introducida: " << mapa[ub.f][ub.c] << endl;
		exit(1);
	}
}

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

ubicacion ComportamientoJugador::next_casilla(const ubicacion &pos) const
{
	unsigned char offsetcol = 0;
	if (pos.brujula == noreste || pos.brujula == sureste || pos.brujula == suroeste || pos.brujula == noroeste)
		offsetcol = 1;
	return traductor_posicion(pos, -1, offsetcol);
}

bool ComportamientoJugador::Find(const estado &item, const queue<estado> &cola) const
{
	queue<estado> copy = cola;
	while (!copy.empty())
	{
		if (copy.front() == item)
			return true;
		copy.pop();
	}

	return false;
}

bool ComportamientoJugador::Find(const estado &item, const queue<nodo> &cola) const
{
	queue<nodo> copy = cola;
	while (!copy.empty())
	{
		if (copy.front().st == item)
			return true;
		copy.pop();
	}

	return false;
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
		st_tras_ultima_accion = apply(a, st, mapa);
		if (st_tras_ultima_accion != st or st.ultima_orden_colaborador == actIDLE)
		{
			st_result = apply(st_result.ultima_orden_colaborador, st_tras_ultima_accion, mapa);
			if (st_result == st_tras_ultima_accion and st.ultima_orden_colaborador != act_CLB_STOP)
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

queue<Action> ComportamientoJugador::nivel_2(const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa)
{
	vector<vector<vector<unsigned short>>> matriz_costes(mapa.size(), vector<vector<unsigned short>>(mapa.size(), vector<unsigned short>(9, numeric_limits<unsigned short>::max())));
	unsigned char accion_inicio = (unsigned char) actIDLE, accion_fin = (unsigned char) actWHEREIS;
	priority_queue<nodo, vector<nodo>, greater<nodo>> frontier;
	queue<Action> plan;
	nodo current_node, child;
	current_node.st = inicio;
	frontier.push(current_node);
	bool solution_found = es_solucion(current_node.st, final, 2);

	while (!frontier.empty() and !solution_found)
	{
		current_node = frontier.top();
		frontier.pop();
		cerr << "------ CURRENT NODE ------" << endl;
		mostrar_nodo(current_node, 2);

 		if ((solution_found = es_solucion(current_node.st, final, 2))) // Si es solución, guardo el plan
		{
			cerr << "SOLUCION ENCONTRADA" << endl << endl;
			plan = current_node.secuencia;
			nodos_abiertos = frontier.size();
		}

		for (unsigned char action = accion_inicio; action != accion_fin && !solution_found; action = (action + 1) % 9)
		{
			generar_hijo(current_node, (Action) action, child, mapa, 2);
			//cerr << "------ NODO HIJO ------" << endl;
			//mostrar_nodo(child, 2);
			if (matriz_costes[child.st.jugador.f][child.st.jugador.c][action] > child.coste_acumulado)
			{
				matriz_costes[child.st.jugador.f][child.st.jugador.c][action] = child.coste_acumulado;
				frontier.push(child);
			}
		}

		//cerr << "------ FRONTERA ------" << endl;
		//mostrar_lista(frontier, false);
		//cerr << endl << endl;;
		//cerr << "*********************************************************************" << endl << endl;

		hijos_explorados++;
	}
	return plan;
}

queue<Action> ComportamientoJugador::nivel_0_1(const estado &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa, const unsigned char nivel)
{
	unsigned char ciclos;
	Action accion_inicio = actIDLE, accion_fin = nivel == 0 ? actWHEREIS : actIDLE;
	queue<nodo> frontier;
	set<nodo> explored;
	queue<Action> plan;
	nodo current_node, child;
	current_node.st = inicio;
	frontier.push(current_node);
	bool solution_found = es_solucion(current_node.st, final, nivel);

	while (!frontier.empty() and !solution_found)
	{
		frontier.pop();
		explored.insert(current_node);
		ciclos = nivel == 0 ? 1 : 0;
		hijos_explorados++;

		for (Action action = accion_inicio; !solution_found;  action = (Action) (((unsigned char) action + 1) % 9))
		{
			if (action == accion_fin and ciclos > 0) break;
			else if (action == actWHEREIS) continue;
			else ciclos++;

			//Actualizo los valores del nodo hijo
			generar_hijo(current_node, action, child, mapa, nivel);
			
			if ((solution_found = es_solucion(child.st, final, nivel))) // Si es solución, guardo el plan
			{
				plan = child.secuencia;
				nodos_abiertos = frontier.size();
				nodos_cerrados = explored.size();
			}
			else if (explored.find(child) == explored.end()) // Sino lo añado a la frontera
				frontier.push(child);

			//cerr << "------ NODO HIJO ------" << endl;
			//mostrar_nodo(child, nivel);
		}

		if (!solution_found and !frontier.empty())
		{
			current_node = frontier.front();
			while (!frontier.empty() and explored.find(current_node) != explored.end())
			{
				frontier.pop();
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

queue<nodo> ComportamientoJugador::generar_nodos_secuencia(const queue<Action> &plan, const estado &inicio)
{
	queue<nodo> nodos;
	queue<Action> copy = plan;
	nodo padre, hijo;
	padre.st = inicio;

	while (!copy.empty())
	{
		generar_hijo(padre, copy.front(), hijo, mapaResultado, 0);
		nodos.push(hijo);
		consumo_total_bateria += coste_casilla(copy.front(), padre, mapaResultado);
		copy.pop();
	}

	return nodos;
}

void ComportamientoJugador::VisualizaPlan(const estado &st, const queue<Action> &plan)
{
	anula_matriz(mapaConPlan);
	estado cst = st;
	queue<Action> copy = plan;

	while (!copy.empty())
	{
		if ((copy.front() != act_CLB_WALK) and (copy.front() != act_CLB_TURN_SR) and (copy.front() != act_CLB_STOP))
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

		switch (copy.front())
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
		copy.pop();
	}
	
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}