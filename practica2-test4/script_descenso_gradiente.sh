#!/usr/bin/bash
#max_bat=1
#porcentaje_bat=0.48538461538461538462
#ciclos_min=0.14615384615384615386
#min_vida=0.17307692307692307695

max_iterations=$1
learning_rate=0.5
x1=0.75
x2=0.2
x3=1
x4=1
#tolerance=0.001
delta=0.1

for (( i=0; i < max_iterations; i++ )); do
    # Calculo la nota sin cambio
    fx=$(./test_nivel_4_paralel.sh $x1 $x2 $x3 $x4)
    echo -e "Nota previa: $fx \n"

    # Calculo la nota tras cambio
    x1_new=$(echo "$x1+ $delta" | bc -l)
    #x2_new=$(echo "$x2+ $delta" | bc -l)
    #x3_new=$(echo "$x3+ $delta" | bc -l)
    #x4_new=$(echo "$x4+ $delta" | bc -l)
    echo "Variable x1 + delta new: $x1_new"
    #echo "Variable x2 + delta new: $x2_new"
    #echo "Variable x3 + delta new: $x3_new"
    #echo -e "Variable x4 + delta new: $x4_new\n"
    ./test_nivel_4_paralel.sh $x1_new $x2 $x3 $x4 > tmp_x1.txt &
    #./test_nivel_4_paralel.sh $x1 $x2_new $x3 $x4 > tmp_x2.txt &
    #./test_nivel_4_paralel.sh $x1 $x2 $x3_new $x4 > tmp_x3.txt &
    #./test_nivel_4_paralel.sh $x1 $x2 $x3 $x4_new > tmp_x4.txt &
    #wait
    fx_x1_new=$(cat tmp_x1.txt)
    #fx_x2_new=$(cat tmp_x2.txt)
    #fx_x3_new=$(cat tmp_x3.txt)
    #fx_x4_new=$(cat tmp_x4.txt)
    #rm tmp_x1.txt #tmp_x2.txt #tmp_x3.txt tmp_x4.txt
    echo "Nota x1 + delta: $fx_x1_new"
    #echo "Nota x2 + delta: $fx_x2_new"
    #echo "Nota x3 + delta: $fx_x3_new"
    #echo -e "Nota x4 + delta: $fx_x4_new\n"

    # Calculo el gradiente
    gradient_x1=$(echo "($fx_x1_new - $fx) / $delta" | bc -l)
    #gradient_x2=$(echo "($fx_x2_new - $fx) / $delta" | bc -l)
    #gradient_x3=$(echo "($fx_x3_new - $fx) / $delta" | bc -l)
    #gradient_x4=$(echo "($fx_x4_new - $fx) / $delta" | bc -l)
    echo "Gradiente X1: $gradient_x1"
    #echo "Gradiente X2: $gradient_x2"
    #echo "Gradiente X3: $gradient_x3"
    #echo -e "Gradiente X4: $gradient_x4\n"

    # Actualizo los valores de las variables
    x1=$(echo "$x1 + ($learning_rate * $gradient)" | bc -l)
    #x2=$(echo "$x2 + ($learning_rate * $gradient)" | bc -l)
    #x3=$(echo "$x3 + ($learning_rate * $gradient)" | bc -l)
    #x4=$(echo "$x4 + ($learning_rate * $gradient)" | bc -l)
    echo -e "X1 actualizado: $x1"
    #echo -e "X2 actualizado: $x2"
    #echo -e "X3 actualizado: $x3"
    #echo -e "X4 actualizado: $x4"
    echo -e "-------------------------------------------\n"
done

echo "Optimization finished. Final ciclos_min = $ratio_bateria_vida"

#for (( i=0; i < max_iterations; i++ )); do
#    # Calculo la nota sin cambio
#    nota_previa=$(./test_nivel_4_paralel.sh $max_bat $porcentaje_bat $ciclos_min $min_vida)
#    echo -e "Nota previa: $nota_previa \n"
#
#    # Calculo la nota tras cambio
#    min_vida_new=$(echo "$min_vida + $delta" | bc -l)
#    echo "Variable + delta new: $min_vida_new"
#    nota_nueva=$(./test_nivel_4_paralel.sh $max_bat $porcentaje_bat $ciclos_min $min_vida_new)
#    echo -e "Nota nueva: $nota_nueva \n"
#
#    # Calculo el gradiente
#    gradient=$(echo "($nota_nueva - $nota_previa) / $delta" | bc -l)
#    echo "Gradient: $gradient"
#
#    # Actualizo los valores de las variables
#    min_vida=$(echo "$min_vida + ($learning_rate * $gradient)" | bc -l)
#    echo -e "Updated: $min_vida\n"
#
#    # Muestro información de la iteración actual
#    echo -e "Iteration $i: Nuevo valor = $min_vida, Nota = $nota_previa, gradient = $gradient \n\n"
#done
#
#echo "Optimization finished. Final ciclos_min = $min_vida"