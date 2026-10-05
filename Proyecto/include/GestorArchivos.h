// GestorArchivos.h

// Lee y guarda usuarios, materias y solicitudes en archivos de texto.
// Formatos (separados por coma):
//   usuarios.txt    Rol,Nombres,Apellidos,Usuario,Clave,Activo(1/0)
//   materias.txt    Codigo,Nombre,Creditos,Tipo,Prerrequisitos(separados por ';')
//   solicitudes.txt Id,CodigoEstudiante,CodigoMateria,Urgencia(1-3),Estado,Descripcion
#ifndef GESTORARCHIVOS_H
#define GESTORARCHIVOS_H

#include <string>
#include <vector>
#include "Usuario.h"
#include "Materia.h"
#include "Solicitud.h"

using namespace std;

class GestorArchivos {
private:
    string rutaUsuarios, rutaMaterias, rutaSolicitudes;
    string resolverRuta(const string& ruta) const;

public:
    GestorArchivos(const string& rutaUsuarios = "data/usuarios.txt",
                   const string& rutaMaterias = "data/materias.txt",
                   const string& rutaSolicitudes = "data/solicitudes.txt");

    bool cargarUsuarios(vector<Usuario*>& usuarios);
    bool guardarUsuarios(const vector<Usuario*>& usuarios);
    bool cargarMaterias(vector<Materia>& materias);
    bool guardarMaterias(const vector<Materia>& materias);
    bool cargarSolicitudes(vector<Solicitud>& solicitudes);
    bool guardarSolicitudes(const vector<Solicitud>& solicitudes);
};

#endif
