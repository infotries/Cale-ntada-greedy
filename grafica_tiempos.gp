set terminal pngcairo size 1000,600
set output 'grafica_tiempos.png'

set title 'Tiempo medio de ejecucion por instancia'
set xlabel 'Instancia'
set ylabel 'Tiempo medio (s)'
set grid
set xtics rotate by -45

plot 'tiempos_insercion.dat' using 0:3:xtic(1) with linespoints pt 7 ps 1.5 lw 2 title 'Tiempo medio'