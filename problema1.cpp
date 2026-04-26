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
    vector<tarea> tareas;
    vector<servidor> servidores;
    int tam, r, c;

    cout << "Introduce el número de tareas/servidores: ";
    cin >> tam;

    for(int i = 0; i < tam; i++){
        cout << endl << "Introduce la capacidad de cálculo mínima de la tarea " << i+1 << ": ";
        cin >> r;
        tareas.push_back({i+1, r});
    }

    for(int i = 0; i < tam; i++){
        cout << endl << "Introduce la capacidad de procesamiento del servidor" << i+1 << ": ";
        cin >> c;
        servidores.push_back({i+1, c});
    }

    vector<pair<int,int>> solucion = greedy(tareas, servidores);

    cout << "Asignaciones:" << endl;
    for(int i = 0; i < solucion.size(); i++){
        cout << "(" << solucion[i].first << ", " << solucion[i].second << ")" << endl;
    }

    return 0;
}