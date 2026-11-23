//GestorArchivos.cpp
#include "GestorArchivos.h"
#include "../Materia/Materia.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>

using namespace std;

// Función para recortar espacios y retornos
static string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Separa por delimitador
static vector<string> separar(const string& linea, char delim, int maxPartes = 0) {
    vector<string> partes;
    size_t inicio = 0;
    while (true) {
        size_t pos = linea.find(delim, inicio);
        bool ultima = (maxPartes > 0 && partes.size() + 1 == static_cast<size_t>(maxPartes));
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

// Convierte las franjas de un monitor en texto: "dia@inicio-fin|dia@inicio-fin"
static string franjasATexto(const Monitor& m) {
    string texto;
    const vector<BloqueHorario>& f = m.getDisponibilidad();
    for (size_t i = 0; i < f.size(); i++) { 
        if (i > 0) texto += "|";
        texto += to_string(f[i].dia) + "@" + to_string(f[i].inicio) + "-" + to_string(f[i].fin);
    }
    return texto;
}

// Lee el texto anterior y agrega las franjas validas al monitor (las invalidas se ignoran)
static void textoAFranjas(const string& texto, Monitor& m) {   // Carga las franjas guardadas)
    if (texto.empty()) return;   // Sin franjas, no hace nada
    vector<string> bloques = separar(texto, '|');   // Separa cada franja
    for (size_t i = 0; i < bloques.size(); i++) {   // Recorre las franjas
        int dia, ini, fn;
        if (sscanf(bloques[i].c_str(), "%d@%d-%d", &dia, &ini, &fn) == 3) m.agregarFranja(dia, ini, fn);   // Agrega la franja, agregarFranja descarta las invalidas o cruzadas
    }
}

// HU-01.06: franjas del estudiante, mismo formato "dia@inicio-fin|..."
static string franjasEstudianteATexto(const Estudiante& e) {
    string texto;
    const vector<BloqueHorario>& f = e.getDisponibilidadEstudiante();
    for (size_t i = 0; i < f.size(); i++) {
        if (i > 0) texto += "|";
        texto += to_string(f[i].dia) + "@" + to_string(f[i].inicio) + "-" + to_string(f[i].fin);
    }
    return texto;
}
static void textoAFranjasEstudiante(const string& texto, Estudiante& e) {
    if (texto.empty()) return;
    vector<string> bloques = separar(texto, '|');
    for (size_t i = 0; i < bloques.size(); i++) {
        int dia, ini, fn;
        if (sscanf(bloques[i].c_str(), "%d@%d-%d", &dia, &ini, &fn) == 3) e.agregarFranjaEstudiante(dia, ini, fn);
    }
}

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
            Estudiante* e = new Estudiante(p[1], p[2], p[3], p[4], aLong(p[6]), p[7], aInt(p[8]));
            if (p.size() >= 10) textoAFranjasEstudiante(p[9], *e);    // HU-01.06
            u = e;
        } else if (p[0] == "Profesor") {
            Profesor* pr = new Profesor(p[1], p[2], p[3], p[4], p[6]);
            if (p.size() >= 8) pr->setCodigoProfesor(aLong(p[7]));
            if (p.size() >= 9) pr->setArea(p[8]);
            u = pr;
        } else if (p[0] == "Monitor" && p.size() >= 11) {
            Monitor* m = new Monitor(p[1], p[2], p[3], p[4], aLong(p[6]), p[7], aInt(p[8]), aLong(p[9]), p[10]);
            for (size_t col = 11; col <= 12 && col < p.size(); col++) {
                vector<string> lista = separar(p[col], ';');
                for (size_t k = 0; k < lista.size(); k++) {
                    if (!lista[k].empty()) m->agregarMateria(lista[k]);
                }
            }
            if (p.size() > 13) textoAFranjas(p[13], *m);
            u = m;
        }
        if (u) {
            u->setActivo(activo);
            usuarios.push_back(u);
        }
    }

    long maxProfesor = 0;
    for (size_t i = 0; i < usuarios.size(); i++){
        const Profesor* pr = dynamic_cast <const Profesor*>(usuarios[i]);
        if (pr != nullptr && pr->getCodigoProfesor() > maxProfesor) maxProfesor = pr->getCodigoProfesor();
    }
    for (size_t i = 0; i < usuarios.size(); i++){
        Profesor* pr = dynamic_cast<Profesor*>(usuarios[i]);
        if (pr != nullptr && pr ->getCodigoProfesor() == 0) pr->setCodigoProfesor(++maxProfesor);
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
                    << "," << m->getCodigoMonitor() << "," << m->getHorario();
            archivo << ",";
            const vector<string>& lista = m->getMaterias();
            for (size_t k = 0; k < lista.size(); k++) archivo << (k > 0 ? ";" : "") << lista[k];
            archivo << ",";
            archivo << "," << franjasATexto(*m);
        } else if (const Estudiante* e = dynamic_cast<const Estudiante*>(u)) {
            archivo << "," << e->getCodigoEstudiante() << "," << e->getCarrera() << "," << e->getSemestre()
                    << "," << franjasEstudianteATexto(*e);
        } else if (const Profesor* pr = dynamic_cast<const Profesor*>(u)) {
            archivo << "," << pr->getDepartamento()
                    << "," << pr->getCodigoProfesor() << "," << pr->getArea();
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
        // Sexta columna: horario como "dia@inicio-fin|...";
        Materia m(p[0], p[1], aInt(p[2]), p[3], previos);
        if (p.size() >= 6) textoAHorario(p[5], m);
        // HU-14.01: columnas 7 y 8 (opcionales): area y nivel. Si faltan: "General", nivel 1
        if (p.size() >= 8) { m.setArea(p[6]); m.setNivel(aInt(p[7])); }
        materias.push_back(m);
    }
    return true;
}
bool GestorArchivos::guardarMaterias(const vector<Materia>& materias) {
    ofstream archivo(resolverRuta(rutaMaterias));
    if (!archivo.is_open()) return false;
    for (const Materia& m : materias) {
        archivo << m.getCodigo() << "," << m.getNombre() << "," << m.getCreditos() << "," << m.getTipo() << ",";
        const vector<string>& previos = m.getPrerrequisitos();
        for (size_t i = 0; i < previos.size(); i++) archivo << (i > 0 ? ";" : "") << previos[i];
        archivo << "," << horarioATexto(m) << "," << m.getArea() << "," << m.getNivel() << "\n";
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
        // Formato nuevo: Id,Est,Materia,Urgencia,Estado,Fecha,Descripcion
        // Formato antiguo (sin fecha): se toma la fecha de carga como creacion
        vector<string> p = separar(linea, ',', 7);
        bool conFecha = (p.size() == 7 && !p[5].empty() &&
                         p[5].find_first_not_of("0123456789") == string::npos);
        if (!conFecha) p = separar(linea, ',', 6);    // la descripcion (ultima) puede llevar comas
        if (p.size() < 6) continue;
        Solicitud s(aInt(p[0]), aLong(p[1]), p[2], aInt(p[3]), conFecha ? p[6] : p[5], p[4]);
        if (conFecha) s.setFechaCreacion(aLong(p[5]));
        solicitudes.push_back(s);
    }
    return true;
}

