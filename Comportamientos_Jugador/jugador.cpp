#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

#include <cstdlib>

// Te devuelve a qué casilla equivale en la orientación actual una posición en la orientación norte/noroeste
casilla ComportamientoJugador::casilla_equivalente(casilla c)
{
	short posfil, poscol;
	bool swap;

	switch (current_state.brujula)
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

	short posfil_tmp = swap ? c.second * poscol : c.first * poscol;
	short poscol_tmp = swap ? c.first * posfil : c.second * posfil;

	return make_pair(estado->fil + posfil_tmp, estado->col + poscol_tmp);
}

bool ComportamientoJugador::escanear_casilla(const casilla c)
{
	switch (mapaResultado[c.first][c.second])
	{
	// Bosque
	case 'B': (*mapa_potencial_actual)[c.first][c.second] = zapatillas ? -5 : -25; return false;
	// Agua
	case 'A': (*mapa_potencial_actual)[c.first][c.second] = bikini ? -5 : -50; return false;
	// Precipicio
	case 'P': (*mapa_potencial_actual)[c.first][c.second] = 0; return false;
	// Piedra
	case 'S': (*mapa_potencial_actual)[c.first][c.second] = -1; return false;
	// Arena
	case 'T': (*mapa_potencial_actual)[c.first][c.second] = -2; return false;
	// Muro
	case 'M': (*mapa_potencial_actual)[c.first][c.second] = 0; return false;
	
	// Casilla de ubicación
	case 'G': (*mapa_potencial_actual)[c.first][c.second] = ubicado ? -1 : 5000; if (!ubicado) casillas_interesantes.push(c); return true;
	// Bikini
	case 'K': (*mapa_potencial_actual)[c.first][c.second] = bikini ? -1 : 3000; if(!bikini) casillas_interesantes.push(c); return true;
	// Zapatillas
	case 'D': (*mapa_potencial_actual)[c.first][c.second] = zapatillas ? -1 : 3000; if(!zapatillas) casillas_interesantes.push(c); return true;
	// Recarga
	case 'X': (*mapa_potencial_actual)[c.first][c.second] = prioridad_recarga; casillas_interesantes.push(c); return true;
	// Desconocida
 	case '?': (*mapa_potencial_actual)[c.first][c.second] = 1; casillas_desconocidas.push(c); return true;

	// Jugador
	case 'a': (*mapa_potencial_actual)[c.first][c.second] = 0; return false;
	// Lobos
	case 'l': (*mapa_potencial_actual)[c.first][c.second] = 0; return false;	
	}
}

// Escanea las casillas del perímetro p y guarda los resultados en el mapa de potencial y las colas correspondientes
bool ComportamientoJugador::escanear_perimetro(const unsigned char perimetro)
{
	bool encontrada = false;

	for (size_t i = 0; i < (perimetro * 2); i++)
	{
		// Borde superior
		if (escanear_casilla(make_pair(estado->fil - perimetro, (estado->col - perimetro) + i)))
			encontrada = true;
		// Borde inferior
		if (escanear_casilla(make_pair(estado->fil + perimetro, (estado->col - perimetro) + i)))
			encontrada = true;
		// Borde izquierdo
		if (escanear_casilla(make_pair((estado->col - perimetro) + i , estado->col - perimetro)))
			encontrada = true;
		// Borde derecho
		if (escanear_casilla(make_pair((estado->col - perimetro) + i, estado->col + perimetro)))
			encontrada = true;
	}
	
	return encontrada;
}

