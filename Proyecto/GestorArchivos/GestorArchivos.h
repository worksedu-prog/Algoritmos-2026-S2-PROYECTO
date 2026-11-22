// GestorArchivos.h

// Lee y guarda usuarios, materias y solicitudes en archivos de texto.
// Formatos (separados por coma):
//   usuarios.txt    Rol,Nombres,Apellidos,Usuario,Clave,Activo(1/0)
//   materias.txt    Codigo,Nombre,Creditos,Tipo,Prerrequisitos y horario
//   solicitudes.txt Id,CodigoEstudiante,CodigoMateria,Urgencia(1-3),Estado,Descripcion
//   Monitor agrega al final: ...,CodigoMonitor,Horario,Materias(separadas por ;),Reservada(vacia),Franjas(dia@inicio-fin separadas por |)
#ifndef GESTORARCHIVOS_H
#define GESTORARCHIVOS_H

#include <string>
#include <vector>
#include "../Usuario/Usuario.h"
#include "../Materia/Materia.h"
#include "../Solicitud/Solicitud.h"
#include "../Asignaciones/Asignaciones.h"

using namespace std;

class GestorArchivos {
private:
    string rutaUsuarios, rutaMaterias, rutaSolicitudes;
    string rutaBitacora = "data/bitacora.txt";
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
    bool cargarBitacora(BitacoraAsignaciones& bitacora);
    bool guardarBitacora(const BitacoraAsignaciones& bitacora);
};

#endif