bool GestorArchivos::guardarSolicitudes(const vector<Solicitud>& solicitudes) {
    ofstream archivo(resolverRuta(rutaSolicitudes));
    if (!archivo.is_open()) return false;
    for (const Solicitud& s : solicitudes) {
        archivo << s.getId() << "," << s.getCodigoEstudiante() << "," << s.getCodigoMateria() << ","
                << s.getUrgencia() << "," << s.getEstado() << "," << s.getFechaCreacion() << ","
                << s.getDescripcion() << "\n";
    }
    return true;
}

// PERSISTENCIA BITACORA
bool GestorArchivos::cargarBitacora(BitacoraAsignaciones& bitacora) {
    ifstream archivo(resolverRuta(rutaBitacora));
    if (!archivo.is_open()) return false;

    string linea;
    while (getline(archivo, linea)) {
        linea = trim(linea);
        if (linea.empty() || linea[0] == '#') continue;
        bitacora.registrar(linea);
    }
    return true;
}

bool GestorArchivos::guardarBitacora(const BitacoraAsignaciones& bitacora) {
    string ruta = resolverRuta(rutaBitacora);
    {
        ifstream existe(ruta);
        if (!existe.is_open()) {
            string rutaBase = resolverRuta(rutaUsuarios);
            size_t barra = rutaBase.find_last_of("/\\");
            string carpeta = (barra == string::npos) ? "" : rutaBase.substr(0, barra + 1);
            ruta = carpeta + "bitacora.txt";
        }
    }

    ofstream archivo(ruta);
    if (!archivo.is_open()) return false;

    const vector<string>& registros = bitacora.getRegistros();
    for (size_t i = 0; i < registros.size(); i++) {
        archivo << registros[i] << "\n";
    }
    return true;
}
//Gestor de archivos: ruta de un archivo nuevo junto a usuarios.txt
string GestorArchivos::rutaEnData(const string& archivo) const {
    string rutaBase = resolverRuta(rutaUsuarios);
    size_t barra = rutaBase.find_last_of("/\\");
    string carpeta = (barra == string::npos) ? "" : rutaBase.substr(0, barra + 1);
    return carpeta + archivo;
}

