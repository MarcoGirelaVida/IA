#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
#include <map>
#include <iomanip>
#include <math.h>

using namespace std;

#include <cstdlib>
#include <unistd.h>

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

map<int, pair<char,char>> casilla_opcion = {
	{0, make_pair(-1, -1)},
	{1, make_pair(-1, 0)},
	{2, make_pair(-1, 1)},
	{3, make_pair(0, -1)},
	{4, make_pair(0, 0)},
	{5, make_pair(0, 1)},
	{6, make_pair(1, -1)},
	{7, make_pair(1, 0)},
	{8, make_pair(1, 1)}
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
void ComportamientoJugador::printea_matriz(const vector<vector<T>>& matriz, int radio)
{
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

    // Printeo la matriz según el radio
	if (!radio)
	{
		for (size_t i = 0; i < matriz.size(); i++)
		{
			for (size_t j = 0; j < matriz.size(); j++)
			{
				if (i == matriz.size() / 2 && j == matriz.size() / 2)
					cerr << std::setw(max_width) << "J" << " ";
				else
				cerr << std::setw(max_width) << matriz[i][j] << " ";
			}
			cerr << endl;
		}
		
	}
	// Si el usuario solicita un radio concreto, solo printeo las casillas de la matriz que rodean al usuario en ese radio
	else
	{
		int fil = estado->fil - radio;
		int col = estado->col - radio;
		for (int i = 0; i < radio * 2 + 1; i++)
		{
			for (int j = 0; j < radio * 2 + 1; j++)
			{
				if (esta_dentro_de_mapa(make_pair(fil + i, col + j), matriz))
				{
					if (fil + i == estado->fil && col + j == estado->col)
						cerr << fixed << setprecision(2) << std::setw(max_width) << "J" << " ";
					else
						cerr << fixed << setprecision(2) << std::setw(max_width) << matriz[fil + i][col + j] << " ";
				}
			}
			cerr << endl;
		}
	}
}

void ComportamientoJugador::printea_set(set<casilla>& casillas_int)
{
  for (const auto& element : casillas_int)
  {
	if (esta_dentro_de_mapa(element, *mapa_actual))
		cerr << (int) element.first << ", " << (int) element.second << " ( " << (*mapa_actual)[element.first][element.second] << " ) " << " | ";
	else
		casillas_int.erase(element);
  }
	cerr << endl;
}

void ComportamientoJugador::printear_estado_general(int opcion)
{

	if (opcion > 1)
	{
		if (opcion == 3)
		{
		cerr << "Mapa resultado actual: " << ((mapa_actual->size() == mapaResultado.size()) ? "Mapa Resultado Real" : "Mapa Resultado Falso") << endl;
		printea_matriz(*mapa_actual, mapa_decision.size() / 2);
		//cerr << "Mapa resultado: " << mapaResultado.size() << endl;
		//printea_matriz(mapaResultado);
		//cerr << endl << endl;
		//cerr << "Mapa resultado falsa: " << mapa_resultado_temporal.size() << endl;
		//printea_matriz(mapa_resultado_temporal);
		//cerr << endl << endl;

		cerr << "Mapa potencial actual: " << ((mapa_potencial_actual->size() == mapa_potencial.size()) ? "Mapa Potencial Real" : "Mapa Potencial Falso") << endl;
		printea_matriz(*mapa_potencial_actual, mapa_decision.size() / 2);
		//cerr << endl << endl;
		//cerr << "Mapa potencial: " << mapa_potencial.size() << endl;
		//printea_matriz(mapa_potencial);
		//cerr << endl << endl;
		//cerr << "Mapa potencial falsa: " << mapa_potencial_temporal.size() << endl;
		//printea_matriz(mapa_potencial_temporal);
		//cerr << endl << endl;
		}

		cerr << "Mapa decision: " << mapa_decision.size() << endl;
		printea_matriz(mapa_decision);
		cerr << endl << endl;

		cerr << "Mapa decision ponderado: " << mapa_decision_ponderado.size() << endl;
		printea_matriz(mapa_decision_ponderado);
		cerr << endl << endl;
	}

	cerr << "Casillas interesantes: " << boolalpha << !casillas_interesantes->empty() << endl;
	printea_set(*casillas_interesantes);
	cerr << "casillas desconocidas: " << boolalpha << !casillas_desconocidas.empty() << endl;
	printea_cola(casillas_desconocidas);
	cerr << endl << endl;

	cerr << "Estado actual: " << endl;
	cerr << "\t¿Ubicado?: " << boolalpha << ubicado << endl;
	cerr << "\t¿Recargando?: " << boolalpha << recargando << endl;
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

	cerr << "CACHE DE ACCIONES: " << endl;
	stack<Action> cache_acciones_aux = cache_acciones;
	while (!cache_acciones_aux.empty())
	{
		cerr << cache_acciones_aux.top() << " | ";
		cache_acciones_aux.pop();
	}
	cerr << endl << endl;
}

template <typename T>
bool ComportamientoJugador::esta_dentro_de_mapa(const casilla c, const vector<vector<T>> &mapa)
{
	return c.first >= 0 && c.first < mapa.size() && c.second >= 0 && c.second < mapa.size();
}


bool ComportamientoJugador::escanear_casilla(const casilla c)
{
	char tmp;
	if ((casillas_agentes.find(c) != casillas_agentes.end()))
	{
		tmp = (*mapa_actual)[c.first][c.second];
		(*mapa_actual)[c.first][c.second] = casillas_agentes[c];
	}

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
	case 'G': (*mapa_potencial_actual)[c.first][c.second] = ubicado ? -1 : 2000; if (!ubicado) casillas_interesantes->insert(c); return true;
	// Bikini
	case 'K': (*mapa_potencial_actual)[c.first][c.second] = bikini ? -1 : 1000; if(!bikini) casillas_interesantes->insert(c); return true;
	// Zapatillas
	case 'D': (*mapa_potencial_actual)[c.first][c.second] = zapatillas ? -1 : 1000; if(!zapatillas) casillas_interesantes->insert(c); return true;
	// Recarga
	case 'X': (*mapa_potencial_actual)[c.first][c.second] = prioridad_recarga; recargador_encontrado = true; recargador = c; return true;
	// Desconocida
 	case '?': (*mapa_potencial_actual)[c.first][c.second] = 1; casillas_desconocidas.push(c); return true;

	// Jugador
	case 'a': (*mapa_potencial_actual)[c.first][c.second] = 0; (*mapa_actual)[c.first][c.second] = tmp; casillas_agentes.erase(c); return false;
	// Lobos
	case 'l': (*mapa_potencial_actual)[c.first][c.second] = 0; (*mapa_actual)[c.first][c.second] = tmp; casillas_agentes.erase(c); return false;	
	}
}

