// Grafo.cpp
#include "Grafo.h"
#include <iostream>
#include <queue>
#include <stack>
#include <ctime>

using namespace std;

void GrafoPrerrequisitos::construir(const vector<Materia>& materias) {
    codigos.clear(); nombres.clear(); lista.clear();
    // Copia ordenada por codigo para poder buscar vertices con busqueda binaria
    vector<Materia> copia = materias;
    ordenarMergeSort(copia, POR_CODIGO);
    for (size_t i = 0; i < copia.size(); i++) {
        codigos.push_back(copia[i].getCodigo());
        nombres.push_back(copia[i].getNombre());
    }
    lista.assign(codigos.size(), vector<int>());
    // Arista prerrequisito -> materia
    for (size_t i = 0; i < copia.size(); i++) {
        const vector<string>& previos = copia[i].getPrerrequisitos();
        for (size_t k = 0; k < previos.size(); k++) {
            int u = indiceDe(previos[k]);
            if (u != -1) lista[u].push_back(static_cast<int>(i));
        }
    }
}

int GrafoPrerrequisitos::indiceDe(const string& codigo) const {
    int izq = 0, der = numVertices() - 1;
    while (izq <= der) {
        int medio = izq + (der - izq) / 2;
        if (codigos[medio] == codigo) return medio;
        if (codigos[medio] < codigo) izq = medio + 1;
        else der = medio - 1;
    }
    return -1;
}

void GrafoPrerrequisitos::bfs(int inicio, vector<int>& orden, vector<int>& dist, vector<int>& padre) const {
    int n = numVertices();
    vector<bool> vis(n, false);
    dist.assign(n, -1); padre.assign(n, -1); orden.clear();
    if (inicio < 0 || inicio >= n) return;
    queue<int> cola;
    cola.push(inicio); vis[inicio] = true; dist[inicio] = 0;
    while (!cola.empty()) {
        int u = cola.front(); cola.pop();       // FIFO: produce el orden por niveles
        orden.push_back(u);
        for (int v : lista[u]) if (!vis[v]) {
            vis[v]   = true;                    // se marca al ENCOLAR, no al sacar
            dist[v]  = dist[u] + 1;
            padre[v] = u;
            cola.push(v);
        }
    }
}

int GrafoPrerrequisitos::contarDependientes(const string& codigo) const {
    int u = indiceDe(codigo);
    if (u == -1) return 0;
    vector<int> orden, dist, padre;
    bfs(u, orden, dist, padre);
    return static_cast<int>(orden.size()) - 1;  // no cuenta la propia materia
}

int GrafoPrerrequisitos::maxDependientes() const {
    int mayor = 0;
    for (int i = 0; i < numVertices(); i++) {
        int d = contarDependientes(codigos[i]);
        if (d > mayor) mayor = d;
    }
    return mayor;                               // O(V * (V + E)), igual que el reto 3 de la sesion 43
}

vector<string> GrafoPrerrequisitos::materiasAfectadas(const string& codigo) const {
    vector<string> afectadas;
    int u = indiceDe(codigo);
    if (u == -1) return afectadas;
    vector<int> orden, dist, padre;
    bfs(u, orden, dist, padre);
    for (size_t i = 1; i < orden.size(); i++) afectadas.push_back(codigos[orden[i]]);   // sin la propia
    return afectadas;
}

void GrafoPrerrequisitos::mostrarMateriasAfectadas(const string& codigo, const string& motivo) const {
    int u = indiceDe(codigo);
    if (u == -1) { cout << "La materia no existe en la red." << endl; return; }
    vector<int> orden, dist, padre;
    bfs(u, orden, dist, padre);
    cout << "Motivo de la consulta: " << motivo << " " << codigos[u] << " (" << nombres[u] << ")" << endl;
    if (orden.size() <= 1) {
        cout << "Materias afectadas: [] (ninguna materia depende de " << codigo << ")." << endl;
        return;
    }
    cout << "Al " << motivo << " " << codigos[u] << " quedan bloqueadas "
         << orden.size() - 1 << " materia(s):" << endl;
    // Como BFS saca por niveles, 'orden' ya viene agrupado por distancia
    int nivelActual = 0;
    for (size_t i = 1; i < orden.size(); i++) {
        int v = orden[i];
        if (dist[v] != nivelActual) {
            nivelActual = dist[v];
            cout << "  nivel " << nivelActual << (nivelActual == 1 ? " (directas):" : " (indirectas):") << endl;
        }
        cout << "    - " << codigos[v] << " " << nombres[v] << "  (via " << codigos[padre[v]] << ")" << endl;
    }
}

