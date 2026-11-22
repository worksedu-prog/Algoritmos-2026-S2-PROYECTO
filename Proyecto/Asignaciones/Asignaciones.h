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

    // HU-35: al reasignar una solicitud, las entradas de la pila que la referencian pasan
    // al nuevo monitor, asi deshacerla despues libera al monitor correcto
    void actualizarMonitor(int idSolicitud, Monitor* nuevo) {
        for (size_t i = 0; i < elementos.size(); i++)
            if (elementos[i].idSolicitud == idSolicitud) elementos[i].monitor = nuevo;
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

// Duracion de una sesion de monitoria: 2 franjas de 30 min = 1 hora
const int DURACION_SESION = 2;

// HU-09.03: busca el primer bloque de DURACION_SESION que
//   - cabe en la disponibilidad del monitor,
//   - cabe en la disponibilidad del estudiante (si el estudiante no registro disponibilidad,
//     solo se usa la del monitor),
//   - no se cruza con la clase de la materia (el estudiante esta en clase),
//   - no se cruza con otra sesion asignada del mismo monitor o del mismo estudiante.
// 'idIgnorar' excluye una solicitud (la que se esta reasignando). O(F * S), F franjas, S solicitudes.
bool buscarBloqueComun(const Monitor& mon, const Estudiante* est, const string& codigoMateria,
                       const vector<Solicitud>& solicitudes, const vector<Materia>& materias,
                       int idIgnorar, BloqueHorario& resultado);

// Asigna una solicitud pendiente a un monitor libre que atiende esa materia y con horario compatible.
// La saca de la cola, la marca "Asignada", guarda el bloque de la sesion y ocupa al monitor.
// Si no se pudo, la solicitud sigue pendiente y devuelve false.
bool asignarSolicitudAMonitor(Monitor& mon, int idSolicitud, vector<Solicitud>& solicitudes,
                              ColaSolicitudes& cola, PilaAsignaciones& historial,
                              BitacoraAsignaciones& bitacora,
                              const vector<Usuario*>& usuarios, const vector<Materia>& materias);

// HU-09.01/02: busca automaticamente un monitor activo, libre, que domine la materia y con
// horario compatible. Si no hay ninguno, la solicitud sigue pendiente.
bool emparejarSolicitud(int idSolicitud, vector<Solicitud>& solicitudes, ColaSolicitudes& cola,
                        PilaAsignaciones& historial, BitacoraAsignaciones& bitacora,
                        vector<Usuario*>& usuarios, const vector<Materia>& materias);

// HU-35: cambia el monitor de una solicitud ya asignada sin deshacer asignaciones posteriores
bool reasignarSolicitud(int idSolicitud, Monitor& nuevo, vector<Solicitud>& solicitudes,
                        PilaAsignaciones& historial, BitacoraAsignaciones& bitacora,
                        vector<Usuario*>& usuarios, const vector<Materia>& materias);

// HU-21.04: franjas de disponibilidad del monitor que no se cruzan con la clase de la materia
int franjasLibresDeClase(const Monitor& mon, const Materia& materia);

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