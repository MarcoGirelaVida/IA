#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

#include <cstdlib> // Add missing include directive for the 'cstdlib' library
void PonerTerrenoEnMatriz(const vector<unsigned char> terreno, const state current_state, vector< vector< unsigned char> > &mapaResultado)
{
	const vector<pair<short, short>> pos_norte = { {0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, {-3, -3}, {-3, -2}, {-3, -1}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3} };
	const vector<pair<short, short>> pos_noreste = { {0, 0}, {-1, 0}, {-1, 1}, {0, 1}, {-2, 0}, {-2, 1}, {-2, 2}, {-1, 2}, {0, 2}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3}, {-2, 3}, {-1, 3}, {0, 3} };
	vector<pair<short, short>> vector_pos;
	short posfil, poscol;

	switch (current_state.brujula)
	{
	case norte: 	posfil = 1; 	poscol = 1;		vector_pos = pos_norte; 	break;
	case noreste: 	posfil = 1; 	poscol = 1; 	vector_pos = pos_noreste; 	break;
	case este: 		posfil = -1; 	poscol = 1;		vector_pos = pos_norte;		break;
	case sureste: 	posfil = -1; 	poscol = 1; 	vector_pos = pos_noreste;	break;
	case sur: 		posfil = -1; 	poscol = -1; 	vector_pos = pos_norte;		break;
	case suroeste: 	posfil = -1; 	poscol = -1; 	vector_pos = pos_noreste;	break;
	case oeste: 	posfil = 1; 	poscol = -1; 	vector_pos = pos_norte;		break;
	case noroeste: 	posfil = 1; 	poscol = -1; 	vector_pos = pos_noreste; 	break;
	}

	for (size_t i = 0; i < 16; i++)
	 {
		unsigned fil = current_state.fil + vector_pos[i].first*posfil;
		unsigned col = current_state.col + vector_pos[i].second*poscol;
		if (terreno[i] == 'G' || terreno[i] == 'B' || terreno[i] == 'R' || terreno[i] == 'Z')
			casillas_objetivo[{fil, col}] = terreno[i];
		else
			casillas_objetivo.erase({fil, col});

		mapaResultado[fil][col] = terreno[i];
	 }
}

void GenerarMapaPotencial (const unordered_map<pair<unsigned, unsigned>, unsigned char> casillas_objetivo, vector<vector<unsigned char>> & mapaResultado)
{
	queue<pair<unsigned, unsigned>> restantes;
}
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;	

	// Actualización según última acción
	switch (last_action)
	{
	case actWALK:
		switch (current_state.brujula)
		{
			case norte: current_state.fil -= walk_stepsize; break;
			case noreste: current_state.fil -= walk_stepsize; current_state.col += walk_stepsize; break;
			case este: current_state.col += walk_stepsize; break;
			case sureste: current_state.fil += walk_stepsize; current_state.col += walk_stepsize; break;
			case sur: current_state.fil += walk_stepsize; break;
			case suroeste: current_state.fil += walk_stepsize; current_state.col -= walk_stepsize; break;
			case oeste: current_state.col -= walk_stepsize; break;
			case noroeste: current_state.fil -= walk_stepsize; current_state.col -= walk_stepsize; break;
		}
		break;

	case actRUN:
		switch (current_state.brujula)
		{
			case norte: current_state.fil -= run_stepsize; break;
			case noreste: current_state.fil -= run_stepsize; current_state.col += run_stepsize; break;
			case este: current_state.col += run_stepsize; break;
			case sureste: current_state.fil += run_stepsize; current_state.col += run_stepsize; break;
			case sur: current_state.fil += run_stepsize; break;
			case suroeste: current_state.fil += run_stepsize; current_state.col -= run_stepsize; break;
			case oeste: current_state.col -= run_stepsize; break;
			case noroeste: current_state.fil -= run_stepsize; current_state.col -= run_stepsize; break;
		}
		break;

	case actTURN_SR:
		// Girar 45º derecha
		current_state.brujula = static_cast<Orientacion> ((current_state.brujula+1)%8);
		break;
	
	case actTURN_L:
		// Girar 90º izquierda
		current_state.brujula = static_cast<Orientacion> ((current_state.brujula+6)%8);
		break;
	}

	if (sensores.terreno[0] == 'G' and !bien_situado)
	{
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
	}

	if (bien_situado)
	{
		PonerTerrenoEnMatriz(sensores.terreno, current_state, mapaResultado);
	}
	
	// Decidir nueva acción
	if ((sensores.terreno[2] == 'T' or sensores.terreno[2] == 'S' or
		sensores.terreno[2] == 'G') and sensores.agentes[2] == '_')
	{
		accion = actWALK;
	}
	else if (!girar_derecha)
	{
		accion = actTURN_L;
		girar_derecha = (rand()%2 == 0);
	}
	else
	{
		accion = actTURN_SR;
		girar_derecha = (rand()%2 == 0);
	}
	
	last_action = accion;

	// Mostrar el valor de los sensores
	cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC;
	switch (sensores.sentido)
	{
		case norte:	  cout << " Norte\n";	break;
		case noreste: cout << " Noreste\n";	break;
		case este:    cout << " Este\n";	break;
		case sureste: cout << " Sureste\n";	break;
		case sur:     cout << " Sur\n";	break;
		case suroeste:cout << " Suroeste\n";	break;
		case oeste:   cout << " Oeste\n";	break;
		case noroeste:cout << " Noroeste\n";	break;
	}
	cout << "Terreno: ";
	for (int i=0; i<sensores.terreno.size(); i++)
		cout << sensores.terreno[i];

	cout << "  Agentes: ";
	for (int i=0; i<sensores.agentes.size(); i++)
		cout << sensores.agentes[i];

	cout << "\nColision: " << sensores.colision;
	cout << "  Reset: " << sensores.reset;
	cout << "  Vida: " << sensores.vida << endl<< endl;

	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}