#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
#include <map>
#include <iomanip>
#include <math.h>
using namespace std;

#include <cstdlib>

std::map<Orientacion, string> orientacion_map = {
  {Orientacion::norte, "Norte"},
  {Orientacion::noreste, "Noreste"},
  {Orientacion::este, "Este"},
  {Orientacion::sureste, "Sureste"},
  {Orientacion::sur, "Sur"},
  {Orientacion::suroeste, "Suroeste"},
  {Orientacion::oeste, "Oeste"},
  {Orientacion::noroeste, "Noroeste"}
};

string orientacion_string(Orientacion orientacion)
{
  auto it = orientacion_map.find(orientacion);
  if (it != orientacion_map.end())
    return it->second;

    return "Invalid Orientacion";
}

void ComportamientoJugador::printea_cola(const queue<casilla>& queue)
{
  std::queue<casilla> temp_queue = queue;

  while (!temp_queue.empty())
  {
    cerr << (int) temp_queue.front().first << ", " << (int) temp_queue.front().second << " | ";
    temp_queue.pop();
  }
  cerr << endl;
}

template <typename T>
void ComportamientoJugador::printea_matriz(const vector<vector<T>>& matriz)
{
/*
    for (const vector<T>& row : matriz)
	{
        for (const T& element : row)
            cerr << element << '\t';
        cerr << endl;
    }
*/
    // Find the maximum width of elements in the matrix
    size_t max_width = 0;
    for (const auto& row : matriz) {
        for (const auto& element : row) {
            size_t width = std::to_string(element).size();
            if (width > max_width) {
                max_width = width;
            }
        }
    }

    // Print matrix with adjusted width
    for (const auto& row : matriz) {
        for (const auto& element : row) {
            std::cout << std::setw(max_width) << element << ' ';
        }
        std::cout << std::endl;
    }
}

void ComportamientoJugador::printear_estado_general()
{

	cerr << "Mapa resultado actual: " << mapa_actual->size() << endl;
	//prienta_matriz(*mapa_actual);
	cerr << endl << endl;
	cerr << "Mapa resultado: " << mapaResultado.size() << endl;
	printea_matriz(mapaResultado);
	cerr << endl << endl;
	cerr << "Mapa resultado falsa: " << mapa_resultado_temporal.size() << endl;
	//printea_matriz(mapa_resultado_temporal);
	cerr << endl << endl << endl << endl;

	cerr << "Mapa potencial actual: " << mapa_potencial_actual->size() << endl;
	//printea_matriz(*mapa_potencial_actual);
	cerr << endl << endl;
	cerr << "Mapa potencial: " << mapa_potencial.size() << endl;
	printea_matriz(mapa_potencial);
	cerr << endl << endl;
	cerr << "Mapa potencial falsa: " << mapa_potencial_temporal.size() << endl;
	//printea_matriz(mapa_potencial_temporal);
	cerr << endl << endl << endl << endl;

	cerr << "Mapa decision: " << mapa_decision.size() << endl;
	printea_matriz(mapa_decision);
	cerr << endl << endl << endl << endl;

	cerr << "Casillas interesantes: " << boolalpha << !casillas_interesantes.empty() << endl;
	printea_cola(casillas_interesantes);
	cerr << "casillas desconocidas: " << boolalpha << !casillas_desconocidas.empty() << endl;
	printea_cola(casillas_desconocidas);
	cerr << endl << endl << endl << endl;

	cerr << "Estado actual: " << endl;
	cerr << "\t¿Ubicado?: " << boolalpha << ubicado << endl;
	cerr << "\t¿Recarga?: " << (int) recarga << endl;
	cerr << "\t¿Bikini?: " << boolalpha << bikini << endl;
	cerr << "\t¿Zapatillas?: " << boolalpha << zapatillas << endl << endl;

	cerr << "\tOrientacion Falsa: " << orientacion_string(fake_state.brujula) << endl;
	cerr << "\tX Falso: " << fake_state.fil << endl;
	cerr << "\tY Falso: " << fake_state.col << endl << endl;

	cerr << "\tOrientación Real: " << orientacion_string(current_state.brujula) << endl;
	cerr << "\tX Real: " << current_state.fil << endl;
	cerr << "\tY Real: " << current_state.col << endl << endl;

	cerr << "\tContenido casilla donde estoy: " << (*mapa_actual)[estado->fil][estado->col] << endl;

	cerr << "\tBateria actual: " << bateria << endl;
	cerr << "\tPorcentaje: " << porcentaje_bateria << endl;
	cerr << endl << endl;
}

// Te devuelve a qué casilla equivale en la orientación actual una posición en la orientación norte/noroeste
casilla ComportamientoJugador::casilla_equivalente(casilla c)
{
	short posfil, poscol;
	bool swap;

	switch (estado->brujula)
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
	//cerr << "Escaneando casilla: " << (int) c.first << ", " << (int) c.second << endl;
	switch ((*mapa_actual)[c.first][c.second])
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
		if (escanear_casilla(make_pair(estado->fil + perimetro, (estado->col + perimetro) - i)))
			encontrada = true;
		// Borde izquierdo
		if (escanear_casilla(make_pair((estado->col + perimetro) - i , estado->col - perimetro)))
			encontrada = true;
		// Borde derecho
		if (escanear_casilla(make_pair((estado->col - perimetro) + i, estado->col + perimetro)))
			encontrada = true;
	}
	
	return encontrada;
}

