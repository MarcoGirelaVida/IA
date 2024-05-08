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
	case actWHEREIS: 		cout << "WHEREIS, "; 	break;
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
			cerr << "{"; PintaPlan((copy.front()).secuencia()); cerr << "}, ";
		}
		copy.pop();
		num_paso++;
	}
}
void ComportamientoJugador::mostrar_lista(const priority_queue<nodo, vector<nodo>, nodo::mayor_coste> &q, bool completa) const
{
	size_t num_paso = 1;
	priority_queue<nodo, vector<nodo>, nodo::mayor_coste> copy = q;
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
			cerr << "{"; PintaPlan((copy.top()).secuencia()); cerr << "}, ";
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
	cerr << "¿PADRE NULLPTR?: " << boolalpha << (nd.p_nodo_accion->padre == nullptr) << endl; 
	if (nivel > 1) 			{ cerr << "Coste acumulado: " << 3000 - nd.coste << endl; }
	if (mostrar_secuencia) 	{ cerr << endl << "Secuencia: "; PintaPlan(nd.secuencia()); }
	else 					{ cerr << endl << "Accion: "; accion_string(nd.p_nodo_accion->accion); cerr << endl; }
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

queue<nodo> ComportamientoJugador::generar_nodos_secuencia(const queue<Action> &plan, const estado &inicio, const unsigned char nivel) const
{
	queue<nodo> nodos;
	queue<Action> copy = plan;
	nodo padre, hijo;
	padre.st = inicio;
	while (!copy.empty())
	{
		hijo = generar_nodo(copy.front(), padre, nivel);
		nodos.push(hijo);
		//consumo_total_bateria += coste_accion_total(copy.front(), padre.st);
		padre = hijo;
		copy.pop();
	}
	return nodos;
}
queue<estado> ComportamientoJugador::generar_estados_secuencia(queue<Action> &plan, const estado &inicio, const unsigned char nivel) const
{
	queue<estado> estados;
	queue<Action> copy = plan;
	estado st = inicio;
	estados.push(st);
	while (!copy.empty())
	{
		st = generar_estado(copy.front(), st, nivel);
		estados.push(st);
		copy.pop();
	}
	return estados;
}

// ----------------- FUNCIONES MAIN -----------------

queue<Action> ComportamientoJugador::generar_plan(const estado &origen, const ubicacion &destino, const unsigned char nivel)
{
	while (!plan.empty()) plan.pop();
	switch (nivel)
	{
	case 0: plan = nivel_0_1(origen, destino, 0); break;
	case 1: plan = nivel_0_1(origen, destino, 1); break;
	case 2: plan = nivel_2_3_4(origen, destino, 2); break;
	case 3: plan = nivel_2_3_4(origen, destino, 3); break;
	case 4: plan = nivel_2_3_4(origen, destino, 2); break;
	}
	//if (plan.empty() and nivel != 4) { cerr << "ERROR: No se ha encontrado un plan" << endl; exit(1); }

	VisualizaPlan(c_state, plan);
	hayPlan = true;

	//cout << "--------PLAN ENCONTRADO--------" << endl;
	//cerr << "NIVEL: " << (int) nivel << endl;
	//cerr << "ORIGEN: "; cerr << endl; mostrar_estado(c_state, nivel); cerr << endl;
	//cerr << "GOAL: "; mostrar_ubicacion(destino);
	//PintaPlan(plan); cerr << endl;
	//cerr << "PASOS TOTALES: " << plan.size() << endl;
	//cerr << "INSTANTES ESPERADOS: " << 3000 - plan.size() << endl;
	//cerr << "-----------------------" << endl;

	//paso++;
	//plan_nodos = generar_nodos_secuencia(plan, c_state, sensores.nivel);
	//cerr << "----- PASO " <<  paso << " -----" << endl;
	//mostrar_nodo(plan_nodos.front(), sensores.nivel, false);
	//cerr << "-----------------------" << endl;
	//plan_nodos.pop();
	if (nivel == 3) plan_con_colaborador = true;
	else plan_con_colaborador = false;
	while (!estados_teoricos.empty()) estados_teoricos.pop();
	estados_teoricos = generar_estados_secuencia(plan, c_state, nivel);
	estados_teoricos.pop();

	return plan;
}
void ComportamientoJugador::poner_bordes_en_matriz()
{
	// Borde superior
	for (size_t i = 0; i < 3; i++)
		for (size_t j = 0; j < mapaResultado.size(); j++)
			mapaResultado[i][j] = 'P';
	// Borde inferior
	for (size_t i = mapaResultado.size() - 3; i < mapaResultado.size(); i++)
		for (size_t j = 0; j < mapaResultado.size(); j++)
			mapaResultado[i][j] = 'P';
	// Borde izquierdo
	for (size_t i = 0; i < mapaResultado.size(); i++)
		for (size_t j = 0; j < 3; j++)
			mapaResultado[i][j] = 'P';
	// Borde derecho
	for (size_t i = 0; i < mapaResultado.size(); i++)
		for (size_t j = mapaResultado.size() - 3; j < mapaResultado.size(); j++)
			mapaResultado[i][j] = 'P';
}
ubicacion ComportamientoJugador::determinar_objetivo(const Sensores &sensores)
{
	ubicacion objetivo = {0, 0, norte};
	if (hay_que_recargar(sensores.bateria, sensores.vida) and !recargadores.empty())
	{
		objetivo = recargador_mas_cercano();
		buscando_recargador = true;
	}
	else if (!colaborador_encontrado)
	{
		objetivo = traductor_posicion(c_state.colaborador, 1, 0);
	}
	else
	{
		objetivo.f = sensores.destinoF;
		objetivo.c = sensores.destinoC;
	}
	return objetivo;
}
void ComportamientoJugador::registrar_sensor_terreno(const Sensores &sensor)
{
	const vector<pair<short, short>> pos_norte = { {0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, {-3, -3}, {-3, -2}, {-3, -1}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3} };
	const vector<pair<short, short>> pos_noreste = { {0, 0}, {-1, 0}, {-1, 1}, {0, 1}, {-2, 0}, {-2, 1}, {-2, 2}, {-1, 2}, {0, 2}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3}, {-2, 3}, {-1, 3}, {0, 3} };
	vector<pair<short, short>> vector_pos = pos_norte;

	switch (c_state.jugador.brujula)
	{
	case noreste: 	vector_pos = pos_noreste; 	break;
	case noroeste: 	vector_pos = pos_noreste; 	break;
	case sureste: 	vector_pos = pos_noreste;	break;
	case suroeste: 	vector_pos = pos_noreste;	break;
	}

	for (size_t i = 0; i < 16; i++)
	 {
		ubicacion ub = traductor_posicion(c_state.jugador, vector_pos[i].first, vector_pos[i].second);
		mapaResultado[ub.f][ub.c] = sensor.terreno[i];
		//if (sensor.agentes[i] != '_' && sensor.agentes[i] != 'j')
		//	casillas_agentes.push(make_pair(ub, sensor.agentes[i]));
		if (sensor.terreno[i] == 'X')
			recargadores.push(ub);
	 }
}
void ComportamientoJugador::registrar_sensor_ubicacion(const Sensores &sensores)
{
	c_state.jugador.f = sensores.posF;
	c_state.jugador.c = sensores.posC;
	c_state.jugador.brujula = sensores.sentido;
	c_state.colaborador.f = sensores.CLBposF;
	c_state.colaborador.c = sensores.CLBposC;
	c_state.colaborador.brujula = sensores.CLBsentido;
	goal = {sensores.destinoF, sensores.destinoC, norte};
}
void ComportamientoJugador::reset()
{
	hayPlan = false;
	ubicado = false;
	buscando_recargador = false;
	colaborador_encontrado = true;
	plan_con_colaborador = false;
	recargando = false;
	c_state.jugador.f = -1;
	c_state.jugador.f = -1;
	c_state.jugador.brujula = norte;
	c_state.bikini = false;
	c_state.zapatillas = false;

	c_state.colaborador.f = -1;
	c_state.colaborador.c = -1;
	c_state.colaborador.brujula = norte;
	c_state.bikini_colab = false;
	c_state.zapatillas_colab = false;

	goal = {-1, -1, norte};
}
void ComportamientoJugador::registrar_estado(const Sensores &sensores, const Action a)
{
	// Actualizo el estado de acuerdo con los sensores
	if (sensores.posF != -1)
	{
		ubicado = true;
		registrar_sensor_ubicacion(sensores); // niveles anteriores
	}
	else
		c_state = generar_estado(ultima_accion, c_state, sensores.nivel);

	
	if (sensores.nivel < 4) return;
	registrar_sensor_terreno(sensores); // nivel4

/*---------------------------------------------------------------------------------------------*/

	// Actualizo el objetivo si es necesario
	ubicacion nueva_goal = determinar_objetivo(sensores);
	hayPlan = hayPlan and (goal == nueva_goal);
	if (not hayPlan) goal = nueva_goal;

	//cerr << "Objetivo: "; mostrar_ubicacion(nueva_goal);
	//cerr << "Antiguos: "; mostrar_ubicacion(goal);
/*---------------------------------------------------------------------------------------------*/

	// Si estoy en una casilla de recarga, compruebo si debo seguir recargando
	if (sensores.terreno[0] == 'X')
		if (!(recargando = ((sensores.bateria / 3000.0) < umbral_porcentaje_bateria_maximo) and (sensores.vida >= umbral_vida)))
			ciclos_desde_ultima_recarga = 0;

/*---------------------------------------------------------------------------------------------*/
}
Action ComportamientoJugador::think(Sensores sensores)
{
	umbral_ciclos_recarga = sensores.umbral_ciclos_recarga;
	umbral_vida = sensores.umbral_vida;
	umbral_porcentaje_bateria = sensores.umbral_porcentaje_bateria;
	umbral_porcentaje_bateria_maximo = sensores.umbral_porcentaje_bateria_maximo;

	if(sensores.nivel == 4 and !ubicado)	{ultima_accion = actWHEREIS; ubicado = true; return actWHEREIS;}

	// Actualizo el estado actual y recargo si debo seguir recargando
	Action accion = actIDLE;
	registrar_estado(sensores, ultima_accion);
	if (sensores.nivel == 4 and ultima_accion == act_CLB_WALK or ultima_accion == act_CLB_TURN_SR and !hayPlan) {ultima_accion = act_CLB_STOP; ciclos_desde_ultima_recarga++; return act_CLB_STOP;}
	if (!hayPlan and sensores.nivel == 4 and (c_state.ultima_orden_colaborador == act_CLB_WALK or c_state.ultima_orden_colaborador == act_CLB_TURN_SR)) {c_state.ultima_orden_colaborador = act_CLB_STOP;}
	if (recargando) {ultima_accion = actIDLE; return actIDLE;} else ciclos_desde_ultima_recarga++;

	if (!hayPlan)
	{
		plan = generar_plan(c_state, goal, sensores.nivel);
		if (plan.empty())
		{
			//cerr << "REPLANIFICANDO PORQUE NO SE ENCONTRO UN PLAN" << endl;
			if (!colaborador_encontrado)
			{
				colaborador_encontrado = true;
				hayPlan = false;
				ultima_accion = actIDLE;
				return actIDLE;
			}
			else if (goal.f == sensores.destinoF and goal.c == sensores.destinoC)
				plan = generar_plan(c_state, goal, 3);
		}
		//accion = plan.front();
		//plan.pop();
		if (!plan.empty())
		{
			accion = plan.front();
			plan.pop();
		}
		else
		{
			ultima_accion = actWHEREIS;
			return actWHEREIS;
		}
	}
	else if (plan.size())
	{
		//paso++;
		//cerr << "-----------------------" << endl << endl;
		//cerr << "----- PASO " <<  paso << " -----" << endl;
		//mostrar_nodo(plan_nodos.front(), sensores.nivel, false);
		//cerr << "-----------------------" << endl;
		//plan_nodos.pop();

		if (sensores.nivel == 4)
		{
			estados_teoricos.pop();
			estado estado_real = generar_estado(plan.front(), c_state, sensores.nivel);
			if(estados_teoricos.front() != estado_real or (!plan_con_colaborador and accion_costosa(accion, estado_real, c_state)))
			{
				//cerr << "REAJUSTANDO PLAN" << endl;
				hayPlan = false;
				ultima_accion = actIDLE;
				return actIDLE;
			}
		}
		//accion = plan.front();
		//plan.pop();
		if (!plan.empty())
		{
			accion = plan.front();
			plan.pop();
		}
		else
		{
			ultima_accion = actWHEREIS;
			return actWHEREIS;
		}
	}
	else
		hayPlan = false;

	ultima_accion = accion;
	return accion;
}