bool GestorArchivos::verificarEntorno() const {
    ifstream usuarios(resolverRuta(rutaUsuarios));
    if (!usuarios.is_open()) {
        cout << "ERROR: no se encontro " << rutaUsuarios << " (ni la carpeta data/)." << endl;
        cout << "Ejecute el programa desde la carpeta Proyecto o desde la raiz del repositorio." << endl;
        return false;
    }
    const string opcionales[] = {rutaMaterias, rutaSolicitudes, "data/bitacora.txt",
                                 "data/estados_materias.txt", "data/asignaciones.txt"};
    for (const string& r : opcionales) {
        size_t barra = r.find_last_of('/');
        ifstream f(rutaEnData(r.substr(barra + 1)));
        if (!f.is_open())
            cout << "Aviso: no existe " << r << ". Se inicia vacio y se creara al salir." << endl;
    }
    return true;
}

bool GestorArchivos::cargarEstadosMaterias(vector<Usuario*>& usuarios) {
    ifstream archivo(rutaEnData("estados_materias.txt"));
    if (!archivo.is_open()) return false;
    string linea;
    while (getline(archivo, linea)) {
        linea = trim(linea);
        if (linea.empty() || linea[0] == '#') continue;
        vector<string> p = separar(linea, ',');
        if (p.size() < 3) continue;
        Estudiante* estudiante = buscarEstudiantePorCodigo(usuarios, aLong(p[0]));
        if (estudiante != nullptr) estudiante->registrarEstadoMateria(p[1], p[2]);   // codigo inexistente: se ignora
    }
    return true;
}

bool GestorArchivos::guardarEstadosMaterias(const vector<Usuario*>& usuarios) {
    ofstream archivo(rutaEnData("estados_materias.txt"));
    if (!archivo.is_open()) return false;
    for (size_t i = 0; i < usuarios.size(); i++) {
        if (!esEstudiante(usuarios[i])) continue;
        const Estudiante* estudiante = static_cast<const Estudiante*>(usuarios[i]);   // el rol confirma el tipo
        const vector<EstadoMateria>& estados = estudiante->getEstadosMaterias();
        for (size_t k = 0; k < estados.size(); k++)
            archivo << estudiante->getCodigoEstudiante() << "," << estados[k].codigoMateria << "," << estados[k].estado << "\n";
    }
    return true;
}

bool GestorArchivos::cargarAsignaciones(vector<Solicitud>& solicitudes, vector<Usuario*>& usuarios) {
    ifstream archivo(rutaEnData("asignaciones.txt"));
    if (!archivo.is_open()) return false;
    string linea;
    while (getline(archivo, linea)) {
        linea = trim(linea);
        if (linea.empty() || linea[0] == '#') continue;
        vector<string> p = separar(linea, ',');
        if (p.size() < 3) continue;
        int idSolicitud = aInt(p[0]);
        long codigoMonitor = aLong(p[1]);
        BloqueHorario bloque{-1, 0, 0};
        if (sscanf(p[2].c_str(), "%d@%d-%d", &bloque.dia, &bloque.inicio, &bloque.fin) != 3) continue;

        int pos = buscarSolicitudSecuencial(solicitudes, idSolicitud);
        if (pos == -1 || solicitudes[pos].getEstado() != "Asignada") continue;

        Monitor* monitor = buscarMonitorPorCodigo(usuarios, codigoMonitor);
        if (monitor == nullptr || !monitor->estaLibre()) continue;   // monitor inexistente u ocupado: se descarta

        solicitudes[pos].setSesion(codigoMonitor, bloque);
        monitor->setSolicitudActual(idSolicitud);
    }
    return true;
}

bool GestorArchivos::guardarAsignaciones(const vector<Solicitud>& solicitudes) {
    ofstream archivo(rutaEnData("asignaciones.txt"));
    if (!archivo.is_open()) return false;
    for (const Solicitud& s : solicitudes) {
        if (s.getEstado() != "Asignada" || !s.tieneBloque()) continue;
        const BloqueHorario& b = s.getBloque();
        archivo << s.getId() << "," << s.getCodigoMonitor() << "," << b.dia << "@" << b.inicio << "-" << b.fin << "\n";
    }
    return true;
}
