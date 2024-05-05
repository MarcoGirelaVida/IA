#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

// ----------------- FUNCIONES DE LA DEBUGGING -----------------

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
			cerr << "{"; PintaPlan((copy.front()).secuencia); cerr << "}, ";
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
			cerr << "{"; PintaPlan((copy.top()).secuencia); cerr << "}, ";
		}
		copy.pop();
		num_paso++;
	}
}
void ComportamientoJugador::mostrar_ubicacion(const ubicacion &ub) const
{
	cerr << "Pos: [" << ub.f << "][" << ub.c << "] = " << mapaResultado[ub.f][ub.c] << ", Orientación: "; orientacion_string(ub.brujula); cerr << endl;
}
void ComportamientoJugador::mostrar_estado(const estado &st, const unsigned char nivel) const
{
	cerr << "Jugador " << "\t"; mostrar_ubicacion(st.jugador);
	if (nivel != 0 && nivel != 2)
	{
		cerr << "Colaborador " << "\t"; mostrar_ubicacion(st.colaborador);
		cerr << "Ultima orden colaborador: "; accion_string(st.ultima_orden_colaborador); cerr << endl;
		cerr << "Bikini:" << boolalpha << st.bikini << ", Zapatillas:" <<  boolalpha << st.zapatillas << endl;
		cerr << "Bikini colaborador:" << boolalpha << st.bikini_colab << ", Zapatillas colaborador:" <<  boolalpha << st.zapatillas_colab << endl;
	}
}
void ComportamientoJugador::mostrar_nodo(const nodo &nd, const unsigned char nivel, bool mostrar_secuencia) const
{
	mostrar_estado(nd.st, nivel);
	if (nivel > 1) 			{ cerr << "Coste acumulado: " << 3000 - nd.coste << endl; }
	if (mostrar_secuencia) 	{ cerr << endl << "Secuencia: "; PintaPlan(nd.secuencia); }
	else 					{ cerr << endl << "Accion: "; if(!nd.secuencia.empty()) accion_string(nd.secuencia.back()); cerr << endl; }
	cerr << endl;
}

void ComportamientoJugador::anula_matriz(vector<vector<unsigned char>> & matriz)
{
	for (int i = 0; i < matriz.size(); i++)
		for (int j = 0; j < matriz[0].size(); j++)
			matriz[i][j] = 0;
}
void ComportamientoJugador::PintaPlan(const queue<Action> &plan) const
{
	queue<Action> copy = plan;
	while (!copy.empty())
	{
		accion_string(copy.front());
		copy.pop();
	}
	cout << "  (" << plan.size() << ")";
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

queue<nodo> ComportamientoJugador::generar_nodos_secuencia(const queue<Action> &plan, const estado &inicio)
{
	queue<nodo> nodos;
	queue<Action> copy = plan;
	nodo padre, hijo;
	padre.st = inicio;
	while (!copy.empty())
	{
		generar_hijo(copy.front(), padre, hijo, 3);
		nodos.push(hijo);
		consumo_total_bateria += coste_accion_total(copy.front(), padre.st);
		padre = hijo;
		copy.pop();
	}
	return nodos;
}

// ----------------- FUNCIONES MAIN -----------------

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
			plan = nivel_0_1(c_state, goal, 0);
		else if (sensores.nivel == 1)
			plan = nivel_0_1(c_state, goal, 1);
		else if (sensores.nivel == 2)
			plan = nivel_2_3(c_state, goal, 2);
		else if (sensores.nivel == 3)
			plan = nivel_2_3(c_state, goal, 3);
		//else
		//	plan = nivel_4(c_state, goal, , 3);

		if (plan.empty())
		{
			cerr << "ERROR: No se ha encontrado un plan" << endl;
			exit(1);
		}
		else
		{
			//cout << "--------PLAN ENCONTRADO--------" << endl;
			////PintaPlan(plan); cerr << endl;
			////mostrar_lista(generar_nodos_secuencia(plan, c_state), true);
			//plan_nodos = generar_nodos_secuencia(plan, c_state);
			//cerr << "BATERIA FINAL ESPERADA: " << sensores.bateria - consumo_total_bateria << endl;
			//cerr << "COSTE BATERIA: " << consumo_total_bateria << endl;
			//cerr << "PASOS TOTALES: " << plan.size() << endl;
			//cerr << "INSTANTES ESPERADOS: " << 3000 - plan.size() << endl;
			//cerr << "HIJOS TOTALES EXPLORADOS: " << hijos_explorados << endl;
			//cerr << "NODOS ABIERTOS: " << nodos_abiertos << endl;
			//cerr << "NODOS CERRADOS: " << nodos_cerrados << endl;
			//cerr << "-----------------------" << endl;
//
			//paso++;
			//cerr << "----- PASO " <<  paso << " -----" << endl;
			//mostrar_nodo(plan_nodos.front(), sensores.nivel, false);
			//cerr << "-----------------------" << endl;
			//plan_nodos.pop();

			hayPlan = true;
			VisualizaPlan(c_state, plan);
			accion = plan.front();
			plan.pop();
		}
	}
	else if (plan.size())
	{
		//paso++;
		//cerr << "COSTE REAL ACTUAL: " << sensores.bateria << endl;
		//cerr << "-----------------------" << endl << endl;
		//cerr << "----- PASO " <<  paso << " -----" << endl;
		//mostrar_nodo(plan_nodos.front(), sensores.nivel, false);
		//cerr << "-----------------------" << endl;
		//plan_nodos.pop();

		accion = plan.front();
		plan.pop();
	}
	else
		hayPlan = false;

	return accion;
}

