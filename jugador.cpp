#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include "jugador.hpp"

template<typename T>
bool esta_en_cola(const queue<T> &q, const T n)
{
	queue<T> q_copy = q;
	while (!q_copy.empty())
	{
		if (q_copy.front() == n)
			return true;
		q_copy.pop();
	}
	return false;
}

bool ComportamientoJugador::esta_en_mapa(const int fil, const int col) const
{
	return fil >= 0 and fil < mapaResultado.size() and col >= 0 and col < mapaResultado[0].size();
}

bool ComportamientoJugador::esta_en_mapa(const estado &st) const
{
	return st.fila >= 0 and st.fila < mapaResultado.size() and st.columna >= 0 and st.columna < mapaResultado[0].size();
}

bool ComportamientoJugador::es_solucion(const nodo& n) const
{
	return n.st.fila == objetivo.fila and n.st.columna == objetivo.columna;
}

bool ComportamientoJugador::casilla_valida(const unsigned char casilla) const
{
	return casilla != 'P' and casilla != 'M';
}

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

void ComportamientoJugador::registra_secuencia(unsigned char decision, nodo &n) const
{
	if (actual.orientacion == noreste || actual.orientacion == sureste || actual.orientacion == suroeste || actual.orientacion == noroeste)
	{
		switch (decision)
		{
		case 0: decision = 3; break;
		case 1: decision = 0; break;
		case 2: decision = 1; break;
		case 3: decision = 6; break;
		case 5: decision = 2; break;
		case 6: decision = 7; break;
		case 7: decision = 8; break;
		case 8: decision = 5; break;
		}
	}
	
	switch (decision)
	{
	case 0: n.secuencia.push_back(actTURN_L); n.secuencia.push_back(actTURN_R);
	case 1: n.secuencia.push_back(actFORWARD);
	case 2: n.secuencia.push_back(actTURN_R);
	case 3: n.secuencia.push_back(actTURN_L);
	case 4: n.secuencia.push_back(actIDLE);
	case 5: n.secuencia.push_back(actTURN_R); n.secuencia.push_back(actTURN_R);
	case 6: n.secuencia.push_back(actTURN_L); n.secuencia.push_back(actTURN_L); n.secuencia.push_back(actTURN_R);
	case 7: n.secuencia.push_back(actTURN_L); n.secuencia.push_back(actTURN_L);
	case 8: n.secuencia.push_back(actTURN_R); n.secuencia.push_back(actTURN_R); n.secuencia.push_back(actTURN_R);
	}
}

list<nodo> ComportamientoJugador::acciones_validas(const nodo& n) const
{
	list<nodo> decisiones_validas;
	
	int decision = 0;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (i or j)
			{
				nodo hijo = n;
				hijo.st.fila += i;
				hijo.st.columna += j;

				if (esta_en_mapa(hijo.st) and !casilla_valida(mapaResultado[hijo.st.fila][hijo.st.columna]))
				{
					registra_secuencia(decision, hijo);
					decisiones_validas.push_back(hijo);
				}
			}
			decision++;
		}
	}

	return decisiones_validas;
}

list<Action> ComportamientoJugador::breadth_1st_search()
{
	nodo nodo_actual = {actual, {}};
	queue<nodo> frontera;
	set<nodo> explorados;
	frontera.push(nodo_actual);

	while (!frontera.empty() and !es_solucion(nodo_actual))
	{
		frontera.pop();
		explorados.insert(nodo_actual);

		for (const nodo& hijo : acciones_validas(nodo_actual))
		{
			if (es_solucion(hijo))
				nodo_actual = hijo;
			else if (explorados.find(hijo) == explorados.cend() && !esta_en_cola(frontera, hijo))
				frontera.push(hijo);
		}

		if (!es_solucion(nodo_actual))
			nodo_actual = frontera.front();
	}

	if (es_solucion(nodo_actual))
		return nodo_actual.secuencia;
	else
		return {};
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
		plan = breadth_1st_search();
		hayPlan = true;
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
	/*
	actual.fila = sensores.posF;
	actual.columna = sensores.posC;
	actual.orientacion = sensores.sentido;

	cout << "Fila: " << actual.fila << endl;
	cout << "Col : " << actual.columna << endl;
	cout << "Ori : " << actual.orientacion << endl;

	// Capturo los destinos
	cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
	objetivos.clear();
	for (int i = 0; i < sensores.num_destinos; i++)
	{
		estado aux;
		aux.fila = sensores.destino[2 * i];
		aux.columna = sensores.destino[2 * i + 1];
		objetivos.push_back(aux);
	}

	bool hay_plan = pathFinding(sensores.nivel, actual, objetivos, plan);

	return accion;
	*/
}

// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan)
{
	switch (level)
	{
	case 0:
		cout << "Demo\n";
		estado un_objetivo;
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Profundidad(origen, un_objetivo, plan);
		break;

	case 1:
		cout << "Optimo numero de acciones\n";
		// Incluir aqui la llamada al busqueda en anchura
		cout << "No implementado aun\n";
		break;
	case 2:
		cout << "Optimo en coste\n";
		// Incluir aqui la llamada al busqueda de costo uniforme/A*
		cout << "No implementado aun\n";
		break;
	case 3:
		cout << "Reto 1: Descubrir el mapa\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 1
		cout << "No implementado aun\n";
		break;
	case 4:
		cout << "Reto 2: Maximizar objetivos\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 2
		cout << "No implementado aun\n";
		break;
	}
	return false;
}

//---------------------- Implementación de la busqueda en anchura ---------------------------
bool ComportamientoJugador::casilla_transitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa) const
{
	return (mapa[x.f][x.c] != 'P' and mapa[x.f][x.c] != 'M');
}

ubicacion ComportamientoJugador::next_casilla(const ubicacion &pos) const
{
	return {traductor_posicion(pos, -1, 0).first, traductor_posicion(pos, -1, 0).second, pos.brujula};
}

estado ComportamientoJugador::apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa) const
{
	estado st_result = st;
	ubicacion sig_ubicacion, sig_ubicacion2;
	switch (a)
	{
	case actFORWARD:
		
		break;
	
	case
		act
	default:
		break;
	}
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
		vector<Action> actions = {actFORWARD, actTURN_L, actTURN_R};

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
//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla)
{
	if (casilla == 'P' or casilla == 'M')
		return true;
	else
		return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st)
{
	int fil = st.fila, col = st.columna;

	// calculo cual es la casilla de delante del agente
	switch (st.orientacion)
	{
	case 0:
		fil--;
		break;
	case 1:
		fil--;
		col++;
		break;
	case 2:
		col++;
		break;
	case 3:
		fil++;
		col++;
		break;
	case 4:
		fil++;
		break;
	case 5:
		fil++;
		col--;
		break;
	case 6:
		col--;
		break;
	case 7:
		fil--;
		col--;
		break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil < 0 or fil >= mapaResultado.size())
		return true;
	if (col < 0 or col >= mapaResultado[0].size())
		return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col]))
	{
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
		st.fila = fil;
		st.columna = col;
		return false;
	}
	else
	{
		return true;
	}
}

struct ComparaEstados
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};

// Implementación de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan)
{
	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			cout << "A ";
		}
		else if (*it == actTURN_R)
		{
			cout << "D ";
		}
		else if (*it == actSEMITURN_R)
		{
			cout << "d ";
		}
		else if (*it == actTURN_L)
		{
			cout << "I ";
		}
		else if (*it == actSEMITURN_L)
		{
			cout << "i ";
		}
		else
		{
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char>> &m)
{
	for (int i = 0; i < m[0].size(); i++)
	{
		for (int j = 0; j < m.size(); j++)
		{
			m[i][j] = 0;
		}
	}
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			switch (cst.orientacion)
			{
			case 0:
				cst.fila--;
				break;
			case 1:
				cst.fila--;
				cst.columna++;
				break;
			case 2:
				cst.columna++;
				break;
			case 3:
				cst.fila++;
				cst.columna++;
				break;
			case 4:
				cst.fila++;
				break;
			case 5:
				cst.fila++;
				cst.columna--;
				break;
			case 6:
				cst.columna--;
				break;
			case 7:
				cst.fila--;
				cst.columna--;
				break;
			}
			mapaConPlan[cst.fila][cst.columna] = 1;
		}
		else if (*it == actTURN_R)
		{
			cst.orientacion = (cst.orientacion + 2) % 8;
		}
		else if (*it == actSEMITURN_R)
		{
			cst.orientacion = (cst.orientacion + 1) % 8;
		}
		else if (*it == actTURN_L)
		{
			cst.orientacion = (cst.orientacion + 6) % 8;
		}
		else if (*it == actSEMITURN_L)
		{
			cst.orientacion = (cst.orientacion + 7) % 8;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