bool ComportamientoJugador::es_casilla_interesante(const casilla c)
{
	return (*mapa_potencial_actual)[c.first][c.second] > 0;
}

// Escanea las casillas del perímetro p y guarda los resultados en el mapa de potencial y las colas correspondientes
bool ComportamientoJugador::escanear_perimetro(const unsigned char perimetro)
{
	//cerr << "Estoy en escanear perimetro con perimetro: " << (int) perimetro << endl;
	bool encontrada = false;
	casilla c;
	for (size_t i = 0; i < (perimetro * 2); i++)
	{
		// Borde superior
		//cerr << "Escaneo borde superior, i: " << estado->fil - perimetro << " j: " << (estado->col - perimetro) + i << endl;
		c = make_pair(estado->fil - perimetro, (estado->col - perimetro) + i);
		if (esta_dentro_de_mapa(c, *mapa_actual) && escanear_casilla(c))
			encontrada = true;

		// Borde inferior
		//cerr << "Escaneo borde inferior, i: " << estado->fil + perimetro << " j: " << (estado->col + perimetro) - i << endl;
		c = make_pair(estado->fil + perimetro, (estado->col + perimetro) - i);
		if (esta_dentro_de_mapa(c, *mapa_actual) && escanear_casilla(c))
			encontrada = true;

		// Borde izquierdo
		//cerr << "Escaneo borde izquierdo, i: " << (estado->fil + perimetro) - i << " j: " << estado->col - perimetro << endl;
		c = make_pair((estado->fil + perimetro) - i, estado->col - perimetro);
		if (esta_dentro_de_mapa(c, *mapa_actual) && escanear_casilla(c))
			encontrada = true;

		// Borde derecho
		//cerr << "Escaneo borde derecho, i: " << (estado->fil - perimetro) + i << " j: " << estado->col + perimetro << endl;
		c = make_pair((estado->fil - perimetro) + i, estado->col + perimetro);
		if (esta_dentro_de_mapa(c, *mapa_actual) && escanear_casilla(c))
			encontrada = true;
	}

	return encontrada;
}

