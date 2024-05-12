#include <vector>
#include <limits>
using namespace std;

enum movimiento{seis, dos};
vector<movimiento> movimientos = {seis, dos};

struct estado
{

    estado generar_hijo(const movimiento mov) const;
    int heursistica() const;


    bool operator==(const estado b) const
    {
        return;
    }
    bool operator!=(const estado b) const
    {
        return !(*this == b);
    }
    bool es_solucion() const {return;} 
    bool es_hijo_valido(const estado &hijo) const
    {return *this != hijo;}
};


estado estado::generar_hijo(const movimiento mov) const
{
    estado hijo = *this;

    switch (mov)
    {
    case seis:      break;
    default:    break;
    }

    return hijo;
}

int min_max(estado estado_actual, int profundidad, bool turno_jugador, int mejor_nota_actual, int peor_nota_actual)
{
    if (profundidad == 0 or estado_actual.es_solucion())
        return estado_actual.heursistica();
    
    int nota_estado = turno_jugador ? numeric_limits<int>::min() : numeric_limits<int>::max();
    for (const auto& mov : movimientos)
    {
        estado hijo = estado_actual.generar_hijo(mov);

        if (estado_actual.es_hijo_valido(hijo))
        {
            int nota_estado_hijo = min_max(hijo, profundidad - 1, !turno_jugador, mejor_nota_actual, peor_nota_actual);
            nota_estado = turno_jugador ? max(nota_estado, nota_estado_hijo) : min(nota_estado, nota_estado_hijo);
            
            if (turno_jugador)
                mejor_nota_actual = max(mejor_nota_actual, nota_estado);
            else
                peor_nota_actual = min(peor_nota_actual, nota_estado);

            if (peor_nota_actual <= mejor_nota_actual)
                break;
        }
    }

    return nota_estado;
}