// Genera el mapa de decisión con un radio inteligente según el mapa de potencial (debe estar relleno)
void ComportamientoJugador::generar_mapa_decision(unsigned char *radio)
{
	// Borro el mapa de decisión, si es que tuviese algo, para rellenarlo de nuevo
    for (auto& row : mapa_decision)
        row.clear();

	int distance;
	casilla c;

	// Buscamos la máxima distancia a la que se encuentra algún elemento interesante para calcular el tamaño de la matriz
	for (size_t i = 0; i < casillas_interesantes.size(); i++)
	{
		c = casillas_interesantes.front();
		distance = c.first > estado->fil ? c.first - estado->fil : estado->fil - c.first;
		*radio = *radio > distance ? *radio : distance;

		distance = c.second > estado->col ? c.second - estado->col : estado->col - c.second;
		*radio = *radio > distance ? *radio : distance;

		casillas_interesantes.push(c);
		casillas_interesantes.pop();
	}
	mapa_decision = vector<vector<int>>(*radio*2+1, vector<int>(*radio*2+1, 0));
	unsigned short offsetfil = estado->fil - *radio;
	unsigned short offestcol = estado->col - *radio;

/*-------------------------------------------------------------------------------------------------------------------------*/
	// Meto las casillas interesantes en el mapa si las ubiese
	for (size_t i = 0; i < casillas_interesantes.size(); i++)
	{
		c = casillas_interesantes.front();

		mapa_decision[c.first - offsetfil][c.second - offestcol] = bateria + (*mapa_potencial_actual)[c.first][c.second];

		casillas_interesantes.push(c);
		casillas_interesantes.pop();
	}
	// Meto las casillas desconocidas en el mapa si las ubiese
	printear_estado_general();
	for (size_t i = 0; i < casillas_desconocidas.size(); i++)
	{
		c = casillas_desconocidas.front();

		cerr << "Real i: " << c.first << "  ,j:" << c.second << endl;
		cerr << "Decision i: " << c.first - offsetfil << "  ,j:" << c.second - offestcol<< endl;
		mapa_decision[c.first - offsetfil][c.second - offestcol] = bateria;

		casillas_desconocidas.push(c);
		casillas_desconocidas.pop();
	}

/*-------------------------------------------------------------------------------------------------------------------------*/
	// Calculamos el efecto que tienen las casillas interesantes sobre el mapa de potencial
	for (size_t i = 0; i < casillas_interesantes.size(); i++)
	{
		c = casillas_interesantes.front();

		calcular_efecto_entorno(make_pair(c.first - offsetfil, c.second - offestcol), offsetfil, offestcol);

		casillas_interesantes.push(c);
		casillas_interesantes.pop();
	}

	// Calculamos el efecto que tienen las casillas desconocidas sobre el mapa de potencial
	// Además aprovecho para vaciar la pila de casillas desconocidas
	while (!casillas_desconocidas.empty())
	{
		c = casillas_desconocidas.front();
		calcular_efecto_entorno(make_pair(c.first - offsetfil, c.second - offestcol), offsetfil, offestcol);
		casillas_desconocidas.pop();
	}
}

