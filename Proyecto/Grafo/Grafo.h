// Grafo.h
// Manejo de Red de prerrequisitos entre materias 
// Cada materia es un vertice y existe una arista A -> B cuando A es prerrequisito de B
#ifndef GRAFO_H
#define GRAFO_H

#include <string>
#include <vector>
#include "../Materia/Materia.h"
#include "../Solicitud/Solicitud.h"

using namespace std;

class GrafoPrerrequisitos {
private:
    vector<string> codigos;          // vertice i -> codigo de materia (ordenado por codigo)
    vector<string> nombres;          // vertice i -> nombre de la materia
    vector<vector<int>> lista;       // lista[i] = materias que dependen directamente de i

public:
    GrafoPrerrequisitos() {}
    explicit GrafoPrerrequisitos(const vector<Materia>& materias) { construir(materias); }

    // Arma la lista de adyacencia a partir del catalogo. Los prerrequisitos que no
    // existen en el catalogo se ignoran (no hay vertice al cual unirlos).
    void construir(const vector<Materia>& materias);

    int numVertices() const { return static_cast<int>(codigos.size()); }
    int indiceDe(const string& codigo) const;    // busqueda binaria O(log V), -1 si no existe

    // BFS desde 'inicio' 
    void bfs(int inicio, vector<int>& orden, vector<int>& dist, vector<int>& padre) const;

    // Cuantas materias dependen directa o indirectamente de 'codigo'
    int contarDependientes(const string& codigo) const;
    int maxDependientes() const;                 // maximo de contarDependientes en todo el grafo

    // Materias afectadas (directa o indirectamente) si se reprueba o retira 'codigo'.
    // Devuelve los codigos en orden BFS; lista vacia si ninguna depende de ella.
    vector<string> materiasAfectadas(const string& codigo) const;
    // Igual, pero impresa por niveles con el motivo ("reprobar" o "retirar")
    void mostrarMateriasAfectadas(const string& codigo, const string& motivo = "reprobar") const;
    // Camino de prerrequisitos mas corto origen -> destino reconstruido con el arreglo padre
    void mostrarCadena(const string& origen, const string& destino) const;

    bool tieneCiclo() const;
    void mostrarRed() const;
};

// PRIORIDAD DE SOLICITUDES (3 criterios normalizados en [0,1], pesos suman 1)
const double PESO_SEMESTRE    = 0.30;   // criterio 1
const double PESO_DEPENDENCIA = 0.40;   // criterio 2 
const double PESO_ANTIGUEDAD  = 0.30;   // criterio 3

double puntajeSemestre(int semestre);                         // 1 si semestre 1-2 u 8-9, 0 si intermedio
double puntajeDependencia(int dependientes, int maxDep);      // dependientes / maxDep
double puntajeAntiguedad(int diasEspera);                     // tramos: 1-2 dias baja, 4 media, >=7 maxima
double calcularPrioridad(int semestre, const string& codigoMateria,
                         long fechaCreacion, const GrafoPrerrequisitos& grafo);

#endif
