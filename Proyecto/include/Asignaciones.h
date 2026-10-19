// Asignaciones.h
// Asignacion de materias y solicitudes a monitores.
// PILA: permite deshacer la ultima asignacion mediante (LIFO).
// BITACORA: conserva el registro de cada asignacion y reversion.
#ifndef ASIGNACIONES_H
#define ASIGNACIONES_H

#include <iostream>
#include <string>
#include <vector>
#include "Usuario.h"
#include "Solicitud.h"

using namespace std;

// Registro de un cambio de asignacion (lo que guarda la pila)
//  idSolicitud == 0 -> solo cambio la materia del monitor
//  idSolicitud  > 0 -> se le asigno esa solicitud (ocupa la franja del monitor)
struct Asignacion {
    Monitor* monitor;
    string materiaAnterior;
    string materiaNueva;
    int idSolicitud;
};

// PILA propia
class PilaAsignaciones {
private:
    vector<Asignacion> elementos;

public:
    void push(const Asignacion& a) { elementos.push_back(a); }
    void pop() { if (!elementos.empty()) elementos.pop_back(); }
    Asignacion top() const { return elementos.back(); }   // se llama solo si !isEmpty()
    bool isEmpty() const { return elementos.empty(); }
};

// Bitacora: solo se agregan registros, deshacer NO borra nada de aqui
class BitacoraAsignaciones {
private:
    vector<string> registros;

public:
    void registrar(const string& texto) { registros.push_back(texto); }
    int tamano() const { return static_cast<int>(registros.size()); }
    void mostrar() const;
};

// Asigna una materia a un monitor y apila el cambio
void asignarMateriaAMonitor(Monitor& mon, const string& codigoMateria,
                            PilaAsignaciones& historial, BitacoraAsignaciones& bitacora);

// Asigna una solicitud pendiente a un monitor libre que atiende esa materia.
// La saca de la cola, la marca "Asignada" y ocupa la franja del monitor. Devuelve false si no se pudo.
bool asignarSolicitudAMonitor(Monitor& mon, int idSolicitud, vector<Solicitud>& solicitudes,
                              ColaSolicitudes& cola, PilaAsignaciones& historial,
                              BitacoraAsignaciones& bitacora);

// Desapila la ultima asignacion y la revierte:
//  - de materia: restaura la materia anterior del monitor
//  - de solicitud: la solicitud vuelve a la cola como "Pendiente" y se libera la franja del monitor
void deshacerAsignacion(PilaAsignaciones& historial, vector<Solicitud>& solicitudes,
                        ColaSolicitudes& cola, BitacoraAsignaciones& bitacora);

#endif