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

Profesor::Profesor() : Usuario(), codigoProfesor(0), area("General") {
    rol = "Profesor";
}

Profesor::Profesor(string nombres, string apellidos, string username, string password, string departamento)
    : Usuario(nombres, apellidos, username, password, "Profesor"), departamento(departamento), codigoProfesor(0), area("General") {}

Profesor::Profesor(string nombres, string apellidos, string username, string password, string departamento,
                   long codigoProfesor, string area)
    : Usuario(nombres, apellidos, username, password, "Profesor"), departamento(departamento),
      codigoProfesor(codigoProfesor), area(area.empty() ? "General" : area) {}

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

TablaHashUsuarios::TablaHashUsuarios() : cubetas(CAPACIDAD_INICIAL), cantidad(0) {}

// Funcion hash. Mezcla los bits de la clave y la reduce al numero de cubetas.
//  Se convierte a unsigned para que un codigo negativo (dato danado) tampoco rompa el indice.
size_t TablaHashUsuarios::indiceDe(long clave, size_t numCubetas) {
    unsigned long h = static_cast<unsigned long>(clave);
    h ^= (h >> 16);
    h *= 2654435761UL;                                     // Multiplicador de Knuth
    h ^= (h >> 15); 
    return static_cast<size_t>(h % numCubetas);
}

// Duplica las cubetas y reubica cada entrada con la nueva capacidad)
void TablaHashUsuarios::redimensionar() {
    size_t nuevoTamano = cubetas.size() * 2 + 1;
    vector< vector<Entrada> > nuevas(nuevoTamano);
    for (size_t i = 0; i < cubetas.size(); i++) {
        for (size_t k = 0; k < cubetas[i].size(); k++) {
            nuevas[indiceDe(cubetas[i][k].clave, nuevoTamano)].push_back(cubetas[i][k]);
        }
    }
    cubetas.swap(nuevas);
}

// Inserta O(1) promedio. Si la clave ya existe NO reemplaza: conserva el primero
bool TablaHashUsuarios::insertar(long clave, Usuario* usuario) {
    if (usuario == nullptr) return false;
    if ((cantidad + 1) * 4 > static_cast<int>(cubetas.size()) * 3) redimensionar();
    vector<Entrada>& cubeta = cubetas[indiceDe(clave, cubetas.size())];
    for (size_t i = 0; i < cubeta.size(); i++) {
        if (cubeta[i].clave == clave) return false;
    }
    cubeta.push_back(Entrada{clave, usuario});
    cantidad++;
    return true;
}

//Busqueda O(1) promedio. Solo recorre la cubeta que le toca a la clave, nunca toda la coleccion
Usuario* TablaHashUsuarios::buscar(long clave, long& comparaciones) const {
    const vector<Entrada>& cubeta = cubetas[indiceDe(clave, cubetas.size())];
    for (size_t i = 0; i < cubeta.size(); i++) {
        comparaciones++;
        if (cubeta[i].clave == clave) return cubeta[i].usuario;
    }
    return nullptr;
}

void TablaHashUsuarios::vaciar() {
    cubetas.clear();
    cubetas.resize(CAPACIDAD_INICIAL);
    cantidad = 0;
}