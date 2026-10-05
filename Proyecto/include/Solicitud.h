// Solicitud.h
// Solicitud de monitoria y la COLA propia que decide cual se atiende primero.
#ifndef SOLICITUD_H
#define SOLICITUD_H

#include <iostream>
#include <string>
#include <vector>

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
    int id;                  //indica el orden de llegada
    long codigoEstudiante;
    string codigoMateria;
    int urgencia;
    string descripcion;
    string estado;           // "Pendiente", "Atendida" o "Cancelada"

public:
    Solicitud() : id(0), codigoEstudiante(0), urgencia(URGENCIA_BAJA), estado("Pendiente") {}
    Solicitud(int id, long codigoEstudiante, const string& codigoMateria, int urgencia,
              const string& descripcion, const string& estado = "Pendiente")
        : id(id), codigoEstudiante(codigoEstudiante), codigoMateria(codigoMateria),
          urgencia(urgencia), descripcion(descripcion), estado(estado) {}

    int getId() const { return id; }
    long getCodigoEstudiante() const { return codigoEstudiante; }
    string getCodigoMateria() const { return codigoMateria; }
    int getUrgencia() const { return urgencia; }
    string getDescripcion() const { return descripcion; }
    string getEstado() const { return estado; }
    void setEstado(const string& nuevo) { estado = nuevo; }

    void mostrarInfo() const {
        cout << "[Solicitud #" << id << "] Estudiante: " << codigoEstudiante
             << " | Materia: " << codigoMateria
             << " | Urgencia: " << nombreUrgencia(urgencia)
             << " | Estado: " << estado << endl;
        cout << "  Detalle: " << descripcion << endl;
    }
};

// Elemento de la cola el id y la urgencia
struct ItemCola {
    int id;
    int urgencia;
};

// COLA con prioridad, implementada con un vector ordenado por urgencia (de mayor a menor)
class ColaSolicitudes {
private:
    vector<ItemCola> elementos;

public:
    void encolar(int id, int urgencia) {
        int pos = 0;
        while (pos < elementos.size() && elementos[pos].urgencia >= urgencia) pos++;
        elementos.insert(elementos.begin() + pos, ItemCola{id, urgencia});
    }

    // Id de la solicitud que sigue, o -1 si la cola esta vacia
    int frente() const { return elementos.empty() ? -1 : elementos.front().id; }

    void desencolar() { if (!elementos.empty()) elementos.erase(elementos.begin()); }

    // Saca una solicitud de cualquier posicion para cancelarla
    void eliminar(int id) {
        for (int i = 0; i < elementos.size(); i++) {
            if (elementos[i].id == id) { elementos.erase(elementos.begin() + i); return; }
        }
    }

    bool estaVacia() const { return elementos.empty(); }
    int tamano() const { return static_cast<int>(elementos.size()); }
    const vector<ItemCola>& getElementos() const { return elementos; }
};

#endif
