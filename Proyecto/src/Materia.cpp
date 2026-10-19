// Materia.cpp
#include "../include/Materia.h"

// Todas las franjas empiezan sin horario
static void limpiarHorario(bool horario[DIAS_SEMANA][FRANJAS_DIA]) {
    for (int d = 0; d < DIAS_SEMANA; d++)
        for (int f = 0; f < FRANJAS_DIA; f++) horario[d][f] = false;
}

Materia::Materia() : creditos(0) {
    limpiarHorario(horario);
}

Materia::Materia(string codigo, string nombre, int creditos, string tipo, vector<string> prerrequisitos)
    : codigoMateria(codigo), nombre(nombre), creditos(creditos), tipo(tipo),
      prerrequisitos(prerrequisitos) {
    limpiarHorario(horario);
}

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