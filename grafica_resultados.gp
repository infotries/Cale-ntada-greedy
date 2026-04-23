set terminal pngcairo size 1000,600
set output 'grafica_resultados.png'

set title 'Coste obtenido por insercion mas economica'
set xlabel 'Instancia'
set ylabel 'Coste obtenido'
set grid
set xtics rotate by -45

plot 'resultados_insercion.dat' using 0:3:xtic(1) with linespoints pt 7 ps 1.5 lw 2 title 'Coste'