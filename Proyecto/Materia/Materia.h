// Materia.h
// Una materia con su codigo, creditos, tipo y la lista de codigos de sus prerrequisitos.
#ifndef MATERIA_H
#define MATERIA_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Horario semanal de una materia: matriz horario[dia][franja]
// Filas = dias (0 = Lunes ... 5 = Sabado) columnas = franjas de 30 minutos.
// La franja 0 empieza a las HORA_INICIO_DIA:00 y la ultima termina a las 8:00 PM.
const int DIAS_SEMANA = 6;       // Lunes a Sabado
const int FRANJAS_DIA = 28;      // 6:00 AM - 8:00 PM en bloques de 30 minutos
const int HORA_INICIO_DIA = 6;

// Un bloque continuo de clase: dia, franja de inicio y franja de fin 
struct BloqueHorario {
    int dia;
    int inicio;
    int fin;
};

class Materia {
private:
    string codigoMateria;
    string nombre;
    int creditos;
    string tipo;                     // "Obligatoria" o "Electiva"
    vector<string> prerrequisitos;   // codigos de otras materias
    bool horario[DIAS_SEMANA][FRANJAS_DIA];   // true = franja ocupada
public:
    Materia();
    Materia(string codigo, string nombre, int creditos, string tipo,
            vector<string> prerrequisitos = {});

    string getCodigo() const { return codigoMateria; }
    string getNombre() const { return nombre; }
    int getCreditos() const { return creditos; }
    string getTipo() const { return tipo; }
    const vector<string>& getPrerrequisitos() const { return prerrequisitos; }
    bool franjaOcupada(int dia, int franja) const { return horario[dia][franja]; }
    void ocuparFranja(int dia, int franja) { horario[dia][franja] = true; }
    void mostrarCodigo() const;
    void mostrarInfo() const;
};

#endif