// ----------------- FUNCIONES DE LA CONSULTA -----------------

ubicacion ComportamientoJugador::recargador_mas_cercano() const
{
	queue<ubicacion> copia_recargadores = recargadores;
	ubicacion recargador_mas_cercano = copia_recargadores.front();
	
	while (!copia_recargadores.empty())
	{
		ubicacion recargador = copia_recargadores.front();

		if (distancia_chebyshev(c_state.jugador, recargador) < distancia_chebyshev(c_state.jugador, recargador_mas_cercano))
			recargador_mas_cercano = recargador;
		
		copia_recargadores.pop();
	}

	return recargador_mas_cercano;
}
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

bool ComportamientoJugador::hay_que_recargar(const unsigned short bateria_actual, const unsigned short vida_actual) const
{
	return ((bateria_actual / 3000.0) <= umbral_porcentaje_bateria) &&
	//distancia_chebyshev(c_state.jugador, recargador_mas_cercano())*umbral_vida) &&
	(vida_actual >= umbral_vida) &&
	(ciclos_desde_ultima_recarga >= umbral_ciclos_recarga);
}
bool ComportamientoJugador::accion_costosa(const Action a, const estado &st, const estado &st_antiguo) const
{
	return (mapaResultado[st.jugador.f][st.jugador.c] == 'A' and !st.bikini )
			or (mapaResultado[st.jugador.f][st.jugador.c] == 'B' and !st.zapatillas );
	//bool costosa = false;
	//if (a == act_CLB_WALK or a == act_CLB_TURN_SR or st.ultima_orden_colaborador == act_CLB_WALK or st.ultima_orden_colaborador == act_CLB_TURN_SR)
	//	costosa = (st.valor_casilla_colaborador == 'A' and !st.bikini_colab and st_antiguo.valor_casilla_colaborador != 'A')
	//			or (st.valor_casilla_colaborador == 'B' and !st.zapatillas_colab and st_antiguo.valor_casilla_colaborador != 'B');
	//if (!costosa)
	//	return (st.valor_casilla_jugador == 'A' and !st.bikini )//and st_antiguo.valor_casilla_jugador != 'A')
	//		 	or (st.valor_casilla_jugador == 'B' and !st.zapatillas );//and st_antiguo.valor_casilla_jugador != 'B');
	//return costosa;
}
bool ComportamientoJugador::esta_en_rango_vision(const estado &st) const
{
	const vector<pair<short, short>> pos_norte = { {0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, {-3, -3}, {-3, -2}, {-3, -1}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3} };
	const vector<pair<short, short>> pos_noreste = { {0, 0}, {-1, 0}, {-1, 1}, {0, 1}, {-2, 0}, {-2, 1}, {-2, 2}, {-1, 2}, {0, 2}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3}, {-2, 3}, {-1, 3}, {0, 3} };
	vector<pair<short, short>> vector_pos = pos_norte;

	switch (st.jugador.brujula)
	{
	case noreste: 	vector_pos = pos_noreste; 	break;
	case noroeste: 	vector_pos = pos_noreste; 	break;
	case sureste: 	vector_pos = pos_noreste;	break;
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
	case '?': return 1; //Desconocida
	// Muro case 'M': return 0;
	// Precipicio case 'P': return 0;
	// Jugador case 'a': return 0;
	default:
		cerr << "ERROR EN COSTE ACCION" << endl;
		cerr << "Casilla introducida: " << (char) mapaResultado[ub.f][ub.c] << endl;
		exit(1);
	}
}

