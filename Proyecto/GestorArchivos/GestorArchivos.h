// GestorArchivos.h

// Lee y guarda usuarios, materias y solicitudes en archivos de texto.
// Formatos (separados por coma):
//   usuarios.txt    Rol,Nombres,Apellidos,Usuario,Clave,Activo(1/0)
//   materias.txt    Codigo,Nombre,Creditos,Tipo,Prerrequisitos y horario
//   materias.txt    ...,Horario,Area,Nivel   (Area y Nivel opcionales)
//   solicitudes.txt Id,CodigoEstudiante,CodigoMateria,Urgencia(1-3),Estado,FechaCreacion(epoch),Descripcion
//                   (las lineas antiguas sin FechaCreacion se siguen leyendo)
//   Estudiante agrega al final: ...,CodigoEstudiante,Carrera,Semestre,Franjas
//   Monitor agrega al final: ...,CodigoMonitor,Horario,Materias(separadas por ;),Reservada(vacia),Franjas(dia@inicio-fin separadas por |)
//   Estudiante agrega al final: ...,CodigoEstudiante,Carrera,Semestre,Franjas
//   Estudiante agrega al final: ...,CodigoEstudiante,Carrera,Semestre,Franjas
//   Profesor agrega al final: ...,Departamento,CodigoProfesor,Area   // (Nuevo: HU-34, las dos ultimas columnas son opcionales)


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
    string rutaEnData(const string& archivo) const;   // ruta para un archivo nuevo junto a usuarios.txt

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

    // Revisa la carpeta data/ y los archivos antes de iniciar. Devuelve false si
    // falta algo imprescindible (usuarios.txt: sin el no se puede iniciar sesion).
    bool verificarEntorno() const;
    // Estados de materias por estudiante (data/estados_materias.txt)
    //   CodigoEstudiante,CodigoMateria,Estado
    bool cargarEstadosMaterias(vector<Usuario*>& usuarios);
    bool guardarEstadosMaterias(const vector<Usuario*>& usuarios);
    // Asignaciones vigentes (data/asignaciones.txt)
    //   IdSolicitud,CodigoMonitor,dia@inicio-fin
    bool cargarAsignaciones(vector<Solicitud>& solicitudes, vector<Usuario*>& usuarios);
    bool guardarAsignaciones(const vector<Solicitud>& solicitudes);
};

#endif