// Menus.h
// Menus de consola: inicio de sesion y uno por cada rol
#ifndef MENUS_H
#define MENUS_H

#include <vector>
#include "Usuario.h"
#include "Materia.h"
#include "Solicitud.h"

using namespace std;

void menuPruebas();
void menuGeneral();
void menuEstudiante(Estudiante& est, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                    ColaSolicitudes& cola, int& contadorSolicitudes);
void menuMonitor(Monitor& mon, vector<Solicitud>& solicitudes, ColaSolicitudes& cola);
void menuProfesor(Profesor& prof, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                  vector<Usuario*>& usuarios);
void menuAdministrador(Administrador& admin, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                       vector<Usuario*>& usuarios, PilaAsignaciones& historial);

#endif
