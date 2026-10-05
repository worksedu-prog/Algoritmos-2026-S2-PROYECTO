// Materia.h
// Una materia con su codigo, creditos, tipo y la lista de codigos de sus prerrequisitos.
#ifndef MATERIA_H
#define MATERIA_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Materia {
private:
    string codigoMateria;
    string nombre;
    int creditos;
    string tipo;                     // "Obligatoria" o "Electiva"
    vector<string> prerrequisitos;   // codigos de otras materias

public:
    Materia() : creditos(0) {}
    Materia(string codigo, string nombre, int creditos, string tipo, vector<string> prerrequisitos = {});

    string getCodigo() const { return codigoMateria; }
    string getNombre() const { return nombre; }
    int getCreditos() const { return creditos; }
    string getTipo() const { return tipo; }
    const vector<string>& getPrerrequisitos() const { return prerrequisitos; }
    void mostrarCodigo() const;
    void mostrarInfo() const;
};

#endif