// Genera el mapa de decisión con un radio inteligente según el mapa de potencial (debe estar relleno)
void ComportamientoJugador::generar_mapa_decision(unsigned char *radio)
{
	//cerr << "Estoy en generar mapa decision con radio: " << (int) *radio << endl;

	int distance;
	int radio_solicitado = *radio;
	//cerr << "Se van a buscar el tamaño minimo" << endl;
	// Buscamos la máxima distancia a la que se encuentra algún elemento interesante para calcular el tamaño de la matriz
	for (const auto& c : *casillas_interesantes)
	{
		distance = c.first > estado->fil ? c.first - estado->fil : estado->fil - c.first;
		*radio = *radio > distance ? *radio : distance;

		distance = c.second > estado->col ? c.second - estado->col : estado->col - c.second;
		*radio = *radio > distance ? *radio : distance;
	}
	if (*radio > 15 && *radio > radio_solicitado)
		*radio = radio_solicitado;

	mapa_decision = vector<vector<int>>(*radio*2+1, vector<int>(*radio*2+1, 0));
	mapa_decision_ponderado = vector<vector<float>>(*radio*2+1, vector<float>(*radio*2+1, 0));

	if (*radio > radio_solicitado)
		for (size_t i = radio_solicitado + 1; i <= *radio; i++)
			escanear_perimetro(i);
		

	short offsetfil = estado->fil - *radio;
	short offsetcol = estado->col - *radio;
	//cerr << "Radio final: " << (int) *radio << endl;
	//cerr << "Offset fil: " << offsetfil << "	Offset col: " << offsetcol << endl;
/*-------------------------------------------------------------------------------------------------------------------------*/
	// Meto las casillas interesantes en el mapa si las ubiese
	//cerr << "Se van a meter las casillas interesantes" << endl;
	set<casilla> lista_negra;
	for (const auto& c : *casillas_interesantes){
		if (esta_dentro_de_mapa(make_pair(c.first - offsetfil, c.second - offsetcol), mapa_decision))
		{
			escanear_casilla(c);
			if (es_casilla_interesante(c))
				mapa_decision[c.first - offsetfil][c.second - offsetcol] = bateria + (*mapa_potencial_actual)[c.first][c.second];
			else
				lista_negra.insert(c);
		}
	}
	for (const auto& c : lista_negra)
		casillas_interesantes->erase(c);
	lista_negra.clear();
	// Meto las casillas desconocidas en el mapa si las ubiese
	//cerr << "Se van a meter las casillas desconocidas" << endl;
	size_t casillas_desconocidas_size = casillas_desconocidas.size();
	for (size_t i = 0; i < casillas_desconocidas_size; i++)
	{
		const auto c = casillas_desconocidas.front();
		if (esta_dentro_de_mapa(make_pair(c.first - offsetfil, c.second - offsetcol), mapa_decision))
		{
			//cerr << "Se ha metido la casilla desconocida: " << (int) c.first << ", " << (int) c.second << " en el mapa de decisión" << endl;
			mapa_decision[c.first - offsetfil][c.second - offsetcol] = bateria;
		}
		casillas_desconocidas.pop();
		casillas_desconocidas.push(c);
	}
	//printea_matriz(mapa_decision);
	//cerr << "Se han metido las casillas interesantes y desconocidas en el mapa de decisión" << endl;
/*-------------------------------------------------------------------------------------------------------------------------*/
	// Calculamos el efecto que tienen las casillas interesantes sobre el mapa de potencial
	//cerr << "Se va a calcular el efecto de las casillas interesantes" << endl;
	for (const auto& c : *casillas_interesantes)
	{
		if (esta_dentro_de_mapa(make_pair(c.first - offsetfil, c.second - offsetcol), mapa_decision))
		{
			threads.emplace_back(std::thread(&ComportamientoJugador::calcular_efecto_entorno, this, std::make_pair(c.first - offsetfil, c.second - offsetcol), offsetfil, offsetcol));
		}
	}

	for (auto& thread : threads) {
        thread.join();
    }
	// Calculamos el efecto que tienen las casillas desconocidas sobre el mapa de potencial
	// Además aprovecho para vaciar la pila de casillas desconocidas
	//cerr << "Se va a calcular el efecto de las casillas desconocidas" << endl;
	for (size_t i = 0; i < casillas_desconocidas_size; i++)
	{
		const auto c = casillas_desconocidas.front();
		if (esta_dentro_de_mapa(make_pair(c.first - offsetfil, c.second - offsetcol), mapa_decision))
		{
			threads.emplace_back(std::thread(&ComportamientoJugador::calcular_efecto_entorno, this, std::make_pair(c.first - offsetfil, c.second - offsetcol), offsetfil, offsetcol));
		}
		casillas_desconocidas.pop();
		casillas_desconocidas.push(c);
	}

	for (auto& thread : threads) {
        thread.join();
    }
/*-------------------------------------------------------------------------------------------------------------------------*/
// Copio el mapa de decisión en el mapa de decisión ponderado
	//cerr << "Se va a copiar el mapa de decisión en el mapa de decisión ponderado" << endl;
	for (size_t i = 0; i < mapa_decision.size(); i++)
		for (size_t j = 0; j < mapa_decision.size(); j++)
			mapa_decision_ponderado[i][j] = static_cast<float>(mapa_decision[i][j]);
}

