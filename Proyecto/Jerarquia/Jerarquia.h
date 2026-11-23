// Jerarquia.h
// Organiza las materias en la jerarquia AREA -> NIVEL -> MATERIA.
// cada nodo apunta a su primer hijo y a su siguiente hermano. Los hermanos se mantienen
// ordenados (areas alfabeticamente, niveles de menor a mayor, materias por codigo), asi
// el recorrido recursivo en preorden imprime la jerarquia ya ordenada.
#ifndef JERARQUIA_H
#define JERARQUIA_H

#include <string>
#include <vector>
#include "../Materia/Materia.h"

using namespace std;

enum TipoNodo { NODO_RAIZ = 0, NODO_AREA = 1, NODO_NIVEL = 2, NODO_MATERIA = 3 };

struct NodoJerarquia {
    int tipo;
    string clave;            // nombre del area, numero de nivel o codigo de materia
    string detalle;          // nombre de la materia 
    NodoJerarquia* hijo;     // primer hijo
    NodoJerarquia* hermano;  // siguiente hermano
    NodoJerarquia(int t, const string& c, const string& d = "")
        : tipo(t), clave(c), detalle(d), hijo(nullptr), hermano(nullptr) {}
};

class JerarquiaMaterias {
private:
    NodoJerarquia* raiz;
    NodoJerarquia* buscarOCrearHijo(NodoJerarquia* padre, int tipo, const string& clave, const string& detalle = "");
    void liberar(NodoJerarquia* nodo);                                  // postorden recursivo
    void mostrarRec(const NodoJerarquia* nodo, int profundidad) const;  // preorden recursivo
    int contarMaterias(const NodoJerarquia* nodo) const;                // recursivo
    NodoJerarquia* buscarAreaExistente(const string& area) const;   // Busca un area entre las hijas de la raiz ignorando mayusculas; nullptr si no existe
    void recolectarMaterias(const NodoJerarquia* nodo, vector<string>& codigos) const;   // Recorrido recursivo en preorden que junta los codigos de las materias de un subarbol

public:
    JerarquiaMaterias() : raiz(new NodoJerarquia(NODO_RAIZ, "Catalogo")) {}
    explicit JerarquiaMaterias(const vector<Materia>& materias);
    ~JerarquiaMaterias() { liberar(raiz); }
    JerarquiaMaterias(const JerarquiaMaterias&) = delete;               // evita doble liberacion
    JerarquiaMaterias& operator=(const JerarquiaMaterias&) = delete;

    void insertar(const string& area, int nivel, const string& codigo, const string& nombre);
    void mostrar() const;                                  // arbol completo
    vector<string> areas() const;                          // areas en orden alfabetico
    bool existeArea(const string& area) const;   // True si el area existe, sin distinguir mayusculas
    vector<string> materiasDeArea(const string& area) const;   // Codigos de todas las materias del area, de menor a mayor nivel; vacia si el area no existe
    bool mostrarArea(const string& area) const;   // Imprime el area con sus niveles y materias; false si el area no existe
};

#endif
