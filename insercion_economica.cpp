#include <iostream>
#include <vector>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;

vector<int> insercion_mas_barata(const vector<vector<double>>& distancia) {
    int n = distancia.size();
    if (n <= 3) {
        vector<int> tour(n);
        for (int i = 0; i < n; ++i) tour[i] = i;
        return tour;
    }

    vector<int> tour;
    vector<bool> visited(n, false);

    srand(time(NULL));
    while (tour.size() < 3) {
        int c = rand() % n;
        if (!visited[c]) {
            visited[c] = true;
            tour.push_back(c);
        }
    }

    while (tour.size() < n) {
        int best_k = -1;
        double min_dist_to_T = numeric_limits<double>::infinity();

        for (int k = 0; k < n; ++k) {
            if (!visited[k]) {
                for (int c : tour) {
                    if (distancia[k][c] < min_dist_to_T) {
                        min_dist_to_T = distancia[k][c];
                        best_k = k;
                    }
                }
            }
        }

        int best_pos = -1;
        double min_increase = numeric_limits<double>::infinity();

        for (size_t p = 0; p < tour.size(); ++p) {
            int i = tour[p];
            int j = tour[(p + 1) % tour.size()];
            double increase = distancia[i][best_k] + distancia[best_k][j] - distancia[i][j];

            if (increase < min_increase) {
                min_increase = increase;
                best_pos = p + 1;
            }
        }

        tour.insert(tour.begin() + best_pos, best_k);
        visited[best_k] = true;
    }

    return tour;
}