// Algoritmos.cpp
#include "../include/Algoritmos.h"
#include <algorithm>
#include <cctype>

// Dice si 'a' va antes que 'b' segun el criterio (si empatan, desempata por codigo)
static bool menorQue(const Materia& a, const Materia& b, int criterio, long& comparaciones) {
    comparaciones++;
    if (criterio == POR_NOMBRE && a.getNombre() != b.getNombre()) return a.getNombre() < b.getNombre();
    if (criterio == POR_CREDITOS && a.getCreditos() != b.getCreditos()) return a.getCreditos() < b.getCreditos();
    return a.getCodigo() < b.getCodigo();
}

// Busquedas: secuencial O(n), binaria O(log n)

int buscarMateriaSecuencial(const vector<Materia>& materias, const string& codigo, long& comparaciones) {
    for (int i = 0; i < materias.size(); i++) {
        comparaciones++;
        if (materias[i].getCodigo() == codigo) return static_cast<int>(i);
    }
    return -1;
}

int buscarMateriaBinaria(const vector<Materia>& materias, const string& codigo, long& comparaciones) {
    int bajo = 0, alto = static_cast<int>(materias.size()) - 1;
    while (bajo <= alto) {
        int medio = bajo + (alto - bajo) / 2;
        comparaciones++;
        const string actual = materias[medio].getCodigo();
        if (actual == codigo) return medio;
        if (actual < codigo) bajo = medio + 1;
        else alto = medio - 1;
    }
    return -1;
}

int buscarSolicitudSecuencial(const vector<Solicitud>& solicitudes, int id) {
    for (int i = 0; i < solicitudes.size(); i++) {
        if (solicitudes[i].getId() == id) return static_cast<int>(i);
    }
    return -1;
}

// Ordenamientos

//insercion O(n^2)
long ordenarInsercion(vector<Materia>& v, int criterio) {
    long comparaciones = 0;
    for (int i = 1; i < v.size(); i++) {
        Materia clave = v[i];
        int j = static_cast<int>(i) - 1;
        while (j >= 0 && menorQue(clave, v[j], criterio, comparaciones)) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = clave;
    }
    return comparaciones;
}

// Merge sort O(n log n)
static void mezclar(vector<Materia>& v, int izq, int medio, int der, int criterio, long& comparaciones) {
    vector<Materia> izquierda(v.begin() + izq, v.begin() + medio + 1);
    vector<Materia> derecha(v.begin() + medio + 1, v.begin() + der + 1);
    int i = 0, j = 0;
    int k = izq;
    while (i < izquierda.size() && j < derecha.size()) {
        if (menorQue(derecha[j], izquierda[i], criterio, comparaciones)) v[k++] = derecha[j++];
        else v[k++] = izquierda[i++];
    }
    while (i < izquierda.size()) v[k++] = izquierda[i++];
    while (j < derecha.size()) v[k++] = derecha[j++];
}

static void mergeSortRec(vector<Materia>& v, int izq, int der, int criterio, long& comparaciones) {
    if (izq >= der) return;                       // caso base: 0 o 1 elemento
    int medio = izq + (der - izq) / 2;
    mergeSortRec(v, izq, medio, criterio, comparaciones);
    mergeSortRec(v, medio + 1, der, criterio, comparaciones);
    mezclar(v, izq, medio, der, criterio, comparaciones);
}

long ordenarMergeSort(vector<Materia>& v, int criterio) {
    long comparaciones = 0;
    if (v.size() > 1) mergeSortRec(v, 0, static_cast<int>(v.size()) - 1, criterio, comparaciones);
    return comparaciones;
}

// Quick sort O(n log n)
static int particionar(vector<Materia>& v, int bajo, int alto, int criterio, long& comparaciones) {
    int medio = bajo + (alto - bajo) / 2;         // pivote central: evita el peor caso en datos ordenados
    swap(v[medio], v[alto]);
    Materia pivote = v[alto];
    int i = bajo - 1;
    for (int j = bajo; j < alto; j++) {
        if (menorQue(v[j], pivote, criterio, comparaciones)) swap(v[++i], v[j]);
    }
    swap(v[i + 1], v[alto]);
    return i + 1;
}

static void quickSortRec(vector<Materia>& v, int bajo, int alto, int criterio, long& comparaciones) {
    if (bajo >= alto) return;                     // caso base
    int p = particionar(v, bajo, alto, criterio, comparaciones);
    quickSortRec(v, bajo, p - 1, criterio, comparaciones);
    quickSortRec(v, p + 1, alto, criterio, comparaciones);
}

long ordenarQuickSort(vector<Materia>& v, int criterio) {
    long comparaciones = 0;
    if (v.size() > 1) quickSortRec(v, 0, static_cast<int>(v.size()) - 1, criterio, comparaciones);
    return comparaciones;
}

// Recursion sobre prerrequisitos: 
//Caso base: materia sin prerrequisitos.
// Caso recursivo: repetir para cada prerrequisito
void mostrarPrerrequisitos(const vector<Materia>& materias, const string& codigo, int nivel) {
    long ignorar = 0;
    int pos = buscarMateriaBinaria(materias, codigo, ignorar);
    if (pos == -1 || nivel > static_cast<int>(materias.size())) return;

    const vector<string>& previos = materias[pos].getPrerrequisitos();
    for (int i = 0; i < previos.size(); i++) {
        int posPrevio = buscarMateriaBinaria(materias, previos[i], ignorar);
        string nombre = (posPrevio == -1) ? "(no existe)" : materias[posPrevio].getNombre();
        cout << string((nivel + 1) * 2, ' ') << "- " << previos[i] << " " << nombre << endl;
        mostrarPrerrequisitos(materias, previos[i], nivel + 1);
    }
}

string generarCodigoMateria(const string& nombre, const vector<Materia>& materias) {
    string letras;
    for (int i = 0; i < nombre.size() && letras.size() < 3; i++) {
        unsigned char c = nombre[i];
        if (isalpha(c)) letras += static_cast<char>(toupper(c));
    }
    while (letras.size() < 3) letras += 'X';

    int consecutivo = 100 + static_cast<int>(materias.size());
    long ignorar = 0;
    string codigo = letras + to_string(consecutivo);
    while (buscarMateriaBinaria(materias, codigo, ignorar) != -1) {   // garantiza unicidad
        consecutivo++;
        codigo = letras + to_string(consecutivo);
    }
    return codigo;
}
