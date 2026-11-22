//Usuario.cpp

#include "Usuario.h"
#include "../Materia/Materia.h"   // Se usa para nombreDia y formatoHora para escribir las franjas como texto
#include <algorithm>   // Se usa para buscar una materia en la lista del monitor mediante un "find"

Usuario::Usuario() : activo(true) {}

Usuario::Usuario(string nombres, string apellidos, string username, string password, string rol)
    : nombres(nombres), apellidos(apellidos), username(username), password(password),
      rol(rol), activo(true) {}

bool Usuario::autenticarUsuario(const string& usuario, const string& clave) const {
    return username == usuario && password == clave && activo;
}

void Usuario::informacion() const {
    cout << "Nombre: " << nombres << " " << apellidos << endl;
    cout << "Usuario: " << username << " | Rol: " << rol << endl;
}

Administrador::Administrador(string nombres, string apellidos, string username, string password, long codigo)
    : Usuario(nombres, apellidos, username, password, "Administrador"),
      codigoAdministrador(codigo) {}

Profesor::Profesor() : Usuario() {
    rol = "Profesor";
}

Profesor::Profesor(string nombres, string apellidos, string username, string password, string departamento)
    : Usuario(nombres, apellidos, username, password, "Profesor"), departamento(departamento) {}

string Profesor::getDepartamento() const { return departamento; }
void Profesor::setDepartamento(const string& dep) { departamento = dep; }

Estudiante::Estudiante(string nombres, string apellidos, string username, string password,
                       long codigoEstudiante, string carrera, int semestre)
    : Usuario(nombres, apellidos, username, password, "Estudiante"),
      carrera(carrera), codigoEstudiante(codigoEstudiante), semestre(semestre) {}

Monitor::Monitor(string nombres, string apellidos, string username, string password,
                 long codigoEstudiante, string carrera, int semestre,
                 long codigoMonitor, string horarioDisponibilidad)
    : Estudiante(nombres, apellidos, username, password, codigoEstudiante, carrera, semestre),
      codigoMonitor(codigoMonitor), horarioDisponibilidad(horarioDisponibilidad),
      solicitudActual(0) {          // NUEVO: al crearse, el monitor esta libre
    rol = "Monitor";
}

// Texto de la disponibilidad: se arma desde las franjas, si no hay franjas, se usa el texto antiguo
string Monitor::getHorario() const { // Devuelve la disponibilidad como texto legible
    if (disponibilidad.empty()) return horarioDisponibilidad;
    string texto;
    for (size_t i = 0; i < disponibilidad.size(); i++) { 
        if (i > 0) texto += "; ";
        texto += nombreDia(disponibilidad[i].dia) + " " + formatoHora(disponibilidad[i].inicio) + " - " + formatoHora(disponibilidad[i].fin);   // Ejemplo "Lunes 2:00 PM - 4:00 PM"
    }
    return texto;
}

bool Monitor::dominaMateria(const string& codigo) const {  //Dice si el monitor atiende esa materia
    if (codigo.empty()) return false;
    return find(materiasMonitor.begin(), materiasMonitor.end(), codigo) != materiasMonitor.end();   // True si la materia esta en la lista
}

bool Monitor::agregarMateria(const string& codigo) {   //Agrega una materia a la lista del monitor
    if (dominaMateria(codigo)) return false;   // Rechaza codigos vacios y repetidos)
    materiasMonitor.push_back(codigo);   // Guarda la materia al final, respetando el orden en que se asigno
    return true;
}

bool Monitor::quitarMateria(const string& codigo) {   //Quita una materia de la lista del monitor
    vector<string>::iterator it = find(materiasMonitor.begin(), materiasMonitor.end(), codigo);   // Busca la materia a quitar
    if (it == materiasMonitor.end()) return false;   // Falso si el monitor no tiene esa materia
    materiasMonitor.erase(it);   // La quita sin alterar el orden de las otras materias
    return true;
}

