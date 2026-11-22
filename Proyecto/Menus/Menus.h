// Menus.h
// Menus de consola: inicio de sesion y uno por cada rol
#ifndef MENUS_H
#define MENUS_H

#include <vector>
#include "../Usuario/Usuario.h"
#include "../Materia/Materia.h"
#include "../Solicitud/Solicitud.h"
#include "../Asignaciones/Asignaciones.h" 
#include "../Grafo/Grafo.h"
#include "../Jerarquia/Jerarquia.h"

using namespace std;

// Recalcula la prioridad (3 criterios) de cada solicitud pendiente y reconstruye la cola.
// Se usa al iniciar el programa y desde el menu del administrador.
void recalcularPrioridades(vector<Solicitud>& solicitudes, const vector<Usuario*>& usuarios,
                           const vector<Materia>& materias, ColaSolicitudes& cola);

void menuPruebas();
void menuGeneral();
void menuEstudiante(Estudiante& est, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                    ColaSolicitudes& cola, int& contadorSolicitudes);
void menuMonitor(Monitor& mon, vector<Solicitud>& solicitudes, ColaSolicitudes& cola);
void menuProfesor(Profesor& prof, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                  vector<Usuario*>& usuarios);
void menuAdministrador(Administrador& admin, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                       vector<Usuario*>& usuarios, PilaAsignaciones& historial,
                       ColaSolicitudes& cola, BitacoraAsignaciones& bitacora);

#endif