// Imporatante, siempre se debe generar antes el mapa de decision
void ComportamientoJugador::generar_mapa_decision_ponderado()
{
	//cerr << "Estoy en generar mapa decision ponderado" << endl;
	int radio = mapa_decision.size() / 2;
	short offsetfil = estado->fil - radio;
	short offsetcol = estado->col - radio;

	for (size_t i = 0; i < mapa_decision_ponderado.size(); i++)
	{
		for (size_t j = 0; j < mapa_decision_ponderado.size(); j++)
		{
			if (mapa_decision[i][j] &&  !es_casilla_interesante(make_pair(i + offsetfil, j + offsetcol)))
			{
				//float media = media_entorno(make_pair(i, j));
				mapa_decision_ponderado[i][j] = media_entorno(make_pair(i, j));
			}
		}
	}
}

float ComportamientoJugador::media_entorno(const casilla c)
{
	float media = 0;
	int contador = 0;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (esta_dentro_de_mapa(make_pair(c.first + i, c.second + j), mapa_decision) && mapa_decision[c.first + i][c.second + j] >= mapa_decision[c.first][c.second])
			{
				media += static_cast<float>(mapa_decision[c.first + i][c.second + j]);
				contador++;
			}
		}
	}
	return (media / contador);
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
			if ((i || j) && esta_dentro_de_mapa(make_pair(c.first + i, c.second + j), mapa_decision))
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

