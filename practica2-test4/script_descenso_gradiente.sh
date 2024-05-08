#!/usr/bin/bash
learning_rate=0.1

max_bat=1
porcentaje_bat=0.48
ciclos_min=0.2
min_vida=0.1

max_iterations=$1
tolerance=0.001
delta=0.05

for (( i=0; i < max_iterations; i++ )); do
    # Calculo la nota sin cambio
    echo "Calculando nota previa"
    nota_previa=$(./test_nivel_4.sh $max_bat $porcentaje_bat $ciclos_min $min_vida)
    echo "Nota previa: $nota_previa"

    # Calculo la nota tras cambio
    porcentaje_bat_new=$(echo "$porcentaje_bat + $delta" | bc -l)
    echo "Ciclos min new: $porcentaje_bat_new"
    echo "Calculando nota nueva"
    nota_nueva=$(./test_nivel_4.sh $max_bat $porcentaje_bat_new $ciclos_min $min_vida)
    echo "Nota nueva: $nota_nueva"

    # Calculo el gradiente
    gradient=$(echo "($nota_nueva - $nota_previa) / $delta" | bc -l)
    echo "Gradient: $gradient"

    # Actualizo los valores de las variables
    porcentaje_bat=$(echo "$porcentaje_bat + ($learning_rate * $gradient)" | bc -l)
    echo "Ciclos min updated: $porcentaje_bat"

    # Compruebo que no haya convergencia
    #echo "Comprobando convergencia"
    #if (( $(echo "$gradient < $tolerance" | bc -l) )); then
    #    echo "Converged alcanzada con iteracion: $i"
    #    break
    #fi

    # Muestro información de la iteración actual
    echo "Iteration $i: Nuevo valor = $porcentaje_bat, Nota = $nota_previa, gradient = $gradient"


    #if (( $(echo "$gradient * $prev_gradient > 0" | bc -l) )); then
    #    echo "Gradient direction unchanged, stopping optimization."
    #    break
    #fi

    # Save current gradient for next iteration
    #prev_gradient=$gradient
done

echo "Optimization finished. Final ciclos_min = $porcentaje_bat"