bool Monitor::agregarFranja(int dia, int inicio, int fin) {   // Agrega una franja de disponibilidad si es valida
    if (dia < 0 || dia >= DIAS_SEMANA) return false;   // Dias fuera de lunes a sabado da falso
    if (inicio < 0 || fin > FRANJAS_DIA || inicio >= fin) return false; 
    for (size_t i = 0; i < disponibilidad.size(); i++) { 
        if (disponibilidad[i].dia == dia && inicio < disponibilidad[i].fin && fin > disponibilidad[i].inicio) return false;
    }   // (Nuevo: cierra el for)
    disponibilidad.push_back(BloqueHorario{dia, inicio, fin});
    return true;
}

// Misma validacion que las franjas del monitor, sobre la lista del estudiante
bool Estudiante::agregarFranjaEstudiante(int dia, int inicio, int fin) {
    if (dia < 0 || dia >= DIAS_SEMANA) return false;
    if (inicio < 0 || fin > FRANJAS_DIA || inicio >= fin) return false;
    for (size_t i = 0; i < disponibilidadEstudiante.size(); i++) {
        if (disponibilidadEstudiante[i].dia == dia && inicio < disponibilidadEstudiante[i].fin && fin > disponibilidadEstudiante[i].inicio) return false;
    }
    disponibilidadEstudiante.push_back(BloqueHorario{dia, inicio, fin});
    return true;
}

bool Estudiante::disponibleEn(int dia, int inicio, int fin) const {
    for (size_t i = 0; i < disponibilidadEstudiante.size(); i++) {
        const BloqueHorario& b = disponibilidadEstudiante[i];
        if (b.dia == dia && b.inicio <= inicio && fin <= b.fin) return true;
    }
    return false;
}

string Estudiante::disponibilidadTexto() const {
    string texto;
    for (size_t i = 0; i < disponibilidadEstudiante.size(); i++) {
        const BloqueHorario& b = disponibilidadEstudiante[i];
        if (i > 0) texto += "; ";
        texto += nombreDia(b.dia) + " " + formatoHora(b.inicio) + " - " + formatoHora(b.fin);
    }
    return texto;
}

void Estudiante::registrarEstadoMateria(const string& codigo, const string& estado) {
    for (size_t i = 0; i < estadosMaterias.size(); i++) {
        if (estadosMaterias[i].codigoMateria == codigo) { estadosMaterias[i].estado = estado; return; }   // se actualiza
    }
    estadosMaterias.push_back(EstadoMateria{codigo, estado});
}

// ============================================================================
// Busquedas sobre la lista de usuarios
// ============================================================================
bool esEstudiante(const Usuario* usuario) {
    return usuario->getRol() == "Estudiante" || usuario->getRol() == "Monitor";
}

bool esMonitor(const Usuario* usuario) {
    return usuario->getRol() == "Monitor";
}

Estudiante* buscarEstudiantePorCodigo(const vector<Usuario*>& usuarios, long codigoEstudiante) {
    for (size_t i = 0; i < usuarios.size(); i++) {
        if (!esEstudiante(usuarios[i])) continue;
        Estudiante* estudiante = static_cast<Estudiante*>(usuarios[i]);   // el rol confirma el tipo
        if (estudiante->getCodigoEstudiante() == codigoEstudiante) return estudiante;
    }
    return nullptr;
}

Monitor* buscarMonitorPorCodigo(const vector<Usuario*>& usuarios, long codigoMonitor) {
    for (size_t i = 0; i < usuarios.size(); i++) {
        if (!esMonitor(usuarios[i])) continue;
        Monitor* monitor = static_cast<Monitor*>(usuarios[i]);
        if (monitor->getCodigoMonitor() == codigoMonitor) return monitor;
    }
    return nullptr;
}

Monitor* buscarMonitorConSolicitud(const vector<Usuario*>& usuarios, int idSolicitud) {
    for (size_t i = 0; i < usuarios.size(); i++) {
        if (!esMonitor(usuarios[i])) continue;
        Monitor* monitor = static_cast<Monitor*>(usuarios[i]);
        if (monitor->getSolicitudActual() == idSolicitud) return monitor;
    }
    return nullptr;
}
