// Algoritmos.h

// Algoritmos del proyecto:
//  - Busqueda secuencial y binaria (la binaria exige el vector ordenado por codigo)
//  - Ordenamiento basico (insercion) y avanzados (merge sort y quick sort, recursivos)
//  - Ordenamiento de solicitudes por urgencia, antiguedad o materia (merge sort recursivo)
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

// NUEVO: criterios para ordenar solicitudes
enum CriterioSolicitud { SOL_POR_URGENCIA = 1, SOL_POR_ANTIGUEDAD = 2, SOL_POR_MATERIA = 3 };

// Devuelven la posicion encontrada o -1 y las comparaciones
int buscarMateriaSecuencial(const vector<Materia>& materias, const string& codigo, long& comparaciones);
int buscarMateriaBinaria(const vector<Materia>& materias, const string& codigo, long& comparaciones);
int buscarSolicitudSecuencial(const vector<Solicitud>& solicitudes, int id);

// Ordenan segun el criterio y devuelven el numero de comparaciones
long ordenarInsercion(vector<Materia>& v, int criterio);
long ordenarMergeSort(vector<Materia>& v, int criterio);
long ordenarQuickSort(vector<Materia>& v, int criterio);

// NUEVO: ordena solicitudes (merge sort recursivo) y devuelve el numero de comparaciones.
//  SOL_POR_URGENCIA:   mayor urgencia primero; a igual urgencia, la mas antigua
//  SOL_POR_ANTIGUEDAD: la mas antigua primero (id menor = llego antes)
//  SOL_POR_MATERIA:    por codigo de materia; dentro de cada materia, la mas antigua
long ordenarSolicitudes(vector<Solicitud>& v, int criterio);

// Imprime los prerrequisitos de una materia
// Requiere 'materias' ordenado por codigo
void mostrarPrerrequisitos(const vector<Materia>& materias, const string& codigo, int nivel = 0);

// Genera un codigo nuevo y unico
string generarCodigoMateria(const string& nombre, const vector<Materia>& materias);

//Horario de materias
string nombreDia(int dia);                                  // 0 = Lunes ... 5 = Sabado
string formatoHora(int franja);                             // 6 -> "9:00 AM"
vector<BloqueHorario> bloquesDeHorario(const Materia& m);   // une franjas consecutivas
string horarioATexto(const Materia& m);                     // para guardar: "3@6-11|0@2-4"
void textoAHorario(const string& texto, Materia& m);        // inverso: llena el horario
void mostrarMatrizHorario(const Materia& m);               // cuadricula dias x franjas
void mostrarHorarioMateria(const vector<Materia>& materias, const string& codigo);  // "Jueves, 9:00 AM - 11:30 AM"

#endif