// Complemento para la función "generar_mapa_decision"
void ComportamientoJugador::calcular_efecto_entorno(const casilla c, const short offsetfil, const short offsetcol)
{
	//cerr << "Calculando efecto de: " << (int) c.first << ", " << (int) c.second << endl;
	int casilla00 = mapa_decision[c.first][c.second];
	int potencial;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			// Si no son la casilla 00 y si no se salen del mapa, compruebo
			if ((i || j) && (c.first + i) >= 0 && ((c.first + i) < mapa_decision.size()) && (c.second + j) >= 0 && ((c.second + j) < mapa_decision.size()))
			{
				potencial = (*mapa_potencial_actual)[c.first + i + offsetfil][c.second + j + offsetcol];
				if (potencial > 0) potencial = -1;
				// Si el potencial actual no es una barrera (0), veo si es menor que el que podría tener y si es así lo sustituyo
				if (potencial && (mapa_decision[c.first + i][c.second + j] < casilla00 + potencial))
				{
						//cerr << "Potencial actual: " << mapa_decision[c.first + i][c.second + j] << "	Nuevo potencial: " << casilla00 + potencial << endl;
						//cerr << "\tX: " << c.first + i << ", Y:" << c.second + j << endl;
						mapa_decision[c.first + i][c.second + j] = casilla00 + potencial;
						calcular_efecto_entorno(make_pair(c.first + i, c.second + j), offsetfil, offsetcol);
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

Action ComportamientoJugador::decision_optima(const size_t decision)
{
	vector<int> penalizacion_giros_derecha = {7, 0, 1, 6, 1, 2, 5, 4, 3};
	vector<int> penalizacion_giros_izquierda = {2, 0, 6, 1, 1, 3, 3, 2, 4};
	
	if (penalizacion_decision(decision) == 0)
		return actWALK;
	//else if (penalizacion_giros_derecha[decision] <= penalizacion_giros_izquierda[decision])
	//	return actTURN_SR;
	else
		return actTURN_L;
}

int ComportamientoJugador::penalizacion_decision(const size_t decision)
{
	vector<int> penalizacion_giros_derecha = {7, 0, 1, 6, 1, 2, 5, 4, 3};
	vector<int> penalizacion_giros_izquierda = {2, 0, 6, 1, 1, 3, 3, 2, 4};
	
	//if (penalizacion_giros_derecha[decision] <= penalizacion_giros_izquierda[decision])
	//	return penalizacion_giros_derecha[decision];
	//else
		return pow(penalizacion_giros_izquierda[decision], 2);
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
	
/*---------------------------------------------------------------------------------------------*/

	// Reviso batería y decido si necesito recargar o seguir recargando
	bateria = sensores.bateria;
	porcentaje_bateria = (sensores.bateria / static_cast<float>(bateria_maxima)) * 100;
	if (porcentaje_bateria <= 50)
		prioridad_recarga = 6000 * ((100 - porcentaje_bateria) / 100.0f);
	else{
		prioridad_recarga = -1;
		recarga = 0;
	}

/*---------------------------------------------------------------------------------------------*/

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
		if (sensores.nivel != 0)
		{
			trasladar_mapa(mapa_resultado_temporal, mapaResultado);
			trasladar_mapa(mapa_potencial_temporal, mapa_potencial);
		}
	}

	if (sensores.terreno[0] == 'K' and !bikini)
		bikini = true;
	if (sensores.terreno[0] == 'D' and !zapatillas)
		zapatillas = true;
	if (sensores.terreno[0] == 'X')
		recarga = recarga ? recarga - 1 : 5;
	
/*---------------------------------------------------------------------------------------------*/
	// Registro lo que veo
	PonerTerrenoEnMatriz(sensores);
	escanear_casilla(make_pair(estado->fil, estado->col));
	
/*---------------------------------------------------------------------------------------------*/
	// Decidir nueva acción
	vector<vector<int>> mapa_decision_definitivo = vector<vector<int>>(3, vector<int>(3, 0));

	if (recarga)
		accion = actIDLE;
	else
	{
		unsigned char radio_vision = 0;
		unsigned char radio_vision_inicial = 3;
		int mejor_decision = 0;
		int coste_casilla_actual = (*mapa_potencial_actual)[estado->fil][estado->col];

		do
		{
			bool casilla_interesante_encontrada = false;
			do {
				radio_vision++;
				casilla_interesante_encontrada = escanear_perimetro(radio_vision) || casilla_interesante_encontrada;
			}
			while (radio_vision < radio_vision_inicial || !casilla_interesante_encontrada);

			generar_mapa_decision(&radio_vision);
			unsigned short offsetfil = estado->fil - radio_vision;
			unsigned short offestcol = estado->col - radio_vision;

			/********************************************************************/
			// Elijo la mejor opción
			int penalizacion = 0;
			casilla equiv;
			size_t opcion = 0;
			for (int i = -1; i < 2; i++)
			{
				for (int j = -1; j < 2; j++)
				{
					if (i || j)
					{
						equiv = casilla_equivalente(make_pair(i, j));
						penalizacion = penalizacion_decision(opcion);
						//cerr << "PENALIZACION i: " << i << " ,j: " << j << ", penalizacion: " << penalizacion << endl;
						mapa_decision_definitivo[i+1][j+1] = mapa_decision[equiv.first - offsetfil][equiv.second - offestcol] + penalizacion*coste_casilla_actual;
						if (mapa_decision[equiv.first - offsetfil][equiv.second - offestcol] + penalizacion*coste_casilla_actual > mejor_decision)
						{
							accion = decision_optima(opcion);
							mejor_decision = mapa_decision[equiv.first - offsetfil][equiv.second - offestcol] + penalizacion*coste_casilla_actual;
						}
					}
					opcion++;
				}
			}

		} while (mejor_decision == 0);
	}
	
	last_action = accion;

/*---------------------------------------------------------------------------------------------*/
	// Mostrar el valor de los sensores
	// printear_estado_general();

	//cerr << "MATRIZ DECISION DEFINITVA: " << endl;
	//printea_matriz(mapa_decision_definitivo);

	/*
	cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC << endl;
	cout << "Orientacion: " << orientacion_string(estado->brujula) << endl;
	cout << "Terreno: ";
	for (int i=0; i<sensores.terreno.size(); i++)
		cout << sensores.terreno[i];
	cout << "  Agentes: ";
	for (int i=0; i<sensores.agentes.size(); i++)
		cout << sensores.agentes[i];
	cout << "\nColision: " << sensores.colision;
	cout << "  Reset: " << sensores.reset;
	cout << "  Vida: " << sensores.vida << endl<< endl;
	*/
	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}