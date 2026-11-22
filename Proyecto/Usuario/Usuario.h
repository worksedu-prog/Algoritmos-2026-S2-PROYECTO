// Usuario.h

#ifndef USUARIO_H
#define USUARIO_H

#include <iostream>
#include <string>
#include <vector>
#include "../Materia/Materia.h"

using namespace std;

class Usuario {
protected:
    string nombres, apellidos, username, password;
    string rol;      // "Administrador", "Estudiante" o "Monitor"
    bool activo;

public:
    Usuario();
    Usuario(string nombres, string apellidos, string username, string password, string rol);
    virtual ~Usuario() {}

    // Credenciales correctas y usuario activo
    bool autenticarUsuario(const string& usuario, const string& clave) const;
    virtual void informacion() const;

    string getNombres() const { return nombres; }
    string getApellidos() const { return apellidos; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getRol() const { return rol; }
    bool isActivo() const { return activo; }
    void setActivo(bool valor) { activo = valor; }
};

class Administrador : public Usuario {
private:
    long codigoAdministrador;

public:
    Administrador(string nombres, string apellidos, string username, string password, long codigo);
    long getCodigoAdministrador() const { return codigoAdministrador; }
};

// Estado de una materia cursada por un estudiante
struct EstadoMateria {
    string codigoMateria;
    string estado;           // "Aprobada", "Reprobada" o "Retirada"
};

class Estudiante : public Usuario {
protected:
    string carrera;
    long codigoEstudiante;
    int semestre;
    // Disponibilidad horaria propia del estudiante (independiente de la del Monitor)
    vector<BloqueHorario> disponibilidadEstudiante;
    vector<EstadoMateria> estadosMaterias;

public:
    Estudiante(string nombres, string apellidos, string username, string password,
               long codigoEstudiante, string carrera, int semestre);

    long getCodigoEstudiante() const { return codigoEstudiante; }
    string getCarrera() const { return carrera; }
    int getSemestre() const { return semestre; }

    const vector<BloqueHorario>& getDisponibilidadEstudiante() const { return disponibilidadEstudiante; }
    bool agregarFranjaEstudiante(int dia, int inicio, int fin);   // false si es invalida o se cruza con otra
    bool disponibleEn(int dia, int inicio, int fin) const;   // el bloque cabe dentro de alguna franja
    string disponibilidadTexto() const;                 // "Lunes 2:00 PM - 4:00 PM; ..."

    // Registra (o actualiza) el estado de una materia
    void registrarEstadoMateria(const string& codigo, const string& estado);
    const vector<EstadoMateria>& getEstadosMaterias() const { return estadosMaterias; }
};

// Un Monitor es un Estudiante con horario y una lista de materias que atiende
class Monitor : public Estudiante {
private:
    long codigoMonitor;
    string horarioDisponibilidad;
    vector<string> materiasMonitor;   // Unica lista de materias del monitor; reemplaza a la variable string materiaAsignada
    int solicitudActual;
    vector<BloqueHorario> disponibilidad;

public:
    Monitor(string nombres, string apellidos, string username, string password,
            long codigoEstudiante, string carrera, int semestre,
            long codigoMonitor, string horarioDisponibilidad);

    // Se elimino asignarMateria() y getMateriaAsignada() porque ahora se usan agregarMateria(), quitarMateria() y getMaterias() con tal de atender multiples materias
    long getCodigoMonitor() const { return codigoMonitor; }
    string getHorario() const;

    
    int getSolicitudActual() const { return solicitudActual; }
    void setSolicitudActual(int id) { solicitudActual = id; }
    bool estaLibre() const { return solicitudActual == 0; }

    const vector<string>& getMaterias() const { return materiasMonitor; }   // Lista unica de materias del monitor)
    bool agregarMateria(const string& codigo);   // Agrega una materia; false si el codigo esta vacio o ya la tiene
    bool quitarMateria(const string& codigo);   // Quita una materia; false si no la tiene
    bool dominaMateria(const string& codigo) const;   // True si la materia esta en la lista del monitor)
    const vector<BloqueHorario>& getDisponibilidad() const { return disponibilidad; }   // Franjas de disponibilidad)
    bool agregarFranja(int dia, int inicio, int fin);   // Agrega una franja valida. False si es invalida o se cruza con otra
};

class Profesor : public Usuario {
private:
    string departamento;

public:
    Profesor();
    Profesor(string nombres, string apellidos, string username, string password,
             string departamento);

    string getDepartamento() const;
    void setDepartamento(const string& departamento);
};

// Busquedas sobre la lista de usuarios
// La lista guarda punteros a Usuario. Para saber si un usuario es Estudiante o Monitor
// se revisa su rol; con el rol confirmado, static_cast convierte el puntero al tipo real.
// Un Monitor tambien es Estudiante (hereda de Estudiante), por eso cuenta en las busquedas
// de estudiantes. Todas son recorridos lineales O(n).
bool esEstudiante(const Usuario* usuario);   // rol "Estudiante" o "Monitor"
bool esMonitor(const Usuario* usuario);      // rol "Monitor"

// Estudiante (o monitor) con ese codigo de estudiante; nullptr si no existe
Estudiante* buscarEstudiantePorCodigo(const vector<Usuario*>& usuarios, long codigoEstudiante);
// Monitor con ese codigo de monitor; nullptr si no existe
Monitor* buscarMonitorPorCodigo(const vector<Usuario*>& usuarios, long codigoMonitor);
// Monitor que tiene asignada esa solicitud; nullptr si ninguno la tiene
Monitor* buscarMonitorConSolicitud(const vector<Usuario*>& usuarios, int idSolicitud);

#endif