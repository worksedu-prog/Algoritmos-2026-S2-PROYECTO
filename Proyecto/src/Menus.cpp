//Usuario.cpp

#include "../include/Usuario.h"

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