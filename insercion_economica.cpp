#include "data_loader.h"
#include "plot_tour.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;
const int INF = 1000000000;

//Construye el conjunto de ciudades candidatas a insertar, es decir, las ciudades que todavía no están dentro del subtour
//int n -> numero total de ciudades
//const vector<bool>& en_tour , true(significa q la ciudad ya está en el subtour) , false(significa q aún no está)
vector<int> construir_candidatos(int n, const vector<bool>& en_tour) {
    vector<int> candidatos;
    for (int i = 0; i < n; ++i) {
        if (!en_tour[i]) {
            candidatos.push_back(i);
        }
    }
    return candidatos;
}
//Este bloque implementa la idea de mantener el conjunto de candidatos C: ciudades aún no introducidas en el tour
//COSTE -> Recorre n ciudades, Complejidad: O(n)



//Selecciona la ciudad candidata k cuya distancia al subtour actual sea mínima
//subtour -> contiene las ciudades ya insertadas, en el orden en que forman el tour parcial
//candidatos -> contiene las ciudades aún no insertadas
//dist -> matriz de distancias ,dist[i][j] es la distancia entre las ciudades i y j
int seleccionar_ciudad_mas_cercana_al_subtour(
    const vector<int>& subtour,
    const vector<int>& candidatos,
    const vector<vector<int>>& dist) {
    int mejor_ciudad = -1;
    int mejor_distancia = INF;

    for (int k : candidatos) {
        int distancia_a_subtour = INF;
        for (int c : subtour) {
            if (dist[k][c] < distancia_a_subtour) {
                distancia_a_subtour = dist[k][c];
            }
        }

        if (distancia_a_subtour < mejor_distancia) {
            mejor_distancia = distancia_a_subtour;
            mejor_ciudad = k;
        }
    }

    return mejor_ciudad;
}
//Esta es la función de selección greedy principal, entre todas las ciudades no insertadas, elige la “más prometedora” según el criterio del algoritmo
//COSTE -> Si hay: r candidatos y m ciudades en el subtour entonces el coste es O(r·m)



//Comprueba si una ciudad puede insertarse
bool es_factible_insertar(int ciudad, const vector<bool>& en_tour) {
    return !en_tour[ciudad];
}
//COSTE -> O(1)



//Dada una ciudad k ya elegida, decide entre qué dos ciudades consecutivas del subtour conviene insertarla para que el aumento del coste sea mínimo
//resuelve esta ecuación ∆i,j = d(i, k) + d(k, j) − d(i, j).
int mejor_posicion_de_insercion(const vector<int>& subtour, int ciudad_k, const vector<vector<int>>& dist) {
    int mejor_posicion = 1;
    int mejor_incremento = INF;
    int m = (int)subtour.size();

    for (int i = 0; i < m; ++i) {
        int j = (i + 1) % m;
        int ciudad_i = subtour[i];
        int ciudad_j = subtour[j];
        int incremento = dist[ciudad_i][ciudad_k] + dist[ciudad_k][ciudad_j] - dist[ciudad_i][ciudad_j];

        if (incremento < mejor_incremento) {
            mejor_incremento = incremento;
            mejor_posicion = i + 1;  // insertar tras i
        }
    }

    return mejor_posicion;
}
//Esta función implementa la segunda decisión greedy: ya elegida la ciudad, ahora se decide dónde insertarla con el menor daño posible sobre el coste total
//COSTE -> Si el subtour tiene tamaño m, el coste es O(m)



//Devuelve el coste total del tour
int funcion_objetivo(const vector<int>& tour, const vector<vector<int>>& dist) {
    return compute_tour_cost(tour, dist);
}
//COSTE -> Depende de compute_tour_cost, pero normalmente sería O(n)



//Comprueba si el subtour ya contiene todas las ciudades
bool es_solucion_completa(const vector<int>& subtour, int total_ciudades) {
    return (int)subtour.size() == total_ciudades;
}
//Si el tamaño del subtour coincide con el número total de ciudades, entonces:ya no quedan candidatos y el tour está completo
//COSTE -> O(1)



