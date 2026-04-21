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

// Conjunto de candidatos C: ciudades que aun no se han insertado en el subtour.
//Guardo el indice de los que estan en false en el vector de candidatos 
vector<int> construir_candidatos(int n, const vector<bool>& en_tour) {
    vector<int> candidatos;
    for (int i = 0; i < n; ++i) {
        if (!en_tour[i]) {
            candidatos.push_back(i);
        }
    }
    return candidatos;
}

// Funcion de seleccion: elige la ciudad k con menor d(k, T) = min_{c in T} d(k, c).
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

// Funcion de factibilidad: comprobar que la ciudad no este ya en el subtour.
bool es_factible_insertar(int ciudad, const vector<bool>& en_tour) {
    return ciudad >= 0 && ciudad < (int)en_tour.size() && !en_tour[ciudad];
}

// Devuelve la posicion de insercion que minimiza Delta(i,j) para insertar k entre i y j.
int mejor_posicion_de_insercion(
    const vector<int>& subtour,
    int ciudad_k,
    const vector<vector<int>>& dist
) {
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

// Funcion objetivo: coste total del tour.
int funcion_objetivo(const vector<int>& tour, const vector<vector<int>>& dist) {
    return compute_tour_cost(tour, dist);
}

// Funcion de solucion: comprueba si ya tenemos una solucion completa.
bool es_solucion_completa(const vector<int>& subtour, int total_ciudades) {
    return (int)subtour.size() == total_ciudades;
}

pair<vector<int>, int> heuristic_economic_insertion(const vector<vector<int>>& dist) {
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

    // Subtour inicial aleatorio de 3 ciudades distintas usando rand().
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
            break;  // caso de seguridad
        }

        int pos = mejor_posicion_de_insercion(subtour, ciudad_k, dist);
        subtour.insert(subtour.begin() + pos, ciudad_k);
        en_tour[ciudad_k] = true;
    }

    int coste = funcion_objetivo(subtour, dist);
    return {subtour, coste};
}

int main(int argc, char* argv[]) {
    // g++ insercion_economica.cpp data_loader.cpp plot_tour.cpp -o insercion_economica.bin -O3 && ./insercion_economica.bin ./data/berlin52.tsp
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <ruta_fichero_tsp>" << endl;
        return 1;
    }

    srand((unsigned int)time(NULL));

    string file_path = argv[1];
    pair<vector<Node>, vector<int>> parsed_data = parse_tsplib_file(file_path);
    vector<Node> nodes = parsed_data.first;
    vector<int> optimal_tour = parsed_data.second;

    if (nodes.empty()) {
        cout << "No se han cargado nodos. Revisa la ruta y el formato del fichero." << endl;
        return 1;
    }

    vector<vector<int>> dist_matrix = build_distance_matrix(nodes);
    cout << "Instancia cargada con " << dist_matrix.size() << " ciudades." << endl;

    string instance_name = get_instance_name(file_path);

    if (!optimal_tour.empty()) {
        int optimal_cost = compute_tour_cost(optimal_tour, dist_matrix);
        cout << "Coste optimo (fichero): " << optimal_cost << endl;

        string optimal_png = instance_name + "_optimal_tour.png";
        if (export_tour_to_png(nodes, optimal_tour, optimal_png, "Optimal Tour", "#1f77b4")) {
            cout << "PNG del tour optimo guardado en " << optimal_png << endl;
        } else {
            cout << "No se pudo generar el PNG del tour optimo (requiere gnuplot)." << endl;
        }
    } else {
        cout << "El fichero no incluye tour optimo." << endl;
    }

    pair<vector<int>, int> result = heuristic_economic_insertion(dist_matrix);
    cout << "Coste insercion mas economica: " << result.second << endl;

    if (!optimal_tour.empty()) {
        int optimal_cost = compute_tour_cost(optimal_tour, dist_matrix);
        double ratio = 100.0 * static_cast<double>(result.second) / static_cast<double>(optimal_cost);
        cout << "Relacion respecto al optimo: " << ratio << "%" << endl;
    }

    if (!result.first.empty()) {
        string out_png = instance_name + "_insercion_economica_tour.png";
        if (export_tour_to_png(nodes, result.first, out_png, "Economic Insertion Tour", "#d62728")) {
            cout << "PNG del tour calculado guardado en " << out_png << endl;
        } else {
            cout << "No se pudo generar el PNG del tour calculado (requiere gnuplot)." << endl;
        }
    }

    return 0;
}
