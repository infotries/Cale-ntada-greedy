#include "data_loader.h"
#include "plot_tour.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace std;

pair<vector<int>, int> heuristic_nearest_insertion(
    const vector<vector<int>>& distance_matrix
) {
    int n = distance_matrix.size();
    vector<int> tour;

    if (n == 0) return {tour, 0};
    if (n == 1) return {{0}, 0};
    if (n == 2) {
        tour = {0, 1};
        return {tour, compute_tour_cost(tour, distance_matrix)};
    }

    // 1) Elegir 3 ciudades iniciales
    vector<int> cities(n);
    iota(cities.begin(), cities.end(), 0);

    mt19937 gen(42);   // semilla fija para que sea reproducible
    shuffle(cities.begin(), cities.end(), gen);

    tour = {cities[0], cities[1], cities[2]};

    vector<bool> in_tour(n, false);
    in_tour[cities[0]] = true;
    in_tour[cities[1]] = true;
    in_tour[cities[2]] = true;

    // 2) Insertar el resto de ciudades
    while ((int)tour.size() < n) {
        int best_city = -1;
        int best_distance_to_tour = numeric_limits<int>::max();

        // Elegir la ciudad fuera del tour más cercana al subtour
        for (int k = 0; k < n; ++k) {
            if (in_tour[k]) continue;

            int min_dist = numeric_limits<int>::max();
            for (int c : tour) {
                min_dist = min(min_dist, distance_matrix[k][c]);
            }

            if (min_dist < best_distance_to_tour) {
                best_distance_to_tour = min_dist;
                best_city = k;
            }
        }

        // Buscar la mejor posición de inserción
        int best_pos = -1;
        int best_increase = numeric_limits<int>::max();
        int m = tour.size();

        for (int i = 0; i < m; ++i) {
            int a = tour[i];
            int b = tour[(i + 1) % m];

            int increase = distance_matrix[a][best_city]
                         + distance_matrix[best_city][b]
                         - distance_matrix[a][b];

            if (increase < best_increase) {
                best_increase = increase;
                best_pos = i + 1;
            }
        }

        tour.insert(tour.begin() + best_pos, best_city);
        in_tour[best_city] = true;
    }

    int total_cost = compute_tour_cost(tour, distance_matrix);
    return {tour, total_cost};
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <ruta_fichero.tsp>" << endl;
        return 1;
    }

    string file_path = argv[1];

    pair<vector<Node>, vector<int>> parsed_data = parse_tsplib_file(file_path);
    vector<Node> nodes = parsed_data.first;
    vector<int> optimal_tour = parsed_data.second;

    if (nodes.empty()) {
        cout << "No se han cargado nodos. Revisa la ruta o el formato del fichero." << endl;
        return 1;
    }

    vector<vector<int>> dist_matrix = build_distance_matrix(nodes);
    cout << "Instancia cargada con " << dist_matrix.size() << " ciudades." << endl;

    string instance_name = get_instance_name(file_path);

    // Tour óptimo del fichero
    if (!optimal_tour.empty()) {
        int optimal_cost = compute_tour_cost(optimal_tour, dist_matrix);
        cout << "Coste tour óptimo: " << optimal_cost << endl;

        string optimal_png_path = instance_name + "_optimal_tour.png";
        if (export_tour_to_png(nodes, optimal_tour, optimal_png_path, "Optimal Tour", "#1f77b4")) {
            cout << "Imagen del tour óptimo guardada en: " << optimal_png_path << endl;
        } else {
            cout << "No se pudo generar PNG del tour óptimo." << endl;
        }
    } else {
        cout << "El fichero no trae tour óptimo." << endl;
    }

    // Heurística de inserción
    pair<vector<int>, int> result = heuristic_nearest_insertion(dist_matrix);

    cout << "Coste insercion economica: " << result.second << endl;
    cout << "Tour obtenido: ";
    for (int city : result.first) {
        cout << city << " ";
    }
    cout << endl;

    if (!result.first.empty()) {
        string computed_png_path = instance_name + "_insercion_economica_tour.png";
        if (export_tour_to_png(nodes, result.first, computed_png_path, "Insercion Economica", "#d62728")) {
            cout << "Imagen del tour calculado guardada en: " << computed_png_path << endl;
        } else {
            cout << "No se pudo generar PNG del tour calculado." << endl;
        }
    }

    return 0;
}