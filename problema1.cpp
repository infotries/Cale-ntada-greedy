#include <iostream>
#include <vector>
using namespace std;

struct tarea{
    int i; // indice de tarea
    int r;  // capacidad de cálculo mínima
};

struct servidor{
    int j; // indice
    int c; // capacidad de procesamiento
};

bool es_solucion(int i, int j, int n){
    return (i >= n || j >=n);
}

bool asignable(const tarea &t, const servidor &s){
    return s.c >= t.r;
}

void ordenar_tareas(vector<tarea> &tareas){
    for(int i = 0; i < tareas.size() - 1; i++){
        int min = i;
        for(int j = i + 1; j < tareas.size(); j++){
            if(tareas[j].r < tareas[min].r){
                min = j;
            }
        }

        if(min != i){
            tarea aux = tareas[i];
            tareas[i] = tareas[min];
            tareas[min] = aux;
        }
    }
}

void ordenar_servidores(vector<servidor> &servidores){
    for(int i = 0; i < servidores.size() - 1; i++){
        int min = i;
        for(int j = i + 1; j < servidores.size(); j++){
            if(servidores[j].c < servidores[min].c){
                min = j;
            }
        }

        if(min != i){
            servidor aux = servidores[i];
            servidores[i] = servidores[min];
            servidores[min] = aux;
        }
    }
}

vector<pair<int,int>> greedy(vector<tarea> &tareas, vector<servidor> &servidores){
    vector<pair<int,int>> solucion;

    ordenar_tareas(tareas);
    ordenar_servidores(servidores);

    int i = 0, j = 0, n = tareas.size();

    while(!es_solucion(i,j,n)){
        if(asignable(tareas[i], servidores[j])){
            solucion.push_back({tareas[i].i, servidores[j].j});
            i++;
            j++;
        }else{
            j++;
        }
    }
    
    return solucion;
}

int main(){
    vector<tarea> tareas = {{1,12}, {2,5}, {3,8}, {4,20}, {5,7}, {6,15}};
    vector<servidor> servidores = {{1,10}, {2,6}, {3,18}, {4,14}, {5,5}, {6,22}};
    vector<pair<int,int>> solucion = greedy(tareas, servidores);

    cout << "Asignaciones:" << endl;
    for(int i = 0; i < solucion.size(); i++){
        cout << "(" << solucion[i].first << ", " << solucion[i].second << ")" << endl;
    }

    return 0;
}