//Construye un tour del TSP con la heurística de inserción más económica: empieza con 3 ciudades aleatorias y va añadiendo, en cada paso,
//la ciudad no visitada más cercana al subtour en la posición que menos aumenta el coste.
pair<vector<int>, int> insercion_economica(const vector<vector<int>>& dist) {
    int n = (int)dist.size();
    vector<int> subtour;

    if (n == 0) {
        return {subtour, 0};
    }

    if (n <= 3) {
        for (int i = 0; i < n; ++i) {
            subtour.push_back(i);
        }
        return {subtour, funcion_objetivo(subtour, dist)};
    }

    // Subtour inicial aleatorio de 3 ciudades distintas
    int c1 = rand() % n;
    int c2 = rand() % n;
    while (c2 == c1) {
        c2 = rand() % n;
    }
    int c3 = rand() % n;
    while (c3 == c1 || c3 == c2) {
        c3 = rand() % n;
    }
    subtour.push_back(c1);
    subtour.push_back(c2);
    subtour.push_back(c3);

    vector<bool> en_tour(n, false);
    en_tour[subtour[0]] = true;
    en_tour[subtour[1]] = true;
    en_tour[subtour[2]] = true;

    // Construccion greedy del tour.
    while (!es_solucion_completa(subtour, n)) {
        vector<int> candidatos = construir_candidatos(n, en_tour);
        int ciudad_k = seleccionar_ciudad_mas_cercana_al_subtour(subtour, candidatos, dist);

        if (!es_factible_insertar(ciudad_k, en_tour)) {
            int pos = mejor_posicion_de_insercion(subtour, ciudad_k, dist);
            subtour.insert(subtour.begin() + pos, ciudad_k);
            en_tour[ciudad_k] = true;
        }
    }

    int coste = funcion_objetivo(subtour, dist);
    return {subtour, coste};
}
//COSTE -> La complejidad temporal de la heurística de inserción más económica implementada es O(n^3)




int main(int argc, char* argv[]) {
    // g++ insercion_economica.cpp data_loader.cpp plot_tour.cpp -o insercion_economica.bin -O3 && ./insercion_economica.bin ./data/berlin52.tsp
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <ruta_fichero_tsp>" << endl;
        return 1;
    }

    srand(time(NULL));

    string file_path = argv[1];
    pair<vector<Node>, vector<int>> parsed_data = parse_tsplib_file(file_path);
    vector<Node> nodos = parsed_data.first;
    vector<int> optimo = parsed_data.second;

    if (nodos.empty()) {
        cout << "No se han cargado nodos. Revisa la ruta y el formato del fichero." << endl;
        return 1;
    }

    vector<vector<int>> dist_matriz = build_distance_matrix(nodos);
    cout << "Instancia cargada con " << dist_matriz.size() << " ciudades." << endl;

    string instance_name = get_instance_name(file_path);

    if (!optimo.empty()) {
        int coste_optimo = compute_tour_cost(optimo, dist_matriz);
        cout << "Coste optimo (fichero): " << coste_optimo << endl;

        string png = instance_name + "_optimal_tour.png";
        if (export_tour_to_png(nodos, optimo, png, "Optimal Tour", "#1f77b4")) {
            cout << "PNG del tour optimo guardado en " << png << endl;
        } else {
            cout << "No se pudo generar el PNG del tour optimo (requiere gnuplot)." << endl;
        }
    } else {
        cout << "El fichero no incluye tour optimo." << endl;
    }

    pair<vector<int>, int> resultado = insercion_economica(dist_matriz);
    cout << "Coste insercion mas economica: " << resultado.second << endl;

    if (!optimo.empty()) {
        int optimal_cost = compute_tour_cost(optimo, dist_matriz);
        double ratio = 100.0 * static_cast<double>(resultado.second) / static_cast<double>(optimal_cost);
        cout << "Relacion respecto al optimo: " << ratio << "%" << endl;
    }

    if (!resultado.first.empty()) {
        string png2 = instance_name + "_insercion_economica_tour.png";
        if (export_tour_to_png(nodos, resultado.first, png2, "Economic Insertion Tour", "#d62728")) {
            cout << "PNG del tour calculado guardado en " << png2 << endl;
        } else {
            cout << "No se pudo generar el PNG del tour calculado (requiere gnuplot)." << endl;
        }
    }

    return 0;
}


//IMPORTANTE DESTACAR
// 1. Conjunto de candidatos -> construir_candidatos(...)
// 2. Función de selección -> seleccionar_ciudad_mas_cercana_al_subtour(...)
// 3. Función de factibilidad -> es_factible_insertar(...)
// 4. Función objetivo -> funcion_objetivo(...)
// 5. Función de solución -> es_solucion_completa(...)