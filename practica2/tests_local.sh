#!/bin/bash

#Nivel 0
./practica2SG mapas/mapa30.map 1 0 5 5 2 10 10 4 12 5 > test_01.txt
./practica2SG mapas/mapa100.map 1 0 5 5 2 10 10 4 95 80 > test_02.txt

#Nivel 1
./practica2SG mapas/mapa30.map 1 1 9 3 0 7 3 6 3 3 > test_11.txt
./practica2SG mapas/mapa30.map 1 1 24 12 6 18 13 6 18 15 > test_12.txt
./practica2SG mapas/mapa30_nuevo.map 1 1 8 9 4 5 5 6 3 3 > test_13.txt
./practica2SG mapas/mapa30.map 1 1 25 11 6 5 10 6 3 3 > test_14.txt
./practica2SG mapas/mapa30.map 1 1 9 18 4 3 15 1 3 3 > test_15.txt
./practica2SG mapas/mapa30.map 1 1 8 13 0 11 18 0 18 12 > test_16.txt

#Nivel 2
./practica2SG mapas/mapa30.map 1 2 3 13 4 4 4 4 13 13 > test_21.txt
./practica2SG mapas/mapa50.map 1 2 19 7 6 3 3 3 16 13 > test_22.txt
./practica2SG mapas/paldea2.map 1 2 82 49 0 73 52 0 25 25 > test_23.txt
./practica2SG mapas/paldea2.map 1 2 35 79 0 50 63 0 85 16 > test_24.txt
./practica2SG mapas/scape.map 1 2 9 13 6 6 9 0 9 25 > test_25.txt
./practica2SG mapas/2ez.map 1 2 25 15 0 4 15 4 5 15 > test_26.txt
./practica2SG mapas/mapa50.map 1 2 22 14 0 25 7 0 19 14 > test_27.txt
./practica2SG mapas/ruinados.map 1 2 90 72 6 66 61 5 10 18 > test_28.txt
./practica2SG mapas/ruinados.map 1 2 90 72 6 93 82 3 10 18 > test_29.txt
./practica2SG mapas/marymonte.map 1 2 8 49 6 3 3 3 9 31 > test_210.txt
./practica2SG mapas/marymonte.map 1 2 67 62 6 3 3 3 67 61 > test_211.txt
./practica2SG mapas/paldea2.map 1 2 60 94 0 46 43 0 39 3 > test_212.txt

#Nivel 3
./practica2SG mapas/mapa30.map 1 3 3 13 6 8 11 6 8 14 > test_31.txt
./practica2SG mapas/mapa30.map 1 3 10 20 0 12 10 2 8 20 > test_32.txt
./practica2SG mapas/mapa30.map 1 3 3 15 0 8 17 6 8 23 > test_33.txt
./practica2SG mapas/scape2.map 0 3 22 19 2 14 18 2 20 13 > test_34.txt
./practica2SG mapas/scape2.map 0 3 14 19 2 22 20 2 18 13 > test_35.txt
./practica2SG mapas/scape2.map 0 3 14 18 2 22 19 2 20 13 > test_36.txt
./practica2SG mapas/scape2.map 0 3 22 20 2 14 19 2 18 13 > test_37.txt
./practica2SG mapas/scape2.map 0 3 14 19 2 22 19 2 19 13 > test_38.txt
./practica2SG mapas/scape2.map 1 3 12 22 0 6 22 4 9 26 > test_39.txt
./practica2SG mapas/scape2.map 1 3 14 19 4 22 19 0 5 23 > test_310.txt
./practica2SG mapas/lumi25.map 1 3 12 4 6 3 12 2 11 6 > test_311.txt
./practica2SG mapas/scape2.map 1 3 9 13 2 9 22 2 9 25 > test_312.txt
./practica2SG mapas/mapa30.map 1 3 21 10 4 18 18 6 18 11 > test_313.txt