void GrafoPrerrequisitos::mostrarCadena(const string& origen, const string& destino) const {
    int s = indiceDe(origen), t = indiceDe(destino);
    if (s == -1 || t == -1) { cout << "Alguna de las materias no existe." << endl; return; }
    vector<int> orden, dist, padre;
    bfs(s, orden, dist, padre);
    if (dist[t] == -1) { cout << destino << " no depende de " << origen << "." << endl; return; }
    // Seguir el arreglo padre hacia atras hasta el origen (padre -1) e invertir
    vector<string> ruta;
    for (int v = t; v != -1; v = padre[v]) ruta.push_back(codigos[v]);
    cout << "Cadena: ";
    for (int i = static_cast<int>(ruta.size()) - 1; i >= 0; i--)
        cout << ruta[i] << (i > 0 ? " -> " : "");
    cout << "  (" << dist[t] << " tramo(s))" << endl;
}

bool GrafoPrerrequisitos::tieneCiclo() const {
    // DFS iterativo con pila. color: 0 = sin visitar, 1 = en el camino actual, 2 = terminado.
    // Una arista hacia un vertice con color 1 cierra un ciclo.
    int n = numVertices();
    vector<int> color(n, 0), siguiente(n, 0);   // siguiente[u] = proximo vecino por revisar
    for (int r = 0; r < n; r++) {
        if (color[r] != 0) continue;
        stack<int> pila;
        pila.push(r); color[r] = 1;
        while (!pila.empty()) {
            int u = pila.top();
            if (siguiente[u] < static_cast<int>(lista[u].size())) {
                int v = lista[u][siguiente[u]++];
                if (color[v] == 1) return true;
                if (color[v] == 0) { color[v] = 1; pila.push(v); }
            } else {
                color[u] = 2;
                pila.pop();
            }
        }
    }
    return false;
}

void GrafoPrerrequisitos::mostrarRed() const {
    cout << "Red de prerrequisitos (" << numVertices() << " materias). A -> B: B requiere A" << endl;
    for (int i = 0; i < numVertices(); i++) {
        cout << "  " << codigos[i] << " ->";
        if (lista[i].empty()) cout << " (ninguna)";
        for (int v : lista[i]) cout << " " << codigos[v];
        cout << "   [" << contarDependientes(codigos[i]) << " dependiente(s) en total]" << endl;
    }
    cout << (tieneCiclo() ? "ADVERTENCIA: la red tiene un ciclo (plan no cursable)."
                          : "La red es aciclica (DAG).") << endl;
}

// ============================================================================
// PRIORIDAD
// ============================================================================
double puntajeSemestre(int semestre) {
    // Primeros semestres (adaptacion) y ultimos (riesgo de retrasar el grado)
    return (semestre <= 2 || semestre >= 8) ? 1.0 : 0.0;
}

double puntajeDependencia(int dependientes, int maxDep) {
    if (maxDep <= 0) return 0.0;                // red sin dependencias: el criterio no discrimina
    return static_cast<double>(dependientes) / maxDep;
}

double puntajeAntiguedad(int dias) {
    if (dias >= 7) return 1.0;                  // maxima prioridad
    if (dias >= 4) return 0.6;                  // media (alrededor de 4 dias)
    if (dias == 3) return 0.45;                 // transicion baja -> media
    if (dias >= 1) return 0.25;                 // baja (1-2 dias)
    return 0.0;                                 // creada hoy
}

double calcularPrioridad(int semestre, const string& codigoMateria,
                         long fechaCreacion, const GrafoPrerrequisitos& grafo) {
    long ahora = static_cast<long>(time(nullptr));
    int dias = (fechaCreacion > 0 && ahora > fechaCreacion) ? static_cast<int>((ahora - fechaCreacion) / 86400) : 0;
    return PESO_SEMESTRE    * puntajeSemestre(semestre)
         + PESO_DEPENDENCIA * puntajeDependencia(grafo.contarDependientes(codigoMateria), grafo.maxDependientes())
         + PESO_ANTIGUEDAD  * puntajeAntiguedad(dias);
}
