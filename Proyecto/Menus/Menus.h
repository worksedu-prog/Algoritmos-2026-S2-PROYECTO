// Menus.h
// Menus de consola: inicio de sesion y uno por cada rol
#ifndef MENUS_H
#define MENUS_H

#include <vector>
#include "../Usuario/Usuario.h"
#include "../Materia/Materia.h"
#include "../Solicitud/Solicitud.h"
#include "../Asignaciones/Asignaciones.h" //PilaAsignaciones y Bitacora

using namespace std;

void menuPruebas();
void menuGeneral();
void menuEstudiante(Estudiante& est, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                    ColaSolicitudes& cola, int& contadorSolicitudes);
void menuMonitor(Monitor& mon, vector<Solicitud>& solicitudes, ColaSolicitudes& cola);
void menuProfesor(Profesor& prof, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                  vector<Usuario*>& usuarios);
//Ahora recibe tambien la cola y la bitacora
void menuAdministrador(Administrador& admin, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                       vector<Usuario*>& usuarios, PilaAsignaciones& historial,
                       ColaSolicitudes& cola, BitacoraAsignaciones& bitacora);

#endif
