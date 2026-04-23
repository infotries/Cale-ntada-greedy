#!/bin/bash

echo "# n tiempo_medio" > tiempos_insercion.dat

for f in data/*.tsp; do
    n=$(grep "DIMENSION" "$f" | grep -o '[0-9]\+')
    suma=0

    for i in {1..20}; do
        t=$(/usr/bin/time -f "%e" sh -c "./insercion_economica.bin \"$f\" > /dev/null" 2>&1 | tail -n 1)
        suma=$(echo "$suma + $t" | bc -l)
    done

    media=$(echo "$suma / 20" | bc -l)
    echo "$n $media" >> tiempos_insercion.dat
done