// ----------------- FUNCIONES DE LA CONSULTA -----------------

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
ubicacion ComportamientoJugador::next_casilla(const ubicacion &pos) const
{
	unsigned char offsetcol = 0;
	if (pos.brujula == noreste || pos.brujula == sureste || pos.brujula == suroeste || pos.brujula == noroeste)
		offsetcol = 1;
	return traductor_posicion(pos, -1, offsetcol);
}

bool ComportamientoJugador::esta_en_rango_vision(const estado &st) const
{
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
bool ComportamientoJugador::casilla_transitable(const estado &cst, bool colaborador) const
{
	ubicacion pos = colaborador ? cst.colaborador : cst.jugador;
	return mapaResultado[pos.f][pos.c] != 'P' and mapaResultado[pos.f][pos.c] != 'M' and !(cst.jugador.f == cst.colaborador.f and cst.jugador.c == cst.colaborador.c);
}
bool ComportamientoJugador::es_solucion (const estado &st, const ubicacion &final, unsigned char nivel) const
{
	ubicacion ub = nivel == 1 or nivel == 3 ? st.colaborador : st.jugador;
	return (ub.f == final.f and ub.c == final.c);
}

unsigned short ComportamientoJugador::coste_accion_total(const Action a, const estado &st) const
{
	if (a == act_CLB_WALK or a == act_CLB_TURN_SR or a == act_CLB_STOP)
		return coste_accion(a, st);
	else
		return coste_accion(a, st) + coste_accion(st.ultima_orden_colaborador, st);
}
unsigned short ComportamientoJugador::coste_accion(const Action a, const estado &st) const
{
	ubicacion ub = a == act_CLB_STOP or a == act_CLB_WALK or a == act_CLB_TURN_SR ? st.colaborador : st.jugador;
	if (a == actIDLE or a == act_CLB_STOP) return 0;
	else if (a == actWHEREIS) return 200;
	
	switch (mapaResultado[ub.f][ub.c])
	{
	case 'A': // Agua
		switch (a)
		{
		case actWALK: return st.bikini ? 10 : 100;
		case act_CLB_WALK: return st.bikini_colab ? 10 : 100;
		case actRUN: return st.bikini ? 15 : 150;
		case actTURN_L: return st.bikini ? 5 : 30;
		case actTURN_SR: return st.bikini ? 2 : 10;
		case act_CLB_TURN_SR: return st.bikini_colab ? 2 : 10;
		}

	case 'B': // Bosque
		switch (a)
		{
		case actWALK: return st.zapatillas ? 15 : 50;
		case act_CLB_WALK: return st.zapatillas_colab ? 15 : 50;
		case actRUN: return st.zapatillas ? 25 : 75;
		case actTURN_L: return st.zapatillas ? 1 : 7;
		case actTURN_SR: return st.zapatillas ? 1 : 5;
		case act_CLB_TURN_SR: return st.zapatillas_colab ? 1 : 5;
		}

	case 'T': // Arena
		switch (a)
		{
		case actWALK: return 2;
		case act_CLB_WALK: return 2;
		case actRUN: return 3;
		case actTURN_L: return 2;
		case actTURN_SR: return 1;
		case act_CLB_TURN_SR: return 1;
		}

	case 'S': return 1; // Piedra
	case 'G': return 1; // Casilla de ubicación
	case 'K': return 1; // Bikini
	case 'D': return 1; // Zapatillas
	case 'X': return 1; // Recarga
	// Muro case 'M': return 0;
	// Precipicio case 'P': return 0;
	// Desconocida case '?': return 1;
	// Jugador case 'a': return 0;
	default:
		cerr << "ERROR EN ASIGNACION DE COSTE DE CASILLA" << endl;
		cerr << "Casilla introducida: " << mapaResultado[ub.f][ub.c] << endl;
		exit(1);
	}
}

// ----------------- FUNCIONES DE LA AUXILIARES -----------------

estado ComportamientoJugador::apply_seguro(const Action &a, const estado &st) const
{
	estado st_tras_ultima_accion, st_result = st;
	if (a == act_CLB_STOP or a == act_CLB_TURN_SR or a == act_CLB_WALK)
	{
		if(esta_en_rango_vision(st))
			st_result = apply(a, st);
	}
	else 
	{
		st_tras_ultima_accion = apply(a, st);
		if (st_tras_ultima_accion != st or a == actIDLE)
		{
			st_result = apply(st_result.ultima_orden_colaborador, st_tras_ultima_accion);
			if (st_result == st_tras_ultima_accion and st.ultima_orden_colaborador != act_CLB_STOP)
				st_result = st;
		}
	}
	return st_result;
}
estado ComportamientoJugador::apply(const Action &a, const estado &st) const
{
	estado st_result, st_sig, st_sig2;
	st_result = st_sig = st_sig2 = st;
	if (a == act_CLB_WALK) 	  st_sig.colaborador = next_casilla(st.colaborador);
	else 					{ st_sig.jugador = next_casilla(st.jugador); st_sig2.jugador = next_casilla(st_sig.jugador); }

	switch (a)
	{
	// ----------------- JUGADOR -----------------
	case actWALK: if (casilla_transitable(st_sig, false)) st_result = st_sig; break;
	case actRUN: if (casilla_transitable(st_sig, false) && casilla_transitable(st_sig2, false)) st_result = st_sig2; break;
	case actTURN_L: st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+6)%8); break;
	case actTURN_SR: st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+1)%8); break;
	case actIDLE: break;

	// ----------------- COLABORADOR -----------------
	case act_CLB_STOP: st_result.ultima_orden_colaborador = act_CLB_STOP; break;
	case act_CLB_WALK: if (casilla_transitable(st_sig, true)) { st_result = st_sig; st_result.ultima_orden_colaborador = act_CLB_WALK; } break;
	case act_CLB_TURN_SR:
		st_result.colaborador.brujula = static_cast<Orientacion>((st_result.colaborador.brujula+1)%8);
		st_result.ultima_orden_colaborador = act_CLB_TURN_SR;
		break;

	default:
		cerr << "ERROR EN LA APLICACIÓN DE LA ACCIÓN" << endl;
		cerr << "Acción introducida: "; accion_string(a); cerr << endl;
		exit(1);
	}
	return st_result;
}