// Traslada los mapas y borra el base
template <typename T>
void ComportamientoJugador::trasladar_mapa(vector< vector<T>>& base, vector< vector<T>>& objetivo, bool mapa_resultado)
{
	int giros = 4 - ((((fake_state.brujula - current_state.brujula) + 8 ) % 8) / 2);
	giros = giros == 4 ? 0 : giros;
	vector<vector<T>> matriz_girada = vector<vector<T>>(base.size(), vector<T>(base.size(), 0));

	for (size_t i = 0; i < giros; i++)
	{
		// Roto la matriz base y la copio en la matriz base
		rotar_mapa_derecha(base, matriz_girada);

		// Hago copia profunda de la matriz girada en la matriz base
		for (size_t i = 0; i < base.size(); i++)
			for (size_t j = 0; j < base.size(); j++)
				base[i][j] = matriz_girada[i][j];
		
		//Roto la posición 0, 0
		int aux = fake_state.fil;
		fake_state.fil = fake_state.col;
		fake_state.col = base.size() - 1 - aux;

		set<casilla> lista_negra;
		//Roto casillas interesantes y desconocidas
		for (const auto& c : casillas_interesantes_falsas)
		{
			lista_negra.insert(c);
			casilla rotada = c;
			int aux = rotada.first;
			rotada.first = rotada.second;
			rotada.second = base.size() - 1 - aux;
			casillas_interesantes_falsas.insert(rotada);
		}
		for (const auto& c : lista_negra)
			casillas_interesantes_falsas.erase(c);

		for (size_t i = 0; i < casillas_desconocidas.size(); i++)
		{
			casilla rotada = casillas_desconocidas.front();
			int aux = rotada.first;
			rotada.first = rotada.second;
			rotada.second = base.size() - 1 - aux;

			casillas_desconocidas.push(rotada);
			casillas_desconocidas.pop();
		}
	}
	// Hago copia profunda de casillas interesantes falsas en casillas interesantes reales
	
	
	unsigned char casilla_inutil = mapa_resultado ? '?' : 1;
	int ni = fake_state.fil - current_state.fil;
	int nj = fake_state.col - current_state.col;
	int fi = fake_state.fil + (objetivo.size()-current_state.fil);
	int fj = fake_state.col + (objetivo.size()-current_state.col);

	for (int i = ni; i < fi; i++){
		for (int j = nj; j < fj; j++){
			if (objetivo[i-ni][j-nj] == casilla_inutil){
				objetivo[i-ni][j-nj] = base[i][j];
			}
		}
	}
	// Reajusto al tamaño las casillas interesantes y desconociads
	for (const auto& c : casillas_interesantes_falsas)
	{
		casilla reajustada = make_pair(c.first - ni, c.second - nj);
		casillas_interesantes_reales.insert(reajustada);
	}
	casillas_interesantes_falsas.clear();

	for (size_t i = 0; i < casillas_desconocidas.size(); i++)
	{
		casilla reajustada = casillas_desconocidas.front();
		casillas_desconocidas.push(make_pair(reajustada.first - ni, reajustada.second - nj));
		casillas_desconocidas.pop();
	}

	// Borro el mapa base
	for (auto& row : base)
		row.clear();
}

casilla ComportamientoJugador::rotar_casilla(const casilla base, const size_t size)
{
	return make_pair(base.second, size-1-base.first);
}
template <typename T>
void ComportamientoJugador::rotar_mapa_derecha(vector<vector<T>> &mapaOriginal, vector<vector<T>> &mapaGirado)
{
	int n = mapaOriginal.size();
	for (int i=0; i < n; i++)
		for (int j=0; j < n; j++)
			mapaGirado[j][n-1-i] = mapaOriginal[i][j];
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

void ComportamientoJugador::PonerTerrenoEnMatriz(Sensores sensor)
{
	//cerr << "Estoy en poner terreno en matriz" << endl;
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
		if (sensor.terreno[i] != '?'){
			casilla c = casilla_equivalente(vector_pos[i]);
			(*mapa_actual)[c.first][c.second] = sensor.terreno[i];
			if (sensor.agentes[i] != '_' && sensor.agentes[i] != 'j')
				casillas_agentes.insert(make_pair(c, sensor.agentes[i]));
		}
	 }
}

