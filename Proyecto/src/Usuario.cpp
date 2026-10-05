
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
      codigoMonitor(codigoMonitor), horarioDisponibilidad(horarioDisponibilidad) {
    rol = "Monitor";
}

void asignarMateriaAMonitor(Monitor& mon, const string& codigoMateria, PilaAsignaciones& historial) {
    Asignacion a;
    a.monitor = &mon;
    a.materiaAnterior = mon.getMateriaAsignada();
    a.materiaNueva = codigoMateria;
    historial.push(a);
    mon.asignarMateria(codigoMateria);
    cout << "Se asigno la materia " << codigoMateria << " a " << mon.getNombres() << "." << endl;
}

void deshacerAsignacion(PilaAsignaciones& historial) {
    if (historial.isEmpty()) {
        cout << "No hay asignaciones para deshacer." << endl;
        return;
    }
    Asignacion ultima = historial.top();
    historial.pop();
    ultima.monitor->asignarMateria(ultima.materiaAnterior);
    cout << "Se deshizo la asignacion de " << ultima.materiaNueva
         << " a " << ultima.monitor->getNombres() << "." << endl;
}