// ----------------- FUNCIONES DE LA AUXILIARES -----------------

estado ComportamientoJugador::apply(const Action &a, const estado &st) const
{
	estado salida, st_sig, st_sig2;
	salida = st_sig = st_sig2 = st;
	if (a == act_CLB_WALK) 	  st_sig.colaborador = next_casilla(st.colaborador);
	else 					{ st_sig.jugador = next_casilla(st.jugador); st_sig2.jugador = next_casilla(st_sig.jugador); }

	switch (a)
	{
	// ----------------- JUGADOR -----------------
	case actWALK: if (casilla_transitable(st_sig, false)) salida = st_sig; break;
	case actRUN: if (casilla_transitable(st_sig, false) && casilla_transitable(st_sig2, false)) salida = st_sig2; break;
	case actTURN_L: salida.jugador.brujula = static_cast<Orientacion>((salida.jugador.brujula+6)%8); break;
	case actTURN_SR: salida.jugador.brujula = static_cast<Orientacion>((salida.jugador.brujula+1)%8); break;
	case actIDLE: break;
	case actWHEREIS: break;

	// ----------------- COLABORADOR -----------------
	case act_CLB_STOP: salida.ultima_orden_colaborador = act_CLB_STOP; break;
	case act_CLB_WALK: if (casilla_transitable(st_sig, true)) { salida = st_sig; salida.ultima_orden_colaborador = act_CLB_WALK; } break;
	case act_CLB_TURN_SR:
		salida.colaborador.brujula = static_cast<Orientacion>((salida.colaborador.brujula+1)%8);
		salida.ultima_orden_colaborador = act_CLB_TURN_SR;
		break;

	default:
		cerr << "ERROR EN LA APLICACIÓN DE LA ACCIÓN" << endl;
		cerr << "Acción introducida: "; accion_string(a); cerr << endl;
		exit(1);
	}
	return salida;
}