unsigned short ComportamientoJugador::distancia_manhattan(const estado &st, const ubicacion &final) const
{
	//return esta_en_rango_vision(st) ? abs(final.f-st.colaborador.f) + abs(final.f-st.colaborador.c) : abs(st.colaborador.f-st.jugador.f) + abs(st.colaborador.c-st.jugador.c);
	return abs(final.f-st.colaborador.f) + abs(final.f-st.colaborador.c);
}
unsigned short ComportamientoJugador::distancia_chebyshev(const estado &st, const ubicacion &final) const
{
	//return esta_en_rango_vision(st) ? max(abs(final.f - st.colaborador.f), abs(final.c - st.colaborador.c)) : max(abs(st.colaborador.f - st.jugador.f), abs(st.colaborador.c - st.jugador.c));
	return max(abs(st.colaborador.f - final.f), abs(st.colaborador.c - final.c));
}

void ComportamientoJugador::generar_hijo(const Action a, const nodo &padre, nodo &hijo, const unsigned char nivel, const ubicacion &final) const
{
	hijo.st = apply_seguro(a, padre.st);
	hijo.secuencia = padre.secuencia;
	hijo.secuencia.push(a);

	if (nivel > 1)
	{
		hijo.coste_con_heuristica = hijo.coste = padre.coste + coste_accion_total(a, padre.st);
		
		hijo.st.bikini = padre.st.bikini;
		hijo.st.zapatillas = padre.st.zapatillas;
		if (mapaResultado[hijo.st.jugador.f][hijo.st.jugador.c] == 'K')
		{
			hijo.st.bikini = true;
			hijo.st.zapatillas = false;
		}
		else if (mapaResultado[hijo.st.jugador.f][hijo.st.jugador.c] == 'D')
		{
			hijo.st.zapatillas = true;
			hijo.st.bikini = false;
		}

		if (nivel > 2)
		{	
			hijo.coste_con_heuristica += distancia_chebyshev(padre.st, final);
			hijo.st.bikini_colab = padre.st.bikini_colab;
			hijo.st.zapatillas_colab = padre.st.zapatillas_colab;
			if (mapaResultado[hijo.st.colaborador.f][hijo.st.colaborador.c] == 'K')
			{
				hijo.st.bikini_colab = true;
				hijo.st.zapatillas_colab = false;
			}
			else if (mapaResultado[hijo.st.colaborador.f][hijo.st.colaborador.c] == 'D')
			{
				hijo.st.zapatillas_colab = true;
				hijo.st.bikini_colab = false;
			}
		}
	}
}

