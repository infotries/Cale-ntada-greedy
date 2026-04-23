#!/usr/bin/env bash
set -euo pipefail

# ==========================================================
# CONFIGURACIÓN
# ==========================================================
PROJECT_DIR="/mnt/c/Users/dario/Desktop/Ugr/2º/Segundo cuatri/Algoritmica/pract3"
DATA_DIR="$PROJECT_DIR/data"

# CPP principal que quieres probar
MAIN_CPP="nearest_neighbor.cpp"

# Otros .cpp necesarios para compilar
EXTRA_CPPS=("data_loader.cpp" "plot_tour.cpp")

# Nombre del ejecutable de salida
EXECUTABLE="programa_medicion"

# Número de repeticiones por mapa para tiempo medio
REPS=5

# Ficheros de salida
TXT_OUT="tabla_resultados.txt"
CSV_OUT="tabla_resultados.csv"
DAT_OUT="datos_grafica.dat"
GP_OUT="grafica_resultados.gp"
PNG_OUT="comparacion_solucion_vs_optima.png"

# ==========================================================
# IR A LA CARPETA DEL PROYECTO
# ==========================================================
cd "$PROJECT_DIR"

# ==========================================================
# COMPILACIÓN
# ==========================================================
echo "Compilando $MAIN_CPP ..."
g++ "$MAIN_CPP" "${EXTRA_CPPS[@]}" -O3 -o "$EXECUTABLE"
echo "Compilación completada."
echo

# ==========================================================
# CABECERAS DE SALIDA
# ==========================================================
printf "%-15s %-18s %-18s %-18s %-12s %-12s\n" \
  "Mapa" "Tiempo medio (s)" "Solución" "Óptima" "Dif." "Error %" > "$TXT_OUT"

printf "%-15s %-18s %-18s %-18s %-12s %-12s\n" \
  "---------------" "------------------" "------------------" "------------------" "------------" "------------" >> "$TXT_OUT"

echo "Mapa,Tiempo_medio_seg,Solucion,Optima,Diferencia,Error_porcentual" > "$CSV_OUT"
echo "#Mapa Solucion Optima" > "$DAT_OUT"

# ==========================================================
# PROCESAR MAPAS
# ==========================================================
shopt -s nullglob
MAPS=("$DATA_DIR"/*.tsp)

if [ ${#MAPS[@]} -eq 0 ]; then
  echo "No se encontraron mapas .tsp en $DATA_DIR"
  exit 1
fi

for map in "${MAPS[@]}"; do
  map_name=$(basename "$map")
  echo "Procesando $map_name ..."

  # Ejecutamos una vez para extraer solución y óptimo
  output=$("./$EXECUTABLE" "$map")

  # Ajusta estos grep si tu programa imprime otras etiquetas
  solucion=$(echo "$output" | grep "Nearest Neighbor Cost:" | awk -F': ' '{print $2}' | tr -d '\r')
  optima=$(echo "$output" | grep "Optimal Tour Cost from file:" | awk -F': ' '{print $2}' | tr -d '\r')

  if [ -z "${solucion:-}" ] || [ -z "${optima:-}" ]; then
    echo "No se pudieron extraer los datos de $map_name"
    echo "Salida del programa:"
    echo "$output"
    exit 1
  fi

  # Tiempo medio
  total_time=0
  for ((i=1; i<=REPS; i++)); do
    t=$({ /usr/bin/time -f "%e" "./$EXECUTABLE" "$map" >/dev/null; } 2>&1)
    total_time=$(awk -v a="$total_time" -v b="$t" 'BEGIN{printf "%.8f", a+b}')
  done
  avg_time=$(awk -v total="$total_time" -v reps="$REPS" 'BEGIN{printf "%.8f", total/reps}')

  # Diferencia y error %
  diff=$((solucion - optima))
  error_pct=$(awk -v s="$solucion" -v o="$optima" 'BEGIN{printf "%.2f", ((s-o)/o)*100}')

  # Nombre corto para la gráfica
  short_name="${map_name%.tsp}"

  # Guardar tabla TXT
  printf "%-15s %-18s %-18s %-18s %-12s %-12s\n" \
    "$map_name" "$avg_time" "$solucion" "$optima" "$diff" "$error_pct" >> "$TXT_OUT"

  # Guardar CSV
  echo "$map_name,$avg_time,$solucion,$optima,$diff,$error_pct" >> "$CSV_OUT"

  # Guardar datos para gnuplot
  echo "$short_name $solucion $optima" >> "$DAT_OUT"
done

# ==========================================================
# CREAR SCRIPT GNUPLOT
# ==========================================================
cat > "$GP_OUT" <<EOF
set terminal png size 1000,600
set output "$PNG_OUT"

set title "Comparación solución obtenida vs óptima"
set xlabel "Mapa"
set ylabel "Coste"

set grid
set key left top
set xtics rotate by -45

plot "$DAT_OUT" using 2:xtic(1) with linespoints title "Solución obtenida", \
     "" using 3 with linespoints title "Óptima"
EOF

# ==========================================================
# GENERAR GRÁFICA
# ==========================================================
gnuplot "$GP_OUT"

# ==========================================================
# FIN
# ==========================================================
echo
echo "Hecho."
echo "Tabla TXT:  $PROJECT_DIR/$TXT_OUT"
echo "Tabla CSV:  $PROJECT_DIR/$CSV_OUT"
echo "Datos plot: $PROJECT_DIR/$DAT_OUT"
echo "Script gp:  $PROJECT_DIR/$GP_OUT"
echo "Gráfica:    $PROJECT_DIR/$PNG_OUT"
