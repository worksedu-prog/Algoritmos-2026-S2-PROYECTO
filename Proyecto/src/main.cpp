#include <iostream>
#include <string>
#include <vector>
#include "../include/Usuario.h"
#include "../include/Materia.h"
#include "../include/Solicitud.h"
#include "../include/GestorArchivos.h"
#include "../include/Grafo.h"
#include "../include/Algoritmos.h"
#include "../include/Menus.h"
#include <limits>

using namespace std;

int main() {

    //Se Instancia el GestorArchivos y estructuras de apoyo
    GestorArchivos gestorArchivos;
    ColaSolicitudes cola;
    PilaAsignaciones historialAsignaciones;

    // Vectores dinámicos en memoria
    vector<Usuario*> usuarios;
    vector<Materia> materias;
    vector<Solicitud> solicitudes;
    int contadorSolicitudes = 0;

    // Cargar los datos desde los .txt a los vectores
    gestorArchivos.cargarUsuarios(usuarios);
    gestorArchivos.cargarMaterias(materias);
    gestorArchivos.cargarSolicitudes(solicitudes);
    for (int i = 0; i < solicitudes.size(); i++) {
        if (solicitudes[i].getId() > contadorSolicitudes) {
            contadorSolicitudes = solicitudes[i].getId();
        }
    }

    // Las materias se mantienen ordenadas por codigo (necesario para la busqueda binaria)
    ordenarMergeSort(materias, POR_CODIGO);

    // Se reconstruye la cola FIFO con las solicitudes que siguen pendientes
  for (int i = 0; i < solicitudes.size(); i++) {
        if (solicitudes[i].getId() > contadorSolicitudes) contadorSolicitudes = solicitudes[i].getId();
        if (solicitudes[i].getEstado() == "Pendiente") {
            cola.encolar(solicitudes[i].getId(), solicitudes[i].getUrgencia());
        }
    }

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

            if (!encontrado) {
                cout << "Usuario o contrasena incorrectos." << endl;
            } else {
                encontrado->informacion();
                string rol = encontrado->getRol();
                if (rol == "Administrador") {
                    menuAdministrador(*static_cast<Administrador*>(encontrado), materias, solicitudes,
                                      usuarios, historialAsignaciones);
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

    // Liberación de memoria dinámica
    for (Usuario* u : usuarios) {delete u;}
    usuarios.clear();
    cout << "\nGracias por usar el sistema. Hasta pronto!" << endl;
    return 0;
}