unsigned short ComportamientoJugador::distancia_manhattan(const ubicacion &ub, const ubicacion &final) const
{
	//return esta_en_rango_vision(st) ? abs(final.f-st.colaborador.f) + abs(final.f-st.colaborador.c) : abs(st.colaborador.f-st.jugador.f) + abs(st.colaborador.c-st.jugador.c);
	return abs(final.f-ub.f) + abs(final.f-ub.c);
}
unsigned short ComportamientoJugador::distancia_chebyshev(const ubicacion &ub, const ubicacion &final) const
{
	//return esta_en_rango_vision(st) ? max(abs(final.f - st.colaborador.f), abs(final.c - st.colaborador.c)) : max(abs(st.colaborador.f - st.jugador.f), abs(st.colaborador.c - st.jugador.c));
	return max(abs(ub.f - final.f), abs(ub.c - final.c));
}

estado ComportamientoJugador::generar_estado(const Action a, const estado &st, const unsigned char nivel) const
{
	estado st_accion_jugador, salida = st;
	if ((a == act_CLB_STOP or a == act_CLB_TURN_SR or a == act_CLB_WALK))
	{
		if (esta_en_rango_vision(st))
			salida = apply(a, st);
	}
	else 
	{
		st_accion_jugador = apply(a, st);
		if (st_accion_jugador != st or a == actIDLE or a == actWHEREIS)
		{
			salida = apply(salida.ultima_orden_colaborador, st_accion_jugador);
			if (salida == st_accion_jugador and st.ultima_orden_colaborador != act_CLB_STOP)
				salida = st;
		}
	}
	//salida.valor_casilla_colaborador = mapaResultado[salida.colaborador.f][salida.colaborador.c];
	//salida.valor_casilla_jugador = mapaResultado[salida.jugador.f][salida.jugador.c];

	if (nivel < 2) return salida;

	salida.bikini = st.bikini;
	salida.zapatillas = st.zapatillas;
	if (mapaResultado[salida.jugador.f][salida.jugador.c] == 'K')
	{ salida.bikini = true; salida.zapatillas = false; }
	else if (mapaResultado[salida.jugador.f][salida.jugador.c] == 'D')
	{ salida.zapatillas = true; salida.bikini = false; }

	if (nivel < 3) return salida;

	salida.bikini_colab = st.bikini_colab;
	salida.zapatillas_colab = st.zapatillas_colab;
	if (mapaResultado[salida.colaborador.f][salida.colaborador.c] == 'K')
	{ salida.bikini_colab = true; salida.zapatillas_colab = false; }
	else if (mapaResultado[salida.colaborador.f][salida.colaborador.c] == 'D')
	{ salida.zapatillas_colab = true; salida.bikini_colab = false; }

	return salida;
}
nodo ComportamientoJugador::generar_nodo(const Action a, const nodo &padre, const unsigned char nivel, const ubicacion &final) const
{
	nodo hijo;
	hijo.st = generar_estado(a, padre.st, nivel);
	hijo.p_nodo_accion = new nodo_accion(a, padre.p_nodo_accion);
	//hijo.secuencia = padre.secuencia;
	//hijo.secuencia.push(a);

	if (nivel < 2) return hijo;
	hijo.coste_con_heuristica = hijo.coste = padre.coste + coste_accion_total(a, padre.st);

	if (nivel < 3) return hijo;
	hijo.coste_con_heuristica += nivel == 3 ? distancia_chebyshev(padre.st.colaborador, final) : distancia_chebyshev(padre.st.jugador, final);

	return hijo;
}