// Te devuelve a qué casilla equivale en la orientación actual una posición en la orientación norte/noroeste
casilla ComportamientoJugador::casilla_equivalente(pair<char, char> c)
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

Action ComportamientoJugador::decision_optima(size_t decision)
{
	//vector<casilla> posiciones = {make_pair(-1, -1), make_pair(-1, 0), make_pair(-1, 1), make_pair(0, -1), make_pair(0, 0), make_pair(0, 1), make_pair(1, -1), make_pair(1, 0), make_pair(1, 1)};
	vector<int> penalizacion_giros_derecha = {7, 0, 1, 6, 1, 2, 5, 4, 3};
	vector<int> penalizacion_giros_izquierda = {2, 0, 5, 1, 1, 3, 3, 2, 4};

	if (estado->brujula == noreste || estado->brujula == sureste || estado->brujula == suroeste || estado->brujula == noroeste)
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
	case 0: cache_acciones.push(actTURN_SR); return actTURN_L;
	case 1: return actWALK;
	case 2: return actTURN_SR;
	case 3: return actTURN_L;
	case 4: return actIDLE;
	case 5:	cache_acciones.push(actTURN_SR); return actTURN_SR;
	case 6: cache_acciones.push(actTURN_SR); cache_acciones.push(actTURN_L); return actTURN_L;
	case 7: cache_acciones.push(actTURN_L); return actTURN_L;
	case 8: cache_acciones.push(actTURN_SR); cache_acciones.push(actTURN_SR); return actTURN_SR;
	}
}

float ComportamientoJugador::penalizacion_decision(size_t decision)
{
	vector<int> penalizacion_giros_derecha = {7, 0, 1, 6, 1, 2, 5, 4, 3};
	vector<int> penalizacion_giros_izquierda = {2, 0, 6, 1, 1, 3, 3, 2, 4};
	
	if (estado->brujula == noreste || estado->brujula == sureste || estado->brujula == suroeste || estado->brujula == noroeste)
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
	
	if (penalizacion_giros_derecha[decision] <= penalizacion_giros_izquierda[decision])
		//return pow(penalizacion_giros_derecha[decision], 2);
		return penalizacion_giros_derecha[decision] * 0.75;
	else
		//return pow(penalizacion_giros_izquierda[decision], 2);
		return penalizacion_giros_izquierda[decision] * 0.75;
}

