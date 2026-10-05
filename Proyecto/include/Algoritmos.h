// Algoritmos.h

// Algoritmos del proyecto:
//  - Busqueda secuencial y binaria (la binaria exige el vector ordenado por codigo)
//  - Ordenamiento basico (insercion) y avanzados (merge sort y quick sort, recursivos)
//  - Recursion sobre los prerrequisitos de una materia
// Las busquedas y ordenamientos cuentan sus comparaciones para poder compararlos.

#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include <string>
#include <vector>
#include "Materia.h"
#include "Solicitud.h"

using namespace std;

enum CriterioMateria { POR_CODIGO = 1, POR_NOMBRE = 2, POR_CREDITOS = 3 };

// Devuelven la posicion encontrada o -1 y las comparaciones
int buscarMateriaSecuencial(const vector<Materia>& materias, const string& codigo, long& comparaciones);
int buscarMateriaBinaria(const vector<Materia>& materias, const string& codigo, long& comparaciones);
int buscarSolicitudSecuencial(const vector<Solicitud>& solicitudes, int id);

// Ordenan segun el criterio y devuelven el numero de comparaciones
long ordenarInsercion(vector<Materia>& v, int criterio);
long ordenarMergeSort(vector<Materia>& v, int criterio);
long ordenarQuickSort(vector<Materia>& v, int criterio);

// Imprime los prerrequisitos de una materia
// Requiere 'materias' ordenado por codigo
void mostrarPrerrequisitos(const vector<Materia>& materias, const string& codigo, int nivel = 0);

// Genera un codigo nuevo y unico: 3 letras del nombre + consecutivo 
string generarCodigoMateria(const string& nombre, const vector<Materia>& materias);

#endif
