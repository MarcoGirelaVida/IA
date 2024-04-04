#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>interesantes
using namespace std;

#include <cstdlib>

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

		mapa_decision[c.first][c.second] = sensor.bateria + mapa_decision[c.first][c.second];
		calcular_efecto_entorno(c, sensor.bateria);

		casillas_interesantes.push(c);
		casillas_interesantes.pop();
	}
	
	// Lo mismo pero para las casillas desconocidas
	for (size_t i = 0; i < casillas_desconocidas.size(); i++)
	{
		c = casillas_desconocidas.front();

		mapa_decision[c.first][c.second] = sensor.bateria + mapa_decision[c.first][c.second];
		calcular_efecto_entorno(c, sensor.bateria);

		casillas_desconocidas.push(c);
		casillas_desconocidas.pop();
	}
}
// Complemento para la función "generar_mapa_decision"
void ComportamientoJugador::calcular_efecto_entorno(const casilla c, const int bateria)
{
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (i || j)
			{
				int potencial = (*mapa_potencial_actual)[c.first + i][c.second + j];

				if (potencial){ 
					if (mapa_decision[c.first + i][c.second + j] < potencial + bateria){
						mapa_decision[c.first + i][c.second + j] = potencial + bateria;
						calcular_efecto_entorno(make_pair(c.first + i, c.second + j), bateria);
					}
				}
			}
		}
	}
}

template <typename T>
void ComportamientoJugador::trasladar_mapa(vector< vector<T>>& base, const vector< vector<T>>& objetivo)
{
	if (fake_state.brujula < current_state.brujula)
		fake_state.brujula += 8;
	
	const short offset_fil = fake_state.fil - current_state.fil;
	const short offset_col = fake_state.col - current_state.col;

	for (size_t i = 0; i < base.size(); i++)
		for (size_t j = 0; j < base.size(); j++)
			objetivo[i][j] = base[i + offset_fil][j + offset_col];
}

void ComportamientoJugador::PonerTerrenoEnMatriz(const vector<unsigned char> terreno, const state current_state, vector< vector< unsigned char> > &mapaResultado, const short nivel)
{
	const vector<pair<short, short>> pos_norte = { {0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, {-3, -3}, {-3, -2}, {-3, -1}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3} };
	const vector<pair<short, short>> pos_noreste = { {0, 0}, {-1, 0}, {-1, 1}, {0, 1}, {-2, 0}, {-2, 1}, {-2, 2}, {-1, 2}, {0, 2}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3}, {-2, 3}, {-1, 3}, {0, 3} };
	vector<pair<short, short>> vector_pos = pos_noreste;
	short posfil, poscol;
	bool swap;

	switch (current_state.brujula)
	{
	case norte: 	posfil = 1; 	poscol = 1;		vector_pos = pos_norte; 	swap = false;	break;
	case noreste: 	posfil = 1; 	poscol = 1; 	vector_pos = pos_noreste; 	swap = false;	break;
	case oeste: 	posfil = 1; 	poscol = -1; 	vector_pos = pos_norte;		swap = true;	break;
	case noroeste: 	posfil = 1; 	poscol = -1; 	vector_pos = pos_noreste; 	swap = true;	break;
	case este: 		posfil = -1; 	poscol = 1;		vector_pos = pos_norte;		swap = true;	break;
	case sureste: 	posfil = -1; 	poscol = 1; 	vector_pos = pos_noreste;	swap = true;	break;
	case sur: 		posfil = -1; 	poscol = -1; 	vector_pos = pos_norte;		swap = false;	break;
	case suroeste: 	posfil = -1; 	poscol = -1; 	vector_pos = pos_noreste;	swap = false;	break;
	}

	for (size_t i = 0; i < 16; i++)
	 {
		short posfil_tmp = swap ? vector_pos[i].second * poscol : vector_pos[i].first * poscol;
		short poscol_tmp = swap ? vector_pos[i].first * posfil : vector_pos[i].second * posfil;

		mapaResultado[current_state.fil + posfil_tmp][current_state.col + poscol_tmp] = terreno[i];
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

	int porcentaje_bateria = static_cast<int>((static_cast<float>(sensores.bateria) / bateria_maxima) * 100);
	if (porcentaje_bateria < 50)
		prioridad_recarga = 6000 - ((bateria_maxima*porcentaje_bateria)/100);
	else
		prioridad_recarga = -1;
	
	// Si encuentro alguna pieza valiosa lo registro
	if (sensores.terreno[0] == 'G' and !ubicado)
	{
		mapa_actual = &mapaResultado;
		mapa_potencial_actual = &mapa_potencial;
		estado = &current_state;
		estado->fil = sensores.posF;
		estado->col = sensores.posC;
		estado->brujula = sensores.sentido;
		ubicado = true;
		trasladar_mapa(mapa_potencial_temporal, mapa_potencial);
		trasladar_mapa(mapa_resultado_temporal, mapaResultado);
	}
	if (sensores.terreno[0] == 'K' and !bikini)
		bikini = true;
	if (sensores.terreno[0] == 'D' and !zapatillas)
		zapatillas = true;
	if (sensores.terreno[0] == 'X')
		recarga = recarga ? recarga - 1 : 5;
	
	// Registro lo que veo
	PonerTerrenoEnMatriz(sensores.terreno, *estado, *mapa_actual, sensores.nivel);
	
	// Decidir nueva acción
	if (prioridad_recarga != -1 && recarga)
	{
		accion = actIDLE;
	}
	
	switch (estado->brujula)
	{
		case norte: frente = break;
		case noreste: break;
		case este: break;
		case sureste: break;
		case sur: break;
		case suroeste: break;
		case oeste: break;
		case noroeste: break;

	int mejor_valor = 0;
	unsigned char radio_vision = 1;
	do
	{
		while (!escanear_perimetro(radio_vision))
		{
			radio_vision++;
		}
		
		generar_mapa_decision(radio_vision, sensores);

		// Elijo la mejor opción
		int penalizacion = (*mapa_potencial_actual)[estado->fil][estado->col];
		//casilla mejor_opcion = make_pair(estado->fil + frente, estado->col + frente);
		mejor_valor = mapa_decision[estado->fil + frente][estado->col + frente];
		accion = actWALK;

		if (mapa_decision[estado->fil + izquierda][estado->col + izquierda] - penalizacion > mejor_valor){
			accion = actTURN_L;
			mejor_valor = mapa_decision[estado->fil + izquierda][estado->col + izquierda] - penalizacion;
		}

		if (mapa_decision[estado->fil + derecha][estado->col + derecha] - penalizacion > mejor_valor){
			accion = actTURN_SR;
			mejor_valor = mapa_decision[estado->fil + derecha][estado->col + derecha] - penalizacion;
		}

		radio_vision++;
		
	} while (mejor_valor == 0);
	
	
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