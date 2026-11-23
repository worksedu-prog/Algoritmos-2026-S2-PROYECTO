// Solicitud.h
#ifndef SOLICITUD_H
#define SOLICITUD_H

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "../Materia/Materia.h"   // BloqueHorario

using namespace std;

// Niveles de urgencia
enum Urgencia { URGENCIA_BAJA = 1, URGENCIA_MEDIA = 2, URGENCIA_ALTA = 3 };

inline string nombreUrgencia(int urgencia) {
    if (urgencia == URGENCIA_ALTA) return "Alta";
    if (urgencia == URGENCIA_MEDIA) return "Media";
    return "Baja";
}

class Solicitud {
private:
    int id;                  // indica el orden de llegada
    long codigoEstudiante;
    string codigoMateria;
    int urgencia;
    string descripcion;
    string estado;           // "Pendiente", "Asignada", "Atendida" o "Cancelada"
    long fechaCreacion;      // segundos desde epoch: criterio 3 (antiguedad)
    double prioridad;        // 0..1, calculada con los 3 criterios (ver Grafo.h)
    // HU-09 / HU-23.03: monitor emparejado y bloque de la sesion (dia = -1: sin bloque).
    // TODO [dependencia compañero HU-33]: la clase Sesion puede construirse a partir de estos datos.
    long codigoMonitor;
    BloqueHorario bloque;

public:
    Solicitud() : id(0), codigoEstudiante(0), urgencia(URGENCIA_BAJA), estado("Pendiente"),
                  fechaCreacion(static_cast<long>(time(nullptr))), prioridad(0.0),
          codigoMonitor(0), bloque{-1, 0, 0} {}
    Solicitud(int id, long codigoEstudiante, const string& codigoMateria, int urgencia,
              const string& descripcion, const string& estado = "Pendiente")
        : id(id), codigoEstudiante(codigoEstudiante), codigoMateria(codigoMateria),
          urgencia(urgencia), descripcion(descripcion), estado(estado),
          fechaCreacion(static_cast<long>(time(nullptr))), prioridad(0.0),
          codigoMonitor(0), bloque{-1, 0, 0} {}

    int getId() const { return id; }
    long getCodigoEstudiante() const { return codigoEstudiante; }
    string getCodigoMateria() const { return codigoMateria; }
    int getUrgencia() const { return urgencia; }
    string getDescripcion() const { return descripcion; }
    string getEstado() const { return estado; }
    void setEstado(const string& nuevo) { estado = nuevo; }
    long getFechaCreacion() const { return fechaCreacion; }
    void setFechaCreacion(long f) { fechaCreacion = f; }
    double getPrioridad() const { return prioridad; }
    long getCodigoMonitor() const { return codigoMonitor; }
    const BloqueHorario& getBloque() const { return bloque; }
    bool tieneBloque() const { return bloque.dia >= 0; }
    void setSesion(long monitor, const BloqueHorario& b) { codigoMonitor = monitor; bloque = b; }
    void limpiarSesion() { codigoMonitor = 0; bloque = BloqueHorario{-1, 0, 0}; }
    // Guarda la prioridad y deriva de ella el nivel de urgencia que se muestra y persiste
    void setPrioridad(double p) {
        prioridad = p;
        urgencia = (p >= 0.66) ? URGENCIA_ALTA : (p >= 0.33) ? URGENCIA_MEDIA : URGENCIA_BAJA;
    }

    void mostrarInfo() const {
        cout << "[Solicitud #" << id << "] Estudiante: " << codigoEstudiante
             << " | Materia: " << codigoMateria
             << " | Urgencia: " << nombreUrgencia(urgencia) << " (" << prioridad << ")"
             << " | Estado: " << estado << endl;
        // TODO [dependencia compañero HU-06.03]: mostrar aqui el monitor y el horario de la sesion
        // (disponibles con getCodigoMonitor() y getBloque()).
        cout << "  Detalle: " << descripcion << endl;
    }
};

// Elemento de la cola: el id y la prioridad calculada (3 criterios)
struct ItemCola {
    int id;
    double prioridad;
};

// COLA con prioridad, implementada con un vector ordenado por urgencia (de mayor a menor)
class ColaSolicitudes {
private:
    vector<ItemCola> elementos;

public:
    // Mayor prioridad primero; a igual urgencia, el id menor (la mas antigua) primero.
    // Compara tambien por id hace que una solicitud reencolada (al deshacer
    // una asignacion) recupere su lugar por antiguedad y no quede al final de su urgencia.
    void encolar(int id, double prioridad) {
        size_t pos = 0;
        while (pos < elementos.size() &&
               (elementos[pos].prioridad > prioridad ||
                (elementos[pos].prioridad == prioridad && elementos[pos].id < id))) pos++;
        elementos.insert(elementos.begin() + pos, ItemCola{id, prioridad});
    }

    void vaciar() { elementos.clear(); }   // para reconstruir la cola al recalcular prioridades

    // Id de la solicitud que sigue, o -1 si la cola esta vacia
    int frente() const { return elementos.empty() ? -1 : elementos.front().id; }

    void desencolar() { if (!elementos.empty()) elementos.erase(elementos.begin()); }

    // Saca una solicitud de cualquier posicion para cancelarla
    void eliminar(int id) {
        for (size_t i = 0; i < elementos.size(); i++) {
            if (elementos[i].id == id) { elementos.erase(elementos.begin() + i); return; }
        }
    }

    bool estaVacia() const { return elementos.empty(); }
    int tamano() const { return static_cast<int>(elementos.size()); }
    const vector<ItemCola>& getElementos() const { return elementos; }
};

// ALGORITMOS DE SOLICITUD: Busqueda y Ordenamiento
// Criterios para ordenar solicitudes
enum CriterioSolicitud { SOL_POR_URGENCIA = 1, SOL_POR_ANTIGUEDAD = 2, SOL_POR_MATERIA = 3 };

// Devuelve la posicion encontrada o -1
int buscarSolicitudSecuencial(const vector<Solicitud>& solicitudes, int id);

long ordenarSolicitudes(vector<Solicitud>& v, int criterio);

#endif
