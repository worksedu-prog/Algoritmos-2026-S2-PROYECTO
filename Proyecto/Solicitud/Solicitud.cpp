// Solicitud.cpp
#include "Solicitud.h"
#include <algorithm>

using namespace std;

// Busqueda secuencial en solicitudes O(n)
int buscarSolicitudSecuencial(const vector<Solicitud>& solicitudes, int id) {
    for (size_t i = 0; i < solicitudes.size(); i++) {
        if (solicitudes[i].getId() == id) return static_cast<int>(i);
    }
    return -1;
}

// Ordenamiento de SOLICITUDES (merge sort recursivo, O(n log n), estable)
static bool solicitudMenorQue(const Solicitud& a, const Solicitud& b, int criterio, long& comparaciones) {
    comparaciones++;
    if (criterio == SOL_POR_URGENCIA && a.getPrioridad() != b.getPrioridad())
        return a.getPrioridad() > b.getPrioridad();                     // mayor prioridad primero
    if (criterio == SOL_POR_MATERIA && a.getCodigoMateria() != b.getCodigoMateria())
        return a.getCodigoMateria() < b.getCodigoMateria();             // orden alfabetico por codigo
    return a.getId() < b.getId();                                       // antiguedad (y desempate)
}

static void mezclarSolicitudes(vector<Solicitud>& v, int izq, int medio, int der, int criterio, long& comparaciones) {
    vector<Solicitud> izquierda(v.begin() + izq, v.begin() + medio + 1);
    vector<Solicitud> derecha(v.begin() + medio + 1, v.begin() + der + 1);
    size_t i = 0, j = 0;
    int k = izq;
    while (i < izquierda.size() && j < derecha.size()) {
        if (solicitudMenorQue(derecha[j], izquierda[i], criterio, comparaciones)) v[k++] = derecha[j++];
        else v[k++] = izquierda[i++];
    }
    while (i < izquierda.size()) v[k++] = izquierda[i++];
    while (j < derecha.size()) v[k++] = derecha[j++];
}

static void mergeSortSolicitudesRec(vector<Solicitud>& v, int izq, int der, int criterio, long& comparaciones) {
    if (izq >= der) return;                       // caso base: 0 o 1 elemento
    int medio = izq + (der - izq) / 2;
    mergeSortSolicitudesRec(v, izq, medio, criterio, comparaciones);
    mergeSortSolicitudesRec(v, medio + 1, der, criterio, comparaciones);
    mezclarSolicitudes(v, izq, medio, der, criterio, comparaciones);
}

long ordenarSolicitudes(vector<Solicitud>& v, int criterio) {
    long comparaciones = 0;
    if (v.size() > 1) mergeSortSolicitudesRec(v, 0, static_cast<int>(v.size()) - 1, criterio, comparaciones);
    return comparaciones;
}

// HU-19: suma una solicitud al resumen segun su estado
static void contarEstado(ResumenCobertura& resumen, const string& estado) {   // Auxiliar que cuenta por estado
    if (estado == "Pendiente") resumen.pendientes++;
    else if (estado == "Asignada") resumen.asignadas++;
    else if (estado == "Atendida") resumen.atendidas++;
    else if (estado == "Cancelada") resumen.canceladas++;
}

ResumenCobertura resumirCobertura(const vector<Solicitud>& solicitudes) {
    ResumenCobertura resumen = {"", 0, 0, 0, 0};
    for (size_t i = 0; i < solicitudes.size(); i++) contarEstado(resumen, solicitudes[i].getEstado());
    return resumen;
}

vector<ResumenCobertura> resumirCoberturaPorMateria(const vector<Solicitud>& solicitudes) {   // Resumen por materia
    vector<Solicitud> copia = solicitudes;
    ordenarSolicitudes(copia, SOL_POR_MATERIA);
    vector<ResumenCobertura> resumenes;
    for (size_t i = 0; i < copia.size(); i++) {   //Recorre las solicitudes ya agrupadas)
        if (resumenes.empty() || resumenes.back().codigoMateria != copia[i].getCodigoMateria())
            resumenes.push_back(ResumenCobertura{copia[i].getCodigoMateria(), 0, 0, 0, 0});
        contarEstado(resumenes.back(), copia[i].getEstado());
    }
    return resumenes; 
}

double porcentajeCobertura(const ResumenCobertura& resumen) {
    int vigentes = resumen.pendientes + resumen.asignadas + resumen.atendidas;
    if (vigentes == 0) return 0.0;
    return resumen.atendidas * 100.0 / vigentes;
}