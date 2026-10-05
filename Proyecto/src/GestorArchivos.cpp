#include "../include/GestorArchivos.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

// Función para recortar espacios y retornos ('\r')
static string trim(const string& str) {
    int first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    int last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Separa por delimitador
static vector<string> separar(const string& linea, char delim, int maxPartes = 0) {
    vector<string> partes;
    int inicio = 0;
    while (true) {
        int pos = linea.find(delim, inicio);
        bool ultima = (maxPartes > 0 && partes.size() + 1 == maxPartes);
        if (pos == string::npos || ultima) {
            partes.push_back(trim(linea.substr(inicio)));
            break;
        }
        partes.push_back(trim(linea.substr(inicio, pos - inicio)));
        inicio = pos + 1;
    }
    return partes;
}


static long aLong(const string& s) { try { return stol(s); } catch (...) { return 0; } }
static int aInt(const string& s) { try { return stoi(s); } catch (...) { return 0; } }

GestorArchivos::GestorArchivos(const string& u, const string& m, const string& s)
    : rutaUsuarios(u), rutaMaterias(m), rutaSolicitudes(s) {}

// Busca el archivo ejecutando desde la raiz, desde src/ o desde la carpeta superior
string GestorArchivos::resolverRuta(const string& ruta) const {
    const string prefijos[] = {"", "../", "Proyecto/"};
    for (const string& p : prefijos) {
        ifstream f(p + ruta);
        if (f.is_open()) return p + ruta;
    }
    return ruta;
}


// PERSISTENCIA USUARIOS
bool GestorArchivos::cargarUsuarios(vector<Usuario*>& usuarios) {
    ifstream archivo(resolverRuta(rutaUsuarios));
    if (!archivo.is_open()) return false;

    string linea;
    while (getline(archivo, linea)) {
        linea = trim(linea);
        if (linea.empty() || linea[0] == '#') continue;
        vector<string> p = separar(linea, ',');
        if (p.size() < 7) continue;
        bool activo = (p[5] == "1");
        Usuario* u = nullptr;
        if (p[0] == "Administrador") {
            u = new Administrador(p[1], p[2], p[3], p[4], aLong(p[6]));
        } else if (p[0] == "Estudiante" && p.size() >= 9) {
            u = new Estudiante(p[1], p[2], p[3], p[4], aLong(p[6]), p[7], aInt(p[8]));
        } else if (p[0] == "Profesor") {
            u = new Profesor(p[1], p[2], p[3], p[4], p[6]);
        } else if (p[0] == "Monitor" && p.size() >= 11) {
            Monitor* m = new Monitor(p[1], p[2], p[3], p[4], aLong(p[6]), p[7], aInt(p[8]), aLong(p[9]), p[10]);
            if (p.size() > 11) m->asignarMateria(p[11]);
            u = m;
        }
        if (u) {
            u->setActivo(activo);
            usuarios.push_back(u);
        }
    }
    return true;
}
bool GestorArchivos::guardarUsuarios(const vector<Usuario*>& usuarios) {
    ofstream archivo(resolverRuta(rutaUsuarios));
    if (!archivo.is_open()) return false;

    for (const Usuario* u : usuarios) {
        archivo << u->getRol() << "," << u->getNombres() << "," << u->getApellidos() << ","
                << u->getUsername() << "," << u->getPassword() << "," << (u->isActivo() ? 1 : 0);
        if (const Monitor* m = dynamic_cast<const Monitor*>(u)) {          // Monitor antes que Estudiante
            archivo << "," << m->getCodigoEstudiante() << "," << m->getCarrera() << "," << m->getSemestre()
                    << "," << m->getCodigoMonitor() << "," << m->getHorario() << "," << m->getMateriaAsignada();
        } else if (const Estudiante* e = dynamic_cast<const Estudiante*>(u)) {
            archivo << "," << e->getCodigoEstudiante() << "," << e->getCarrera() << "," << e->getSemestre();
        } else if (const Profesor* pr = dynamic_cast<const Profesor*>(u)) {
            archivo << "," << pr->getDepartamento();
        } else if (const Administrador* a = dynamic_cast<const Administrador*>(u)) {
            archivo << "," << a->getCodigoAdministrador();
        }
        archivo << "\n";
    }
    return true;
}


// PERSISTENCIA MATERIAS
bool GestorArchivos::cargarMaterias(vector<Materia>& materias) {
    ifstream archivo(resolverRuta(rutaMaterias));
    if (!archivo.is_open()) return false;

    string linea;
    while (getline(archivo, linea)) {
        linea = trim(linea);
        if (linea.empty() || linea[0] == '#') continue;
        vector<string> p = separar(linea, ',');
        if (p.size() < 4) continue;
        vector<string> previos;
        if (p.size() >= 5 && !p[4].empty()) previos = separar(p[4], ';');
        materias.push_back(Materia(p[0], p[1], aInt(p[2]), p[3], previos));
    }
    return true;
}
bool GestorArchivos::guardarMaterias(const vector<Materia>& materias) {
    ofstream archivo(resolverRuta(rutaMaterias));
    if (!archivo.is_open()) return false;
    for (const Materia& m : materias) {
        archivo << m.getCodigo() << "," << m.getNombre() << "," << m.getCreditos() << "," << m.getTipo() << ",";
        const vector<string>& previos = m.getPrerrequisitos();
        for (int i = 0; i < previos.size(); i++) archivo << (i > 0 ? ";" : "") << previos[i];
        archivo << "\n";
    }
    return true;
}


// PERSISTENCIA SOLICITUDES
bool GestorArchivos::cargarSolicitudes(vector<Solicitud>& solicitudes) {
    ifstream archivo(resolverRuta(rutaSolicitudes));
    if (!archivo.is_open()) return false;
    string linea;
    while (getline(archivo, linea)) {
        linea = trim(linea);
        if (linea.empty() || linea[0] == '#') continue;
        vector<string> p = separar(linea, ',', 6);    // la descripcion (ultima) puede llevar comas
        if (p.size() < 6) continue;
        solicitudes.push_back(Solicitud(aInt(p[0]), aLong(p[1]), p[2], aInt(p[3]), p[5], p[4]));
    }
    return true;
}


bool GestorArchivos::guardarSolicitudes(const vector<Solicitud>& solicitudes) {
    ofstream archivo(resolverRuta(rutaSolicitudes));
    if (!archivo.is_open()) return false;
    for (const Solicitud& s : solicitudes) {
        archivo << s.getId() << "," << s.getCodigoEstudiante() << "," << s.getCodigoMateria() << ","
                << s.getUrgencia() << "," << s.getEstado() << "," << s.getDescripcion() << "\n";
    }
    return true;
}
