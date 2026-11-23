// Jerarquia.cpp
#include "Jerarquia.h"
#include <iostream>
#include <cctype>

using namespace std;

// Comparacion entre hermanos: niveles numericamente, lo demas alfabeticamente
static bool vaAntes(int tipo, const string& a, const string& b) {
    if (tipo == NODO_NIVEL) return stoi(a) < stoi(b);
    return a < b;
}

// Pasa un texto a minusculas para comparar areas sin distinguir mayusculas
static string enMinusculasArea(const string& texto) {   // Auxiliar para unir areas que solo difieren en mayusculas
    string resultado = texto;   // (Nuevo: copia del texto)
    for (size_t i = 0; i < resultado.size(); i++)   // Recorre cada caracter
        resultado[i] = static_cast<char>(tolower(static_cast<unsigned char>(resultado[i])));   // Lo pasa a minuscula
    return resultado;   // Devuelve el texto en minusculas
} 

JerarquiaMaterias::JerarquiaMaterias(const vector<Materia>& materias)
    : raiz(new NodoJerarquia(NODO_RAIZ, "Catalogo")) {
    for (size_t i = 0; i < materias.size(); i++)
        insertar(materias[i].getArea(), materias[i].getNivel(), materias[i].getCodigo(), materias[i].getNombre());
}

// Busca entre los hijos de 'padre' el de clave dada; si no existe lo inserta en orden
NodoJerarquia* JerarquiaMaterias::buscarOCrearHijo(NodoJerarquia* padre, int tipo,
                                                   const string& clave, const string& detalle) {
    NodoJerarquia* anterior = nullptr;
    NodoJerarquia* actual = padre->hijo;
    while (actual && vaAntes(tipo, actual->clave, clave)) { anterior = actual; actual = actual->hermano; }
    if (actual && actual->clave == clave) return actual;
    NodoJerarquia* nuevo = new NodoJerarquia(tipo, clave, detalle);
    nuevo->hermano = actual;
    if (anterior) anterior->hermano = nuevo;
    else padre->hijo = nuevo;
    return nuevo;
}

void JerarquiaMaterias::insertar(const string& area, int nivel, const string& codigo, const string& nombre) {
    string nombreArea = area.empty() ? "General" : area;   // Nombre del area a usar
    NodoJerarquia* nodoArea = buscarAreaExistente(nombreArea);
    if (nodoArea == nullptr) nodoArea = buscarOCrearHijo(raiz, NODO_AREA, nombreArea);
    NodoJerarquia* nodoNivel = buscarOCrearHijo(nodoArea, NODO_NIVEL, to_string(nivel < 1 ? 1 : nivel));
    buscarOCrearHijo(nodoNivel, NODO_MATERIA, codigo, nombre);   // codigo repetido no se duplica
}

void JerarquiaMaterias::liberar(NodoJerarquia* nodo) {
    if (!nodo) return;
    liberar(nodo->hijo);
    liberar(nodo->hermano);
    delete nodo;
}

int JerarquiaMaterias::contarMaterias(const NodoJerarquia* nodo) const {
    if (!nodo) return 0;
    int propias = (nodo->tipo == NODO_MATERIA) ? 1 : 0;
    int enHijos = 0;
    for (const NodoJerarquia* h = nodo->hijo; h; h = h->hermano) enHijos += contarMaterias(h);
    return propias + enHijos;
}

void JerarquiaMaterias::mostrarRec(const NodoJerarquia* nodo, int profundidad) const {
    // Caso base: no hay nodo. Caso recursivo: imprime el nodo y baja a cada hijo.
    if (!nodo) return;
    string sangria(profundidad * 2, ' ');
    if (nodo->tipo == NODO_AREA)
        cout << sangria << "Area: " << nodo->clave << " (" << contarMaterias(nodo) << " materias)" << endl;
    else if (nodo->tipo == NODO_NIVEL)
        cout << sangria << "Nivel " << nodo->clave << endl;
    else if (nodo->tipo == NODO_MATERIA)
        cout << sangria << "- [" << nodo->clave << "] " << nodo->detalle << endl;
    for (const NodoJerarquia* h = nodo->hijo; h; h = h->hermano) mostrarRec(h, profundidad + 1);
}

void JerarquiaMaterias::mostrar() const {
    if (!raiz->hijo) { cout << "No hay materias registradas." << endl; return; }
    cout << "Jerarquia Area -> Nivel -> Materia" << endl;
    for (const NodoJerarquia* h = raiz->hijo; h; h = h->hermano) mostrarRec(h, 0);
}

vector<string> JerarquiaMaterias::areas() const {
    vector<string> lista;
    for (const NodoJerarquia* h = raiz->hijo; h; h = h->hermano) lista.push_back(h->clave);
    return lista;
}

// Busca un area entre las hijas de la raiz ignorando mayusculas
NodoJerarquia* JerarquiaMaterias::buscarAreaExistente(const string& area) const {   //Localiza el nodo de un area
    string buscada = enMinusculasArea(area);
    for (NodoJerarquia* h = raiz->hijo; h; h = h->hermano) {
        if (enMinusculasArea(h->clave) == buscada) return h;
    }
    return nullptr;
}

bool JerarquiaMaterias::existeArea(const string& area) const {
    return buscarAreaExistente(area) != nullptr;
}

// Caso base: no hay nodo. Caso recursivo: si es una materia la agrega y baja a cada hijo (preorden: niveles de menor a mayor)
void JerarquiaMaterias::recolectarMaterias(const NodoJerarquia* nodo, vector<string>& codigos) const {
    if (!nodo) return;
    if (nodo->tipo == NODO_MATERIA) codigos.push_back(nodo->clave);
    for (const NodoJerarquia* h = nodo->hijo; h; h = h->hermano) recolectarMaterias(h, codigos);
}

vector<string> JerarquiaMaterias::materiasDeArea(const string& area) const {   // Devuelve todas las materias que cuelgan de un area
    vector<string> codigos;
    const NodoJerarquia* nodoArea = buscarAreaExistente(area);
    if (nodoArea != nullptr) recolectarMaterias(nodoArea, codigos);
    return codigos;
}

bool JerarquiaMaterias::mostrarArea(const string& area) const {   //Imprime el area con sus niveles y materias
    const NodoJerarquia* nodoArea = buscarAreaExistente(area);
    if (nodoArea == nullptr) return false;
    mostrarRec(nodoArea, 0);
    return true;
}