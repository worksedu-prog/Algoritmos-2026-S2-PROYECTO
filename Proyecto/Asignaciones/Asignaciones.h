// Asignaciones.h
// Asignacion de materias y solicitudes a monitores.
// PILA: permite deshacer la ultima asignacion mediante (LIFO).
// BITACORA: conserva el registro de cada asignacion y reversion.
#ifndef ASIGNACIONES_H
#define ASIGNACIONES_H

#include <iostream>
#include <string>
#include <vector>
#include "../Usuario/Usuario.h"
#include "../Solicitud/Solicitud.h"

using namespace std;

// Registro de un cambio de asignacion (lo que guarda la pila)
//  idSolicitud == 0 -> solo cambio la materia del monitor
//  idSolicitud  > 0 -> se le asigno esa solicitud (ocupa la franja del monitor)
struct Asignacion {
    Monitor* monitor;
    //Se elimina el campo string materiaAnterior porque la lista ya no reemplaza materias
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

    // Materias que se le asignaron a 'mon' y se pueden revertir, de la mas reciente a la mas antigua
    vector<string> materiasRevertibles(const Monitor* mon) const {   //Lista las materias que se pueden elegir al revertir
        vector<string> codigos;   // (Nuevo: resultado)
        for (int i = static_cast<int>(elementos.size()) - 1; i >= 0; i--) {   // Recorre de la cima hacia el fondo
            if (elementos[i].monitor == mon && elementos[i].idSolicitud == 0) codigos.push_back(elementos[i].materiaNueva);   // Solo asignaciones de materia de ese monitor
        }
        return codigos;
    }

    // Quita de la pila el registro mas reciente de esa materia para ese monitor (mantiene el orden de los demas)
    bool eliminarMateria(const Monitor* mon, const string& codigo) {   // Mantiene la pila consistente al revertir una materia que no esta en la cima
        for (int i = static_cast<int>(elementos.size()) - 1; i >= 0; i--) {   // Busca desde la cima
            if (elementos[i].monitor == mon && elementos[i].idSolicitud == 0 && elementos[i].materiaNueva == codigo) {
                elementos.erase(elementos.begin() + i);
                return true;
            }
        }
        return false;
    }
};



// Bitacora: solo se agregan registros, deshacer NO borra nada de aqui
class BitacoraAsignaciones {
private:
    vector<string> registros;

public:
    void registrar(const string& texto) { registros.push_back(texto); }
    int tamano() const { return static_cast<int>(registros.size()); }
    const vector<string>& getRegistros() const { return registros; }
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

// Revierte una materia elegida de un monitor (no tiene por que ser necesariamente la ultima asignada).
// Devuelve false si no se pudo
bool deshacerMateriaElegida(Monitor& mon, const string& codigoMateria, vector<Solicitud>& solicitudes,
                            PilaAsignaciones& historial, BitacoraAsignaciones& bitacora);

#endif