// ----------------- FUNCIONES DE LA BÚSQUEDA -----------------

queue<Action> ComportamientoJugador::nivel_2_3(const estado &inicio, const ubicacion &final, const unsigned char nivel)
{
	const vector<Action> acciones = nivel == 2 ? acciones_jugador : acciones_jugador_colaborador;
	priority_queue<nodo, vector<nodo>, nodo::mayor_coste> frontier;
	set<estado> explorados;
	queue<Action> plan;
	nodo current_node, child;
	current_node.st = inicio;
	frontier.push(current_node);
	bool solution_found = es_solucion(current_node.st, final, nivel);

	while (!frontier.empty() and !solution_found)
	{
		//hijos_explorados++;
		frontier.pop();
		explorados.insert(current_node.st);

		if ((solution_found = es_solucion(current_node.st, final, nivel)))
		{	
			//nodos_abiertos = frontier.size();
			//nodos_cerrados = explorados.size();
			plan = current_node.secuencia;
			break;
		}
		for (const auto &action : acciones)
		{
			generar_hijo(action, current_node, child, nivel, final);
			if (explorados.find(child.st) == explorados.end())
				frontier.push(child);
		}

		if (!solution_found and !frontier.empty())
		{
			current_node = frontier.top();
			while (!frontier.empty() and explorados.find(current_node.st) != explorados.end())
			{
				frontier.pop();
				if (!frontier.empty())
					current_node = frontier.top();
			}
		}
	}
	return plan;
}
queue<Action> ComportamientoJugador::nivel_0_1(const estado &inicio, const ubicacion &final, const unsigned char nivel)
{
	const vector<Action> acciones = nivel == 0 ? acciones_jugador : acciones_jugador_colaborador;
	queue<nodo> frontier;
	set<estado> explorados;
	queue<Action> plan;
	nodo current_node, child;
	current_node.st = inicio;
	frontier.push(current_node);
	bool solution_found = es_solucion(current_node.st, final, nivel);

	while (!frontier.empty() and !solution_found)
	{
		//hijos_explorados++;
		frontier.pop();
		explorados.insert(current_node.st);

		for (const auto& action : acciones)
		{
			generar_hijo(action, current_node, child, nivel);
			if ((solution_found = es_solucion(child.st, final, nivel)))
			{
				//nodos_abiertos = frontier.size();
				//nodos_cerrados = explorados.size();
				plan = child.secuencia;
				break;
			}
			else if (explorados.find(child.st) == explorados.end())
				frontier.push(child);
		}

		if (!solution_found and !frontier.empty())
		{
			current_node = frontier.front();
			while (!frontier.empty() and explorados.find(current_node.st) != explorados.end())
			{
				frontier.pop();
				if (!frontier.empty())
					current_node = frontier.front();
			}
		}
	}
	return plan;
}

// ----------------- EL INTERACT -----------------

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}