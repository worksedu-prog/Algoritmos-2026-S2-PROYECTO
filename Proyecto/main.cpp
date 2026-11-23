//Main.cpp
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include "Usuario/Usuario.h"
#include "Materia/Materia.h"
#include "Solicitud/Solicitud.h"
#include "Asignaciones/Asignaciones.h"
#include "GestorArchivos/GestorArchivos.h"
#include "Grafo/Grafo.h"
#include "Menus/Menus.h"

using namespace std;

int main() {

    //Se Instancia el GestorArchivos y estructuras de apoyo
    GestorArchivos gestorArchivos;
    ColaSolicitudes cola;
    PilaAsignaciones historialAsignaciones;
    BitacoraAsignaciones bitacora;               // NUEVO: registro de asignaciones y reversiones

    // Vectores dinámicos en memoria
    vector<Usuario*> usuarios;
    vector<Materia> materias;
    vector<Solicitud> solicitudes;
    int contadorSolicitudes = 0;

    // Avisar antes de iniciar si falta lo necesario
    if (!gestorArchivos.verificarEntorno()) return 1;

    // Cargar los datos desde los .txt a los vectores
    gestorArchivos.cargarUsuarios(usuarios);
    gestorArchivos.cargarMaterias(materias);
    gestorArchivos.cargarSolicitudes(solicitudes);
    gestorArchivos.cargarBitacora(bitacora);
    gestorArchivos.cargarEstadosMaterias(usuarios);
    gestorArchivos.cargarAsignaciones(solicitudes, usuarios);
    for (size_t i = 0; i < solicitudes.size(); i++) {
        if (solicitudes[i].getId() > contadorSolicitudes) {
            contadorSolicitudes = solicitudes[i].getId();
        }
    }

    // Las materias se mantienen ordenadas por codigo (necesario para la busqueda binaria)
    ordenarMergeSort(materias, POR_CODIGO);

    // Se reconstruye la cola con prioridad con las solicitudes que siguen pendientes
    for (size_t i = 0; i < solicitudes.size(); i++) {
        if (solicitudes[i].getId() > contadorSolicitudes) contadorSolicitudes = solicitudes[i].getId();
        
        // Una "Asignada" sin asignacion guardada (archivo viejo o monitor inexistente) vuelve a "Pendiente"
        if (solicitudes[i].getEstado() == "Asignada" && !solicitudes[i].tieneBloque()) {
            solicitudes[i].setEstado("Pendiente");
            bitacora.registrar("REVERSION automatica de la solicitud #" + to_string(solicitudes[i].getId()) + " al reiniciar el programa (no habia una asignacion guardada valida)");
        }

        if (solicitudes[i].getEstado() == "Asignada" && solicitudes[i].tieneBloque()) {
            Monitor* asignado = buscarMonitorPorCodigo(usuarios, solicitudes[i].getCodigoMonitor());
            if (asignado != nullptr && !asignado->isActivo()) {
                solicitudes[i].setEstado("Pendiente");
                solicitudes[i].limpiarSesion(); 
                asignado->setSolicitudActual(0);
                bitacora.registrar("REVERSION automatica de la solicitud #" + to_string(solicitudes[i].getId()) +
                                   " al reiniciar el programa (el monitor " + asignado->getUsername() + " esta inactivo)");
            }
        }
    }
    // La cola se arma con la prioridad de 3 criterios (semestre, grafo de prerrequisitos, antiguedad)
    recalcularPrioridades(solicitudes, usuarios, materias, cola);

    configurarGuardado(gestorArchivos, usuarios, materias, solicitudes, bitacora);

    // Se muestra el menú informativo para pruebas
    menuPruebas();

    // c) Mostrar el menuGeneral() y gestionar el inicio de sesión
    int opcionGeneral = -1;

    while (opcionGeneral != 0) {
        menuGeneral();
        cout << "Seleccione una opcion: ";
        if (!(cin >> opcionGeneral)) {
            if (cin.eof()) break;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opcionGeneral = -1;
            cout << "Opcion invalida." << endl;
            continue;
        }

        if (opcionGeneral == 1) {
            string user, pass;
            cout << "Usuario: ";
            cin >> user;
            cout << "Contrasena: ";
            cin >> pass;

            Usuario* encontrado = nullptr;
            for (Usuario* u : usuarios) {
                if (u->autenticarUsuario(user, pass)) { encontrado = u; break; }
            }

            // Distingue credenciales incorrectas de usuario inactivo
            bool inactivo = false;
            for (Usuario* u : usuarios) {
                if (u->getUsername() == user && u->getPassword() == pass && !u->isActivo()) inactivo = true;
            }
            if (!encontrado && inactivo) {
                cout << "El usuario esta inactivo. Contacte al administrador." << endl;
            } else if (!encontrado) {
                cout << "Usuario o contrasena incorrectos." << endl;
            } else {
                encontrado->informacion();
                string rol = encontrado->getRol();
                if (rol == "Administrador") {
                    // CAMBIO: se pasan tambien la cola y la bitacora
                    menuAdministrador(*static_cast<Administrador*>(encontrado), materias, solicitudes,
                                      usuarios, historialAsignaciones, cola, bitacora);
                } else if (rol == "Estudiante") {
                    menuEstudiante(*static_cast<Estudiante*>(encontrado), materias, solicitudes,
                                   cola, contadorSolicitudes);
                } else if (rol == "Profesor") {
                    menuProfesor(*static_cast<Profesor*>(encontrado), materias, solicitudes, usuarios);
                } else if (rol == "Monitor") {
                    menuMonitor(*static_cast<Monitor*>(encontrado), solicitudes, cola);
                }
            }
        } else if (opcionGeneral != 0) {
            cout << "Opcion invalida." << endl;
        }
    }

    //Al cerrar el programa, guardar cambios en los .txt
    gestorArchivos.guardarUsuarios(usuarios);
    gestorArchivos.guardarMaterias(materias);
    gestorArchivos.guardarSolicitudes(solicitudes);
    gestorArchivos.guardarBitacora(bitacora);
    gestorArchivos.guardarEstadosMaterias(usuarios);
    gestorArchivos.guardarAsignaciones(solicitudes);
    // Liberación de memoria dinámica
    for (Usuario* u : usuarios) {delete u;}
    usuarios.clear();
    cout << "\nGracias por usar el sistema. Hasta pronto!" << endl;
    return 0;
}