void ComportamientoJugador::actualizar_ultima_accion(Sensores sensores)
{
	// Actualización según última acción
	//cerr << "Se ha colisionado?: " << sensores.colision << endl;
	if (!sensores.colision)
	{
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
	}
	
/*---------------------------------------------------------------------------------------------*/
	// Miro si me he muerto
	if (sensores.reset == true )
	{
		//cerr << "HE MUERTO" << endl;
		// Vacio la cache de acciones
		while (!cache_acciones.empty())
			cache_acciones.pop();
		
		ubicado = false;
		bikini = false;
		zapatillas = false;
		recargador_encontrado = false;
		recargando = false;
		fake_state.fil = (mapa_size*3) / 2;
		fake_state.col = (mapa_size*3) / 2;
		fake_state.brujula = norte;
		estado = &fake_state;

		mapa_potencial_temporal = vector<vector<int>>(mapa_size*3, vector<int>(mapa_size*3, 0));
		mapa_potencial_actual = &mapa_potencial_temporal;

		mapa_resultado_temporal = vector<vector<unsigned char>>(mapa_size*3, vector<unsigned char>(mapa_size*3, '?'));
		mapa_actual = &mapa_resultado_temporal;

		// Vacio casillas interesasntes falsas
		casillas_interesantes_falsas.clear();
		casillas_interesantes = &casillas_interesantes_falsas;
	}


/*---------------------------------------------------------------------------------------------*/
	// Actulizo las variables si encuentro pieza valiosa
	if (sensores.posF != -1 and !ubicado)
	{
		mapa_actual = &mapaResultado;
		mapa_potencial_actual = &mapa_potencial;
		casillas_interesantes = &casillas_interesantes_reales;
		estado = &current_state;
		estado->fil = sensores.posF;
		estado->col = sensores.posC;
		estado->brujula = sensores.sentido;
		ubicado = true;
		if (sensores.nivel != 0)
		{
			//cerr << "Se va a trasladar el mapa" << endl;
			trasladar_mapa(mapa_resultado_temporal, mapaResultado, true);
			trasladar_mapa(mapa_potencial_temporal, mapa_potencial, false);
		}
	}

	// Saco la casilla donde me encuentro si es que fuese una casilla interesante
	casillas_interesantes->erase(make_pair(estado->fil, estado->col));

	if (sensores.terreno[0] == 'K' and !bikini)
	{
		bikini = true;
	}
	if (sensores.terreno[0] == 'D' and !zapatillas)
		zapatillas = true;

/*---------------------------------------------------------------------------------------------*/
	// Reviso batería y decido si necesito recargar o seguir recargando
	bateria = sensores.bateria;
	porcentaje_bateria = (sensores.bateria / static_cast<float>(bateria_maxima)) * 100;
	if (porcentaje_bateria <= 75 && ciclos_desde_ultima_recarga >= 250 && sensores.vida >= 250){
		prioridad_recarga = 10000 * ((100 - porcentaje_bateria) / 100.0f);
		if (recargador_encontrado){
			casillas_interesantes->insert(recargador);
		}
	}
	else{
		prioridad_recarga = -1;
		casillas_interesantes->erase(recargador);
		ciclos_desde_ultima_recarga++;
	}
	// Si estoy en una casilla de recarga, descuento la batería
	if (sensores.terreno[0] == 'X'){
		if (prioridad_recarga != -1)
			recargando = true;
		else{
			recargando = false;
			ciclos_desde_ultima_recarga = 0;
		}
	}
}

Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;
	actualizar_ultima_accion(sensores);

/*---------------------------------------------------------------------------------------------*/
	// Registro lo que veo
	//cerr << "Se va a poner el terreno en la matriz" << endl;
	PonerTerrenoEnMatriz(sensores);
	escanear_casilla(make_pair(estado->fil, estado->col));
	
