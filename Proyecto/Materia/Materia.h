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

// Criterios para ordenar materias
enum CriterioMateria { POR_CODIGO = 1, POR_NOMBRE = 2, POR_CREDITOS = 3 };

// Devuelven la posicion encontrada o -1 y las comparaciones
int buscarMateriaSecuencial(const vector<Materia>& materias, const string& codigo, long& comparaciones);
int buscarMateriaBinaria(const vector<Materia>& materias, const string& codigo, long& comparaciones);

// Ordenan segun el criterio y devuelven el numero de comparaciones
long ordenarInsercion(vector<Materia>& v, int criterio);
long ordenarMergeSort(vector<Materia>& v, int criterio);
long ordenarQuickSort(vector<Materia>& v, int criterio);

// Imprime los prerrequisitos de una materia
// Requiere 'materias' ordenado por codigo
void mostrarPrerrequisitos(const vector<Materia>& materias, const string& codigo, int nivel = 0);

// Genera un codigo nuevo y unico
string generarCodigoMateria(const string& nombre, const vector<Materia>& materias);

// Horario de materias
string nombreDia(int dia);                                  // 0 = Lunes ... 5 = Sabado
string formatoHora(int franja);                             // 6 -> "9:00 AM"
vector<BloqueHorario> bloquesDeHorario(const Materia& m);   // une franjas consecutivas
string horarioATexto(const Materia& m);                     // para guardar: "3@6-11|0@2-4"
void textoAHorario(const string& texto, Materia& m);        // inverso: llena el horario
void mostrarMatrizHorario(const Materia& m);               // cuadricula dias x franjas
void mostrarHorarioMateria(const vector<Materia>& materias, const string& codigo);  // "Jueves, 9:00 AM - 11:30 AM"

#endif