void ComportamientoJugador::liberar_memoria_grafo(queue<nodo> &frontier) const
{
	while (!frontier.empty())
	{
		cerr << "Liberando memoria" << endl;
		liberar_memoria_nodo(frontier.front().p_nodo_accion);
		frontier.pop();
	}
}
void ComportamientoJugador::liberar_memoria_grafo(priority_queue<nodo, vector<nodo>, nodo::mayor_coste> &frontier) const
{
	while (!frontier.empty())
	{

		liberar_memoria_nodo(frontier.top().p_nodo_accion);
		frontier.pop();
	}
}
void ComportamientoJugador::liberar_memoria_nodo(const nodo_accion *nodo) const
{
	if (nodo->padre != nullptr)
		liberar_memoria_nodo(nodo->padre);
	delete nodo;
}

// ----------------- FUNCIONES DE LA BÚSQUEDA -----------------

queue<Action> ComportamientoJugador::nivel_2_3_4(const estado &inicio, const ubicacion &final, const unsigned char nivel)
{
	const vector<Action> acciones = nivel == 2 or nivel == 4 ? acciones_jugador : acciones_jugador_colaborador;
	priority_queue<nodo, vector<nodo>, nodo::mayor_coste> frontier;
	unordered_set<estado, estado_hash> explorados;
	//set<estado> explorados;
	queue<Action> plan;
	nodo current_node, child;
	current_node.st = inicio;
	current_node.p_nodo_accion = new nodo_accion();
	frontier.push(current_node);
	bool solution_found = es_solucion(current_node.st, final, nivel);

	while (!frontier.empty() and !solution_found)
	{
		//cerr << "EXPLORANDO NODO: " << endl; mostrar_nodo(frontier.top(), nivel);
		hijos_explorados++;
		frontier.pop();
		explorados.insert(current_node.st);
		if ((solution_found = es_solucion(current_node.st, final, nivel)))
		{
			//nodos_abiertos = frontier.size();
			//nodos_cerrados = explorados.size();
			plan = current_node.secuencia();
			liberar_memoria_nodo(current_node.p_nodo_accion);
			break;
		}
		for (const auto &action : acciones)
		{
			//cerr << "Estoy generando un hijo" << endl;
			child = generar_nodo(action, current_node, nivel, final);
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
		//cerr << "TAMAÑO FRONTERA: " << frontier.size() << endl << endl;
		//cerr << "FRONTIER: " << endl; mostrar_lista(frontier, false); cerr << endl << endl;
	}

	return plan;
}
queue<Action> ComportamientoJugador::nivel_0_1(const estado &inicio, const ubicacion &final, const unsigned char nivel)
{
	const vector<Action> acciones = nivel == 0 ? acciones_jugador : acciones_jugador_colaborador;
	queue<nodo> frontier;
	unordered_set<estado, estado_hash> explorados;
	//set<estado> explorados;
	queue<Action> plan;
	nodo current_node, child;
	current_node.st = inicio;
	current_node.p_nodo_accion = new nodo_accion();
	frontier.push(current_node);
	bool solution_found = es_solucion(current_node.st, final, nivel);

	while (!frontier.empty() and !solution_found)
	{
		//hijos_explorados++;
		frontier.pop();
		explorados.insert(current_node.st);

		for (const auto& action : acciones)
		{
			child = generar_nodo(action, current_node, nivel);
			if ((solution_found = es_solucion(child.st, final, nivel)))
			{
				//nodos_abiertos = frontier.size();
				//nodos_cerrados = explorados.size();
				plan = child.secuencia();
				liberar_memoria_nodo(child.p_nodo_accion);
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