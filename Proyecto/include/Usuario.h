// Usuario.h

#ifndef USUARIO_H
#define USUARIO_H

#include <iostream>
#include <string>
#include <vector>

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

class Estudiante : public Usuario {
protected:
    string carrera;
    long codigoEstudiante;
    int semestre;

public:
    Estudiante(string nombres, string apellidos, string username, string password,
               long codigoEstudiante, string carrera, int semestre);

    long getCodigoEstudiante() const { return codigoEstudiante; }
    string getCarrera() const { return carrera; }
    int getSemestre() const { return semestre; }
};

// Un Monitor es un Estudiante con horario y una materia asignada
class Monitor : public Estudiante {
private:
    long codigoMonitor;
    string horarioDisponibilidad;
    string materiaAsignada;   // codigo de la materia ("" si no tiene)
    int solicitudActual;      // Id de la solicitud que tiene asignada (0 = franja libre)

public:
    Monitor(string nombres, string apellidos, string username, string password,
            long codigoEstudiante, string carrera, int semestre,
            long codigoMonitor, string horarioDisponibilidad);

    void asignarMateria(const string& codigoMateria) { materiaAsignada = codigoMateria; }
    long getCodigoMonitor() const { return codigoMonitor; }
    string getHorario() const { return horarioDisponibilidad; }
    string getMateriaAsignada() const { return materiaAsignada; }

    // NUEVO: control de la franja del monitor
    int getSolicitudActual() const { return solicitudActual; }
    void setSolicitudActual(int id) { solicitudActual = id; }
    bool estaLibre() const { return solicitudActual == 0; }
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

#endif