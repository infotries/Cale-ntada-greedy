#include "nearest_insertion.h"
#include "data_loader.h"
#include "plot_tour.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

const int tope = 1000000000;

vector<int> construir_candidatos(int n, const vector<bool>& en_tour) {
    vector<int> candidatos;
    for (int i = 0; i < n; ++i) {
        if (!en_tour[i]) {
            candidatos.push_back(i);
        }
    }
    return candidatos;
}

int seleccionar_ciudad_mas_cercana_al_subtour(
    const vector<int>& subtour,
    const vector<int>& candidatos,
    const vector<vector<int>>& dist) {
    int mejor_ciudad = -1;
    int mejor_distancia = tope;

    for (int k : candidatos) {
        int distancia_a_subtour = tope;
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


bool es_factible_insertar(int ciudad, const vector<bool>& en_tour) {
    return !en_tour[ciudad];
}

int mejor_posicion_de_insercion(const vector<int>& subtour, int ciudad_k, const vector<vector<int>>& dist) {
    int mejor_posicion = 1;
    int mejor_incremento = tope;
    int m = subtour.size();

    for (int i = 0; i < m; ++i) {
        int j = (i + 1) % m;
        int ciudad_i = subtour[i];
        int ciudad_j = subtour[j];
        int incremento = dist[ciudad_i][ciudad_k] + dist[ciudad_k][ciudad_j] - dist[ciudad_i][ciudad_j];

        if (incremento < mejor_incremento) {
            mejor_incremento = incremento;
            mejor_posicion = i + 1; 
        }
    }

    return mejor_posicion;
}

int funcion_objetivo(const vector<int>& tour, const vector<vector<int>>& dist) {
    return compute_tour_cost(tour, dist);
}

bool es_solucion_completa(const vector<int>& subtour, int total_ciudades) {
    return subtour.size() == total_ciudades;
}
pair<vector<int>, int> heuristic_nearest_insertion(
    const vector<vector<int>>& distance_matrix
) {
    int n = distance_matrix.size();
    vector<int> tour;

    if (n == 0) {
        return {tour, 0};
    }

    if (n <= 3) {
        for (int i = 0; i < n; ++i) {
            tour.push_back(i);
        }
        return {tour, funcion_objetivo(tour, distance_matrix)};
    }

    int c1 = rand() % n;
    int c2 = rand() % n;
    while (c2 == c1) {
        c2 = rand() % n;
    }
    int c3 = rand() % n;
    while (c3 == c1 || c3 == c2) {
        c3 = rand() % n;
    }
    tour.push_back(c1);
    tour.push_back(c2);
    tour.push_back(c3);

    vector<bool> en_tour(n, false);
    en_tour[tour[0]] = true;
    en_tour[tour[1]] = true;
    en_tour[tour[2]] = true;

    while (!es_solucion_completa(tour, n)) {
        vector<int> candidatos = construir_candidatos(n, en_tour);
        int ciudad_k = seleccionar_ciudad_mas_cercana_al_subtour(tour, candidatos, distance_matrix);

        if (es_factible_insertar(ciudad_k, en_tour)) {
            int pos = mejor_posicion_de_insercion(tour, ciudad_k, distance_matrix);
            tour.insert(tour.begin() + pos, ciudad_k);
            en_tour[ciudad_k] = true;
        }
    }

    int coste = funcion_objetivo(tour, distance_matrix);
    return {tour, coste};
}

int main(int argc, char* argv[]) {
    // g++ nearest_insertion.cpp data_loader.cpp plot_tour.cpp -o nearest_insertion.bin -O3 && ./nearest_insertion.bin ./data/berlin52.tsp
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <path_to_tsp_file>" << endl;
        return 1;
    }

    string file_path = argv[1];
    pair<vector<Node>, vector<int>> parsed_data = parse_tsplib_file(file_path);
    vector<Node> nodes = parsed_data.first;
    vector<int> optimal_tour = parsed_data.second;

    if (nodes.empty()) {
        cout << "No nodes loaded. Please check the file path and format." << endl;
        return 1;
    }

    vector<vector<int>> dist_matrix = build_distance_matrix(nodes);
    cout << "Successfully loaded " << dist_matrix.size() << " nodes." << endl;
    string instance_name = get_instance_name(file_path);

    if (!optimal_tour.empty()) {
        int optimal_cost = compute_tour_cost(optimal_tour, dist_matrix);
        cout << "Optimal Tour Cost from file: " << optimal_cost << endl;

        string optimal_png_path = instance_name + "_optimal_tour.png";
        if (export_tour_to_png(nodes, optimal_tour, optimal_png_path, "Optimal Tour", "#1f77b4")) {
            cout << "Saved optimal tour visualization to " << optimal_png_path << endl;
        } else {
            cout << "Could not write optimal tour PNG (requires gnuplot)." << endl;
        }
    } else {
        cout << "No optimal tour found in input file." << endl;
    }

    pair<vector<int>, int> insertion_result = heuristic_nearest_insertion(dist_matrix);
    cout << "Nearest Insertion Cost: " << insertion_result.second << endl;

    if (!insertion_result.first.empty()) {
        string computed_png_path = instance_name + "_nearest_insertion_tour.png";
        if (export_tour_to_png(nodes, insertion_result.first, computed_png_path, "Nearest Insertion Tour", "#d62728")) {
            cout << "Saved nearest-insertion tour visualization to " << computed_png_path << endl;
        } else {
            cout << "Could not write nearest-insertion tour PNG (requires gnuplot)." << endl;
        }
    } else {
        cout << "No computed nearest-insertion tour to visualize." << endl;
    }

    return 0;
}