/*---------------------------------------------------------------------------------------------*/
	// Decidir nueva acción
	unsigned char mejor_opcion = 0;
	if (!cache_acciones.empty() && !sensores.colision)
	{
		//cerr << "Vaciando cache de acciones" << endl;
		accion = cache_acciones.top();
		cache_acciones.pop();
	}
	else if (!recargando)
	{
		mapa_decision_definitivo = vector<vector<float>>(3, vector<float>(3, 0));

		casilla equiv;
		unsigned char opcion;
		unsigned char radio_vision = 0;
		unsigned char radio_vision_inicial = 3;
		int coste_casilla_actual = (*mapa_potencial_actual)[estado->fil][estado->col];

		if (estado->brujula == noreste || estado->brujula == sureste || estado->brujula == suroeste || estado->brujula == noroeste)
			prioridad_opciones = &prioridad_opciones_diagonal;
		else
			prioridad_opciones = &prioridad_opciones_vertical;


		do
		{
			mejor_decision = 0;

			bool casilla_interesante_encontrada = false;
			do {
				radio_vision++;
				//cerr << "Se va a escanear el perimetro con radio: " << (int) radio_vision << endl;
				casilla_interesante_encontrada = escanear_perimetro(radio_vision) || casilla_interesante_encontrada;
			}
			while ((radio_vision < radio_vision_inicial || !casilla_interesante_encontrada) && (radio_vision <= mapaResultado.size()));

			//cerr << "Se va a generar el mapa de decisión con radio: " << (int) radio_vision << endl;
			generar_mapa_decision(&radio_vision);
			short offsetfil = estado->fil - radio_vision;
			short offsetcol = estado->col - radio_vision;

			/********************************************************************/
			// Elijo la mejor opción
			
			// Meto las penalizaciones
			generar_mapa_decision_ponderado();

			//cerr << "Se van a meter las penalizaciones" << endl;
			for (size_t i = 0; i < 8; i++)
			{
				int opcion = (*prioridad_opciones)[i];
				casilla c = casilla_equivalente(casilla_opcion[opcion]);
				if (!es_casilla_interesante(c) && mapa_decision_ponderado[c.first - offsetfil][c.second - offsetcol])
				{
					int penalizacion = penalizacion_decision(opcion);
					mapa_decision_ponderado[c.first - offsetfil][c.second - offsetcol] += penalizacion*coste_casilla_actual;
				}
			}

			//cerr << "Se va a buscar la mejor opcion" << endl;
			// Elijo la mejor opción
			for (size_t i = 0; i < 8; i++)
			{
				int opcion = (*prioridad_opciones)[i];
				casilla c = casilla_equivalente(casilla_opcion[opcion]);
				if (opcion != 4 && mejor_decision < mapa_decision_ponderado[c.first - offsetfil][c.second - offsetcol])
				{
					mejor_decision = mapa_decision_ponderado[c.first - offsetfil][c.second - offsetcol];
					// Vacio la cache de acciones si tuviese algo
					while (!cache_acciones.empty())
						cache_acciones.pop();

					accion = decision_optima(opcion);
					mejor_opcion = opcion;
				}
				mapa_decision_definitivo[casilla_opcion[opcion].first + 1][casilla_opcion[opcion].second + 1] = mapa_decision_ponderado[c.first - offsetfil][c.second - offsetcol];
			}
			
			//printear_estado_general();
		} while (mejor_decision == 0 && radio_vision <= (mapaResultado.size() / 2) - 3);

		if (mejor_decision == 0)
			accion = actWALK;
	}
	
	last_action = accion;

/*---------------------------------------------------------------------------------------------*/
	// Mostrar el valor de los sensores
	//printear_estado_general(3);
	//cerr << "MATRIZ DECISION DEFINITVA: " << endl;
	//printea_matriz(mapa_decision_definitivo);
	//cerr << "DECISION FINAL: "
	//	 << (accion == actIDLE ? "IDLE" : accion == actTURN_L ? "TURN_L" : accion == actTURN_SR ? "TURN_R" : "WALK") << endl;
	//cerr << "Valor: " << fixed << setprecision(2) << mejor_decision << endl;
	//cerr << "Opcion: " << (int) mejor_opcion << endl;
	//cerr << "Se ha colisionado?: " << sensores.colision << endl;

	//cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC << endl;
	//cout << "Orientacion: " << orientacion_string(estado->brujula) << endl;
	//cout << "Terreno: ";
	//for (int i=0; i<sensores.terreno.size(); i++)
	//	cout << sensores.terreno[i];
	//cout << "  Agentes: ";
	//for (int i=0; i<sensores.agentes.size(); i++)
	//	cout << sensores.agentes[i];
	//cout << "\nColision: " << sensores.colision;
	//cout << "  Reset: " << sensores.reset;
	//cout << "  Vida: " << sensores.vida << endl<< endl;
	//cerr << "----------------------------------------------------------------------------------------------------------------" << endl;
	// Desalojo memoria de las matrices restantes
	for (auto& row : mapa_decision)
		row.clear();
	for (auto& row : mapa_decision_ponderado)
		row.clear();
	for (auto& row : mapa_decision_definitivo)
		row.clear();
	while (!casillas_desconocidas.empty())
		casillas_desconocidas.pop();
	
	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}