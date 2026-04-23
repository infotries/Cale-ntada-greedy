set terminal pngcairo size 1000,600 enhanced font "Arial,12"
set output 'grafica_tiempos_barras.png'

set datafile commentschars "#"

set title 'Tiempo de ejecucion por instancia'
set xlabel 'Instancia'
set ylabel 'Tiempo medio (s)'

set grid ytics lc rgb "#d9d9d9" lw 1
set border lw 1.2

set style data histograms
set style fill solid 1.0 border rgb "black"
set boxwidth 0.35

set xtics rotate by -45
set key top right

plot 'tiempos_insercion.dat' using 3:xtic(1) title 'Tiempo medio' lc rgb "#8a2be2"