// Genera el mapa de decisión con un radio inteligente según el mapa de potencial (debe estar relleno)
void ComportamientoJugador::generar_mapa_decision(const unsigned char radio, Sensores sensor)
{
	// Borro el mapa de decisión, si es que tuviese algo, para rellenarlo de nuevo
    for (auto& row : mapa_decision)
        row.clear();

	int distance;
	int optimal_size = radio;
	casilla c;

	// Buscamos la máxima distancia a la que se encuentra algún elemento interesante para calcular el tamaño de la matriz
	for (size_t i = 0; i < casillas_interesantes.size(); i++)
	{
		c = casillas_interesantes.front();
		distance = c.first > estado->fil ? c.first - estado->fil : estado->fil - c.first;
		optimal_size = optimal_size > distance ? optimal_size : distance;

		distance = c.second > estado->col ? c.second - estado->col : estado->col - c.second;
		optimal_size = optimal_size > distance ? optimal_size : distance;

		casillas_interesantes.push(c);
		casillas_interesantes.pop();
	}
	mapa_decision = vector<vector<int>>(optimal_size, vector<int>(optimal_size, 0));

	// Calculamos el efecto que tienen las casillas interesantes sobre el mapa de potencial
	for (size_t i = 0; i < casillas_interesantes.size(); i++)
	{
		c = casillas_interesantes.front();

		mapa_decision[c.first][c.second] = sensor.bateria + (*mapa_potencial_actual)[c.first][c.second];
		calcular_efecto_entorno(c);

		casillas_interesantes.push(c);
		casillas_interesantes.pop();
	}
	
	// Lo mismo pero para las casillas desconocidas
	for (size_t i = 0; i < casillas_desconocidas.size(); i++)
	{
		c = casillas_desconocidas.front();

		mapa_decision[c.first][c.second] = sensor.bateria + (*mapa_potencial_actual)[c.first][c.second];
		calcular_efecto_entorno(c);

		casillas_desconocidas.push(c);
		casillas_desconocidas.pop();
	}
}
// Complemento para la función "generar_mapa_decision"
void ComportamientoJugador::calcular_efecto_entorno(const casilla c)
{
	int casilla00 = mapa_decision[c.first][c.second];
	int potencial;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			// Si no son la casilla 00 y si no se salen del mapa, compruebo
			if ((i || j) && (c.first + i) >=0 && ((c.first + i) < mapa_decision.size()) && (c.second + j) >= 0 && ((c.second + j) < mapa_decision.size()))
			{
				potencial = (*mapa_potencial_actual)[c.first + i][c.second + j];

				// Si el potencial actual no es una barrera (0), veo si es menor que el que podría tener y si es así lo sustituyo
				if (potencial){ 
					if (mapa_decision[c.first + i][c.second + j] < (casilla00 + potencial)){
						mapa_decision[c.first + i][c.second + j] = casilla00 + potencial;
						calcular_efecto_entorno(make_pair(c.first + i, c.second + j));
					}
				}
			}
		}
	}
}

template <typename T>
void ComportamientoJugador::trasladar_mapa(const vector< vector<T>>& base, vector< vector<T>>& objetivo)
{
	unsigned char fake_state_aux = fake_state.brujula;
	if (fake_state_aux < current_state.brujula)
		fake_state_aux += 8;
	const unsigned short giros = (fake_state.brujula - current_state.brujula) / 2;
	const unsigned offset_fil = fake_state.fil - current_state.fil;
	const unsigned offset_col = fake_state.col - current_state.col;

	vector<vector<T>> matriz_rotada = vector<vector<T>>(objetivo.size() * 3, vector<T>(objetivo.size() * 3));
	for (size_t i = 0; i < giros; i++)
	{
		rotar_mapa_derecha(base, matriz_rotada);
		//Roto la posición 0,0
		int aux = fake_state.col;
		fake_state.col = base.size() - 1 - aux;
		fake_state.fil = fake_state.col;
	}
	
	// Recorto el tamaño util
	for (size_t i = 0; i < base.size(); i++)
		for (size_t j = 0; j < base.size(); j++)
			objetivo[i][j] = base[i + offset_fil][j + offset_col];
}

template <typename T>
void ComportamientoJugador::rotar_mapa_derecha(const vector<vector<T>> &base, vector<vector<T>> &objetivo)
{
	for (int i=0; i < objetivo.size(); i++)
		for (int j=0; j < objetivo.size(); j++)
			objetivo[j][objetivo.size()-1-i] = base[i][j];
}