#Nivel 4
./practica2SG mapas/mapa30.map 1 4 4 4 6 12 12 2 8 8 16 6 26 9 25 16 3 3 5 10 16 14 18 10 21 4 7 4 24 4 12 3 6 15 25 18 8 19 15 14 23 15 26 8 10 24 19 26 25 7 16 11 22 15 20 15 22 19 10 20 4 13 26 24 9 6 26 10 17 19 25 13 24 20 26 19 12 18 8 23 9 13 6 5 8 16 12 5 3 14 11 22 11 8 6 17 7 4 21 3 23 4 15 5 7 23 21 19 4 15 6 13 24 17 6 26 5 4 24 10 16 17 13 20 22 9 26 22 22 24 14 4 24 26 7 18 6 21 9 9 18 6 19 15 16 21 3 14 13 10 25 13 17 24 7 20 14 14 9 21 5 18 20 20 19 10 5 18 18 21 9 22 20 19 7 15 26 20 10 17 17 19 8 23 8 9 9 5 20 3 20 11 6 23 16 8 26 14 17 4 8 25 14 13 25 14 8 5 20 21 4 18 14 25 > test_P1.txt
./practica2SG mapas/mapa50.map 1 4 28 25 4 28 20 2 36 23 39 8 46 26 39 34 26 37 18 46 3 46 3 3 10 17 39 45 9 16 38 13 27 23 31 18 45 31 35 7 12 6 40 7 20 6 10 25 41 30 14 31 26 24 38 26 38 20 44 14 17 40 45 3 4 9 33 44 17 3 3 11 42 13 26 18 38 25 33 26 46 46 36 14 36 31 17 34 8 22 44 41 16 11 44 17 29 32 42 21 46 19 40 34 45 24 46 7 44 32 21 30 14 39 15 22 11 9 13 27 20 8 45 5 6 > test_P2.txt
./practica2SG mapas/mapa75.map 1 4 11 47 0 6 47 4 9 51 19 33 29 31 33 31 54 43 51 67 59 68 57 29 47 16 68 6 29 4 3 32 14 25 57 20 41 35 47 27 38 71 68 47 53 10 18 10 60 11 16 38 62 45 22 47 39 37 57 39 57 30 66 21 26 60 68 5 6 14 49 66 26 4 4 17 63 19 40 28 57 38 50 39 69 69 54 21 55 47 26 51 12 33 66 62 24 17 67 26 51 71 44 49 64 32 69 29 > test_P3.txt
./practica2SG mapas/mapa100.map 1 4 63 31 6 63 32 2 66 40 75 24 85 36 83 6 60 10 33 11 84 7 86 40 68 77 79 91 19 33 76 27 55 47 62 36 51 95 91 63 71 14 24 13 80 15 21 51 83 61 29 63 52 49 77 52 76 40 89 28 35 80 91 6 94 52 8 19 66 89 34 6 6 23 85 26 53 37 76 51 66 53 3 43 > test_P4.txt
./practica2SG mapas/pinkworld.map 1 4 46 26 2 41 27 2 44 46 26 59 26 10 59 10 59 63 70 38 41 35 47 27 38 71 68 47 18 10 60 11 30 9 66 21 68 5 71 39 6 14 49 66 4 17 40 28 50 39 69 69 12 33 66 62 67 26 51 71 44 49 64 32 69 29 68 36 > test_P5.txt
./practica2SG mapas/islas.map 1 4 47 53 2 49 53 2 41 56 52 53 74 54 74 47 46 42 71 56 83 52 58 65 85 43 92 39 81 68 91 48 21 95 92 14 88 64 43 61 28 78 30 44 22 18 27 55 41 16 90 10 12 49 76 68 38 74 > test_P6.txt
./practica2SG mapas/marymonte.map 1 4 46 26 2 41 27 2 44 46 26 59 26 10 59 10 59 63 70 38 41 35 47 27 38 71 68 47 18 10 60 11 30 9 66 21 68 5 71 39 6 14 49 66 4 17 40 28 50 39 69 69 12 33 66 62 67 26 51 71 44 49 64 32 69 29 68 36 > test_P7.txt
./practica2SG mapas/medieval.map 1 4 3 3 2 9 20 2 9 88 21 75 34 62 48 16 57 28 72 24 89 27 84 51 61 93 13 48 9 87 84 39 91 19 33 76 87 39 57 29 84 53 96 96 63 3 88 54 32 44 19 75 29 95 96 48 21 33 87 41 9 20 13 48 9 87 84 39 91 19 33 76 87 39 > test_P8.txt
./practica2SG mapas/lumi50_2.map 1 4 5 16 4 34 15 4 46 25 46 11 21 17 30 26 16 35 10 44 37 44 6 44 46 36 11 21 46 46 3 3 39 26 38 26 > test_P9.txt
./practica2SG mapas/lumi25.map 1 4 3 3 0 21 9 4 14 21 16 5 16 13 7 9 21 15 6 14 16 10 20 7 4 4 19 21 14 21 16 5 16 13 7 9 21 15 6 14 16 10 20 7 4 4 19 21 14 21 16 5 16 13 7 9 21 15 6 14 16 10 20 7 4 4 19 21 14 21 16 5 16 13 7 9 21 15 6 14 16 10 20 7 4 4 19 21 > test_P10.txt
./practica2SG mapas/paldea2.map 1 4 91 45 2 91 50 6 87 51 82 48 74 47 67 48 67 65 70 73 64 70 72 28 59 10 54 29 47 29 53 81 45 77 49 62 42 62 43 39 49 12 85 16 23 54 30 57 9 45 25 33 31 81 87 51 67 47 63 38 46 43 > test_S1.txt
./practica2SG mapas/sleepingpool.map 1 4 20 11 6 6 6 0 5 5 8 16 5 7 14 18 7 5 19 21 7 7 4 18 5 6 4 12 7 6 15 25 6 7 19 15 6 5 8 23 6 6 15 23 5 5 12 26 5 7 8 10 7 5 5 19 7 7 26 10 5 6 7 16 7 6 11 22 6 7 15 20 6 5 8 22 6 6 19 10 5 5 20 4 5 7 13 26 7 5 24 9 7 7 6 26 5 6 10 20 7 6 19 25 6 7 11 24 6 5 14 16 6 6 4 26 5 5 19 12 5 7 13 6 7 5 5 8 7 7 16 12 5 6 14 22 7 6 20 11 6 7 22 11 6 5 8 6 6 6 17 7 > test_S2.txt
./practica2SG mapas/paldea_con_foso.map 1 4 91 45 2 55 47 0 87 51 82 48 74 47 67 48 67 65 70 73 64 70 72 28 59 10 54 29 47 29 53 81 45 77 49 62 42 62 43 39 49 12 85 16 23 54 30 57 9 45 25 33 31 81 87 51 67 47 63 38 46 43 > test_S3.txt
./practica2SG mapas/luminaliaZA2.map 1 4 84 39 4 66 35 6 68 62 46 72 29 47 42 29 70 35 87 23 36 94 36 6 3 50 51 50 50 18 78 46 66 80 46 86 32 28 55 21 17 62 20 38 78 60 96 3 3 3 5 96 72 96 48 47 48 52 53 47 53 52 51 50 > test_S4.txt

