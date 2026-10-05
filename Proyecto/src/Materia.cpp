// Materia.cpp
#include "../include/Materia.h"

Materia::Materia(string codigo, string nombre, int creditos, string tipo, vector<string> prerrequisitos)
    : codigoMateria(codigo), nombre(nombre), creditos(creditos), tipo(tipo),
      prerrequisitos(prerrequisitos) {}

void Materia::mostrarInfo() const {
    cout << "[" << codigoMateria << "] " << nombre << " - " << creditos << " creditos (" << tipo << ")";
    if (!prerrequisitos.empty()) {
        cout << " | Prerrequisitos: ";
        for (int i = 0; i < prerrequisitos.size(); i++) {
            if (i > 0) cout << ", ";
            cout << prerrequisitos[i];
        }
    }
    cout << endl;
}
void Materia::mostrarCodigo() const {
    cout << "[" << codigoMateria << "] " << nombre << endl;
}