void ComportamientoJugador::PonerTerrenoEnMatriz(Sensores sensor)
{
	const vector<pair<short, short>> pos_norte = { {0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, {-3, -3}, {-3, -2}, {-3, -1}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3} };
	const vector<pair<short, short>> pos_noreste = { {0, 0}, {-1, 0}, {-1, 1}, {0, 1}, {-2, 0}, {-2, 1}, {-2, 2}, {-1, 2}, {0, 2}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3}, {-2, 3}, {-1, 3}, {0, 3} };
	vector<pair<short, short>> vector_pos = pos_noreste;

	switch (estado->brujula)
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
		casilla c = casilla_equivalente(vector_pos[i]);
		(*mapa_actual)[c.first][c.second] = sensor.terreno[i];
		if (sensor.agentes[i] != '_' && sensor.agentes[i] != 'j')
			(*mapa_actual)[c.first][c.second] = sensor.agentes[i];
	 }
}

Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;	

	// Actualización según última acción
	switch (last_action)
	{
	case actWALK:
		switch (estado->brujula)
		{
			case norte: estado->fil -= walk_stepsize; break;
			case noreste: estado->fil -= walk_stepsize; estado->col += walk_stepsize; break;
			case este: estado->col += walk_stepsize; break;
			case sureste: estado->fil += walk_stepsize; estado->col += walk_stepsize; break;
			case sur: estado->fil += walk_stepsize; break;
			case suroeste: estado->fil += walk_stepsize; estado->col -= walk_stepsize; break;
			case oeste: estado->col -= walk_stepsize; break;
			case noroeste: estado->fil -= walk_stepsize; estado->col -= walk_stepsize; break;
		}
		break;

	case actRUN:
		switch (estado->brujula)
		{
			case norte: estado->fil -= run_stepsize; break;
			case noreste: estado->fil -= run_stepsize; estado->col += run_stepsize; break;
			case este: estado->col += run_stepsize; break;
			case sureste: estado->fil += run_stepsize; estado->col += run_stepsize; break;
			case sur: estado->fil += run_stepsize; break;
			case suroeste: estado->fil += run_stepsize; estado->col -= run_stepsize; break;
			case oeste: estado->col -= run_stepsize; break;
			case noroeste: estado->fil -= run_stepsize; estado->col -= run_stepsize; break;
		}
		break;

	case actTURN_SR:
		// Girar 45º derecha
		estado->brujula = static_cast<Orientacion> ((estado->brujula+1)%8);
		break;
	
	case actTURN_L:
		// Girar 90º izquierda
		estado->brujula = static_cast<Orientacion> ((estado->brujula+6)%8);
		break;
	}

	// Reviso batería y decido si necesito recargar o seguir recargando
	int porcentaje_bateria = (sensores.bateria / static_cast<float>(bateria_maxima)) * 100;
	if (porcentaje_bateria <= 50)
		prioridad_recarga = 6000 * ((100 - porcentaje_bateria) / 100.0f);
	else{
		prioridad_recarga = -1;
		recarga = 0;
	}
	
	// Si encuentro alguna pieza valiosa lo registro
	if (sensores.posF != -1 and !ubicado)
	{
		mapa_actual = &mapaResultado;
		mapa_potencial_actual = &mapa_potencial;
		estado = &current_state;
		estado->fil = sensores.posF;
		estado->col = sensores.posC;
		estado->brujula = sensores.sentido;
		ubicado = true;
		trasladar_mapa(mapa_resultado_temporal, mapaResultado);
		cerr << "heoheo" << endl;
		trasladar_mapa(mapa_potencial_temporal, mapa_potencial);
	}
	if (sensores.terreno[0] == 'K' and !bikini)
		bikini = true;
	if (sensores.terreno[0] == 'D' and !zapatillas)
		zapatillas = true;
	if (sensores.terreno[0] == 'X')
		recarga = recarga ? recarga - 1 : 5;
	
	// Registro lo que veo
	PonerTerrenoEnMatriz(sensores);
	
	// Decidir nueva acción
	if (recarga)
		accion = actIDLE;
	else
	{
		int mejor_valor, penalizacion = 0;
		unsigned char radio_vision = 1;
		do
		{
			while (!escanear_perimetro(radio_vision))
			{
				radio_vision++;
			}
			generar_mapa_decision(radio_vision, sensores);

			// Elijo la mejor opción
			penalizacion = (*mapa_potencial_actual)[estado->fil][estado->col];
			casilla equiv = casilla_equivalente(make_pair(-1, 0));
			mejor_valor = mapa_decision[equiv.first][equiv.second];
			accion = actWALK;

			equiv = casilla_equivalente(make_pair(0, -1));
			if (mapa_decision[equiv.first][equiv.second] - penalizacion > mejor_valor){
				accion = actTURN_L;
				mejor_valor = mapa_decision[equiv.first][equiv.second] - penalizacion;
			}

			equiv = casilla_equivalente(make_pair(-1, 1));
			if (mapa_decision[equiv.first][equiv.second] - penalizacion > mejor_valor){
				accion = actTURN_SR;
				mejor_valor =  mapa_decision[equiv.first][equiv.second] - penalizacion;
			}

			radio_vision++;
			
		} while (mejor_valor == 0);
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