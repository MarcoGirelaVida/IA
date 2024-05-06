#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"
// ----------------- FUNCIONES REACTIVAS -----------------

template <typename T>
bool ComportamientoJugador::esta_dentro_de_mapa(const ubicacion &ub, const vector<vector<T>> &mapa) const
{
	return ub.f >= 0 && ub.f < mapa.size() && ub.c >= 0 && ub.c < mapa.size();
}
bool ComportamientoJugador::escanear_objetivo(const ubicacion &ub)
{
	unsigned short distancia = distancia_manhattan(ub, c_state.jugador);
	char tmp;
	if ((casillas_agentes.find(ub) != casillas_agentes.end()))
	{
		tmp = (*mapa_actual)[ub.f][ub.c];
		(*mapa_actual)[ub.f][ub.c] = casillas_agentes[ub];
	}

	switch ((*mapa_actual)[ub.f][ub.c])
	{
	// Bikini
	case 'K':
		if(!c_state.bikini)
		{
			objetivos.push({ub, distancia*2});
			return true;
		}
		return false;
	// Zapatillas
	case 'D':
		if(!c_state.zapatillas)
		{
			objetivos.push({ub, distancia*2});
			return true;
		}
		return false;
	// Recarga
	case 'X':
		if (prioridad_recarga)
		{
			objetivos.push({ub, distancia*prioridad_recarga});
			return true;
		}
	    return false;
	// Desconocida
 	case '?': objetivos.push({ub, distancia*1}); return true;
	// Jugador
	case 'a': (*mapa_actual)[ub.f][ub.c] = tmp; casillas_agentes.erase(ub); return false;
	// Lobos
	case 'l': (*mapa_actual)[ub.f][ub.c] = tmp; casillas_agentes.erase(ub); return false;	
	// Casilla de ubicación
	//case 'G':
	//	if (!ubicado)
	//	{
	//		objetivos.push({ub, distancia*3});
	//		return true;
	//	}
	//	return false;
	default: return false;
	}
}
bool ComportamientoJugador::escanear_perimetro(const unsigned char perimetro)
{
	bool encontrada = false;
	ubicacion ub = c_state.jugador, ub_base = c_state.jugador;
	for (size_t i = 0; i < (perimetro * 2); i++)
	{
		// Borde superior
		ub.f = ub_base.f - perimetro;
		ub.c = (ub_base.c - perimetro) + i;
		if (esta_dentro_de_mapa(ub, *mapa_actual) && escanear_objetivo(ub))
			encontrada = true;

		// Borde inferior
		ub.f = ub_base.f + perimetro;
		ub.c = (ub_base.c + perimetro) - i;
		if (esta_dentro_de_mapa(ub, *mapa_actual) && escanear_objetivo(ub))
			encontrada = true;

		// Borde izquierdo
		ub.f = (ub_base.f + perimetro) - i;
		ub.c = ub_base.c - perimetro;
		if (esta_dentro_de_mapa(ub, *mapa_actual) && escanear_objetivo(ub))
			encontrada = true;

		// Borde derecho
		ub.f = (ub_base.f - perimetro) + i;
		ub.c = ub_base.c + perimetro;
		//mostrar_ubicacion(ub);
		//cerr << "ES CASILLA INTERESANTE?: " << boolalpha << escanear_objetivo(ub) << endl;;
		//cerr << "HEO HEO" << endl;
		if (esta_dentro_de_mapa(ub, *mapa_actual) && escanear_objetivo(ub))
			encontrada = true;
	}

	return encontrada;
}
void ComportamientoJugador::radar_objetivos()
{
	unsigned char radio_escaneado = 0;
	bool casilla_interesante_encontrada = false;
	do {
		radio_escaneado++;
		casilla_interesante_encontrada = escanear_perimetro(radio_escaneado) || casilla_interesante_encontrada;
	}
	while ((radio_escaneado < 10 || !casilla_interesante_encontrada) && (radio_escaneado <= (*mapa_actual).size()));
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
void ComportamientoJugador::registrar_sensores(Sensores sensor)
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
		if (sensor.terreno[i] != '?'){
			ubicacion ub = traductor_posicion(c_state.jugador, vector_pos[i].first, vector_pos[i].second);
			(*mapa_actual)[ub.f][ub.c] = sensor.terreno[i];
			if (sensor.agentes[i] != '_' && sensor.agentes[i] != 'j')
				casillas_agentes.insert(make_pair(ub, sensor.agentes[i]));
		}
	 }
}
void ComportamientoJugador::reset()
{
	hayPlan = false;
	ubicado = false;
	recargador_encontrado = false;
	c_state.jugador.f = (mapaResultado.size()*3) / 2;
	c_state.jugador.f = (mapaResultado.size()*3) / 2;
	c_state.jugador.brujula = norte;
	c_state.bikini = false;
	c_state.zapatillas = false;

	c_state.colaborador.f = -1;
	c_state.colaborador.c = -1;
	c_state.colaborador.brujula = norte;
	c_state.bikini_colab = false;
	c_state.zapatillas_colab = false;

	mapaResultado_falso = vector<vector<unsigned char>>(mapaResultado.size()*3, vector<unsigned char>(mapaResultado.size()*3, '?'));
	mapa_actual = &mapaResultado_falso;
}
void ComportamientoJugador::trasladar_mapa(vector<vector<unsigned char>>& base, vector<vector<unsigned char>>& rotado, const Sensores &sensores)
{
	int giros = 4 - ((((c_state.jugador.brujula - sensores.sentido) + 8 ) % 8) / 2);
	giros = giros == 4 ? 0 : giros;
	vector<vector<unsigned char>> matriz_girada = vector<vector<unsigned char>>(base.size(), vector<unsigned char>(base.size(), 0));

	for (size_t i = 0; i < giros; i++)
	{
		// Roto la matriz base y la copio en la matriz base
		rotar_mapa_derecha(base, matriz_girada);

		// Hago copia profunda de la matriz girada en la matriz base
		for (size_t i = 0; i < base.size(); i++)
			for (size_t j = 0; j < base.size(); j++)
				base[i][j] = matriz_girada[i][j];
		
		//Roto la posición 0, 0
		int aux = c_state.jugador.f;
		c_state.jugador.f = sensores.posF;
		c_state.jugador.c = base.size() - 1 - aux;

		aux = c_state.colaborador.f;
		c_state.colaborador.f = sensores.CLBposF;
		c_state.colaborador.c = base.size() - 1 - aux;

		//Roto casillas interesantes y desconocidas
		for (size_t i = 0; i < objetivos.size(); i++)
		{
			objetivo rotada = objetivos.top();
			unsigned short aux = rotada.first.f;
			rotada.first.f = rotada.first.c;
			rotada.first.c = base.size() - 1 - aux;

			objetivos.push(rotada);
			objetivos.pop();
		}
	}
	// Hago copia profunda de casillas interesantes falsas en casillas interesantes reales
	
	int ni = c_state.jugador.f - sensores.posF;
	int nj = c_state.jugador.c - sensores.posC;
	int fi = c_state.jugador.f + (rotado.size()-sensores.posF);
	int fj = c_state.jugador.c + (rotado.size()-sensores.posC);
	for (int i = ni; i < fi; i++)
		for (int j = nj; j < fj; j++)
			if (rotado[i-ni][j-nj] == '?')
				rotado[i-ni][j-nj] = base[i][j];

	// Reajusto al tamaño las casillas interesantes y desconociads
	for (size_t i = 0; i < objetivos.size(); i++)
	{
		objetivo reajustada = objetivos.top();
		reajustada.first.f -= ni;
		reajustada.first.c -= nj;
		objetivos.push(reajustada);
		objetivos.pop();
	}

	// Borro el mapa base
	for (auto& row : base)
		row.clear();
}
void ComportamientoJugador::rotar_mapa_derecha(vector<vector<unsigned char>> &mapaOriginal, vector<vector<unsigned char>> &mapaGirado)
{
	int n = mapaOriginal.size();
	for (int i=0; i < n; i++)
		for (int j=0; j < n; j++)
			mapaGirado[j][n-1-i] = mapaOriginal[i][j];
}

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
	cerr << "Pos: [" << ub.f << "][" << ub.c << "] = " << (*mapa_actual)[ub.f][ub.c] << ", Orientación: "; orientacion_string(ub.brujula); cerr << endl;
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
		hijo = generar_nodo(copy.front(), padre, 3);
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
void ComportamientoJugador::registrar_estado_nivel_4(const Sensores &sensores, const Action a)
{

	c_state = generar_estado(a, c_state, 4);
/*---------------------------------------------------------------------------------------------*/
	// Miro si me he muerto
	if (sensores.reset == true)
		reset();
/*---------------------------------------------------------------------------------------------*/
	// Si me ubico
	if (sensores.posF != -1 and !ubicado)
	{
		ubicado = true;
		mapa_actual = &mapaResultado;
		trasladar_mapa(mapaResultado_falso, mapaResultado, sensores);
		registrar_estado(sensores, c_state, goal);
	}

/*---------------------------------------------------------------------------------------------*/
	// Reviso batería y decido si necesito recargar o seguir recargando
	sensores.bateria;
	prioridad_recarga = (5 - ((sensores.bateria / 3000) * 5)) && ciclos_desde_ultima_recarga >= 250 && sensores.vida >= 250;

	// Si estoy en una casilla de recarga, descuento la batería
	if (sensores.terreno[0] == 'X')
		if (!(recargando = prioridad_recarga))
			ciclos_desde_ultima_recarga = 0;
/*---------------------------------------------------------------------------------------------*/
	// Buscar casillas interesantes y guardarlas en la cola de prioridad
	casillas_agentes.clear();
	registrar_sensores(sensores);
	while (!objetivos.empty())
		objetivos.pop();
	radar_objetivos();
	escanear_objetivo(c_state.jugador);
	objetivos.push({goal, 4});
}
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;

	if (sensores.nivel < 4)
	{
		ubicado = true;
		mapa_actual = &mapaResultado;
		registrar_estado(sensores, c_state, goal);
	}
	else
		registrar_estado_nivel_4(sensores, ultima_accion);

	if(!ubicado) return actWHEREIS;
	if (recargando) { ciclos_desde_ultima_recarga++; return actIDLE;}
	if (!hayPlan)
	{
			
		switch (sensores.nivel)
		{
		case 0: plan = nivel_0_1(c_state, goal, 0); break;
		case 1: plan = nivel_0_1(c_state, goal, 1); break;
		case 2: plan = nivel_2_3_4(c_state, goal, 2); break;
		case 3: plan = nivel_2_3_4(c_state, goal, 3); break;
		case 4: plan = nivel_2_3_4(c_state, objetivos.top().first, 3); break;
		}

		if (plan.empty()) { cerr << "ERROR: No se ha encontrado un plan" << endl; exit(1); }
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

		if (sensores.nivel == 4)
			hayPlan = objetivos.top().first == goal;
		accion = hayPlan ? plan.front() : actIDLE;
		plan.pop();
	}
	else
	{
		hayPlan = false;
		if (sensores.nivel == 4)
			objetivos.pop();
	}

	ultima_accion = accion;
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
	return (*mapa_actual)[pos.f][pos.c] != 'P' and (*mapa_actual)[pos.f][pos.c] != 'M' and !(cst.jugador.f == cst.colaborador.f and cst.jugador.c == cst.colaborador.c);
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
	
	switch ((*mapa_actual)[ub.f][ub.c])
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
		cerr << "Casilla introducida: " << (*mapa_actual)[ub.f][ub.c] << endl;
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
		if (st_accion_jugador != st or a == actIDLE)
		{
			salida = apply(salida.ultima_orden_colaborador, st_accion_jugador);
			if (salida == st_accion_jugador and st.ultima_orden_colaborador != act_CLB_STOP)
				salida = st;
		}
	}
	if (nivel < 2) return salida;

	salida.bikini = st.bikini;
	salida.zapatillas = st.zapatillas;
	if ((*mapa_actual)[salida.jugador.f][salida.jugador.c] == 'K')
	{ salida.bikini = true; salida.zapatillas = false; }
	else if ((*mapa_actual)[salida.jugador.f][salida.jugador.c] == 'D')
	{ salida.zapatillas = true; salida.bikini = false; }

	if (nivel < 3 or !ubicado) return salida;

	salida.bikini_colab = st.bikini_colab;
	salida.zapatillas_colab = st.zapatillas_colab;
	if ((*mapa_actual)[salida.colaborador.f][salida.colaborador.c] == 'K')
	{ salida.bikini_colab = true; salida.zapatillas_colab = false; }
	else if ((*mapa_actual)[salida.colaborador.f][salida.colaborador.c] == 'D')
	{ salida.zapatillas_colab = true; salida.bikini_colab = false; }

	return salida;
}
nodo ComportamientoJugador::generar_nodo(const Action a, const nodo &padre, const unsigned char nivel, const ubicacion &final) const
{
	nodo hijo;
	hijo.st = generar_estado(a, padre.st, nivel);
	hijo.secuencia = padre.secuencia;
	hijo.secuencia.push(a);

	if (nivel < 2) return hijo;
	hijo.coste_con_heuristica = hijo.coste = padre.coste + coste_accion_total(a, padre.st);

	if (nivel < 3) return hijo;
	hijo.coste_con_heuristica += nivel == 3 ? distancia_chebyshev(padre.st.colaborador, final) : 0;

	return hijo;
}

// ----------------- FUNCIONES DE LA BÚSQUEDA -----------------

queue<Action> ComportamientoJugador::nivel_2_3_4(const estado &inicio, const ubicacion &final, const unsigned char nivel)
{
	const vector<Action> acciones = nivel == 2 or nivel == 4 ? acciones_jugador : acciones_jugador_colaborador;
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
			child = generar_nodo(action, current_node, nivel);
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