// Asignaciones.cpp
#include "Asignaciones.h"
#include "../Solicitud/Solicitud.h"

// Muestra las materias actuales de un monitor
static void mostrarMateriasMonitor(const Monitor& mon) {
    const vector<string>& lista = mon.getMaterias();
    cout << "Materias de " << mon.getNombres() << ": ";
    if (lista.empty()) cout << "(ninguna)";
    for (size_t i = 0; i < lista.size(); i++) cout << (i > 0 ? ", " : "") << lista[i]; //Las listas estan separadas por comas
    cout << endl;
}

// Dice si el monitor tiene asignada ahora una solicitud de esa materia
static bool materiaEnUso(const Monitor& mon, const string& codigo, const vector<Solicitud>& solicitudes) {
    if (mon.estaLibre()) return false;
    int pos = buscarSolicitudSecuencial(solicitudes, mon.getSolicitudActual());
    return pos != -1 && solicitudes[pos].getEstado() == "Asignada" && solicitudes[pos].getCodigoMateria() == codigo;
}

void BitacoraAsignaciones::mostrar() const {
    if (registros.empty()) {
        cout << "La bitacora esta vacia." << endl;
        return;
    }
    for (size_t i = 0; i < registros.size(); i++) {
        cout << (i + 1) << ". " << registros[i] << endl;
    }
}

void asignarMateriaAMonitor(Monitor& mon, const string& codigoMateria,
                            PilaAsignaciones& historial, BitacoraAsignaciones& bitacora) {
    if (mon.dominaMateria(codigoMateria)) {   //Si ya la tiene, no se apila ni se registra nada
        cout << "El monitor " << mon.getNombres() << " ya tiene asignada la materia " << codigoMateria << "." << endl;
        return;
    }
    Asignacion a;
    a.monitor = &mon;
    // Se eliminó la de línea a.materiaAnterior = mon.getMateriaAsignada(). Porque la lista ya no reemplaza materias y el campo materiaAnterior ya no existe
    a.materiaNueva = codigoMateria;
    a.idSolicitud = 0;                 // solo cambia la materia
    historial.push(a);                 // se guarda ANTES de cambiar, para poder deshacer
    mon.agregarMateria(codigoMateria);   // Agrega la materia a la lista en vez de reemplazar la anterior con asignarMateria

    bitacora.registrar("ASIGNACION de materia " + codigoMateria + " al monitor " + mon.getUsername());
    cout << "Se asigno la materia " << codigoMateria << " a " << mon.getNombres() << "." << endl;
    mostrarMateriasMonitor(mon);   // Muestra como queda la lista del monitor
}

bool asignarSolicitudAMonitor(Monitor& mon, int idSolicitud, vector<Solicitud>& solicitudes,
                              ColaSolicitudes& cola, PilaAsignaciones& historial,
                              BitacoraAsignaciones& bitacora) {
    int pos = buscarSolicitudSecuencial(solicitudes, idSolicitud);
    if (pos == -1) {
        cout << "La solicitud no existe." << endl;
        return false;
    }
    if (solicitudes[pos].getEstado() != "Pendiente") {
        cout << "Solo se pueden asignar solicitudes pendientes." << endl;
        return false;
    }
    if (!mon.dominaMateria(solicitudes[pos].getCodigoMateria())) {   //Ahora acepta cualquier materia de la lista del monitor, el mensaje y el resto del flujo no cambian
        cout << "El monitor no atiende la materia " << solicitudes[pos].getCodigoMateria() << "." << endl;
        return false;
    }
    if (!mon.estaLibre()) {
        cout << "El monitor ya tiene asignada la solicitud #" << mon.getSolicitudActual() << "." << endl;
        return false;
    }

    // Se apila el cambio (la materia no cambia: anterior == nueva)
    Asignacion a;
    a.monitor = &mon;
    //se elimina a.materiaAnterior
    a.materiaNueva = solicitudes[pos].getCodigoMateria();
    a.idSolicitud = idSolicitud;
    historial.push(a);

    solicitudes[pos].setEstado("Asignada");
    cola.eliminar(idSolicitud);              // sale de la cola de pendientes
    mon.setSolicitudActual(idSolicitud);     // ocupa la franja del monitor

    bitacora.registrar("ASIGNACION de la solicitud #" + to_string(idSolicitud) +
                       " al monitor " + mon.getUsername());
    cout << "Solicitud #" << idSolicitud << " asignada a " << mon.getNombres() << "." << endl;
    return true;
}

void deshacerAsignacion(PilaAsignaciones& historial, vector<Solicitud>& solicitudes,
                        ColaSolicitudes& cola, BitacoraAsignaciones& bitacora) {
    if (historial.isEmpty()) {
        cout << "No hay asignaciones para deshacer." << endl;
        return;
    }

    Asignacion ultima = historial.top();   // Leer la cima
    historial.pop();                       // Quitarla de la pila
    Monitor* mon = ultima.monitor;

    // Caso 1: asignacion de materia ahora quita esa materia de la lista del monitor
    if (ultima.idSolicitud == 0) {
        if (materiaEnUso(*mon, ultima.materiaNueva, solicitudes)) {   //No se puede quitar una materia que el monitor esta atendiendo
            historial.push(ultima);   //Devuelve la asignacion a la pila porque no se pudo revertir
            cout << "No se puede deshacer: el monitor tiene asignada la solicitud #" << mon->getSolicitudActual()
                 << " de la materia " << ultima.materiaNueva << ". Reviertala primero." << endl;
            return;
        }
        mon->quitarMateria(ultima.materiaNueva);   // Ahora quita esa materia de la lista
        bitacora.registrar("REVERSION de la asignacion de materia " + ultima.materiaNueva +
                           " al monitor " + mon->getUsername());
        cout << "Se deshizo la asignacion de " << ultima.materiaNueva
             << " a " << mon->getNombres() << "." << endl;
        mostrarMateriasMonitor(*mon);
        return;
    }

    // Caso 2: asignacion de solicitud
    int pos = buscarSolicitudSecuencial(solicitudes, ultima.idSolicitud);
    if (pos == -1 || solicitudes[pos].getEstado() != "Asignada") {
        // Ya fue atendida: no se puede "des-atender". Se descarta de la pila.
        cout << "No se puede deshacer: la solicitud #" << ultima.idSolicitud
             << " ya no esta asignada (fue atendida)." << endl;
        return;
    }

    solicitudes[pos].setEstado("Pendiente");
    cola.encolar(solicitudes[pos].getId(), solicitudes[pos].getUrgencia());   // vuelve a la cola
    mon->setSolicitudActual(0);                                              // libera la franja

    bitacora.registrar("REVERSION de la asignacion de la solicitud #" + to_string(ultima.idSolicitud) +
                       " al monitor " + mon->getUsername());
    cout << "Se deshizo la asignacion de la solicitud #" << ultima.idSolicitud
         << ". Vuelve a la cola como pendiente y " << mon->getNombres() << " queda libre." << endl;
}

bool deshacerMateriaElegida(Monitor& mon, const string& codigoMateria, vector<Solicitud>& solicitudes,
                            PilaAsignaciones& historial, BitacoraAsignaciones& bitacora) {   // Revierte una materia elegida, aunque no sea la ultima
    if (!mon.dominaMateria(codigoMateria)) {   //El monitor debe tener esa materia
        cout << "El monitor no tiene la materia " << codigoMateria << "." << endl;
        return false;
    }
    if (materiaEnUso(mon, codigoMateria, solicitudes)) {   // No se quita una materia mientras se atiende una solicitud de ella
        cout << "No se puede revertir " << codigoMateria << ": el monitor tiene asignada la solicitud #"
             << mon.getSolicitudActual() << " de esa materia. Reviertala primero." << endl;
        return false;
    }
    if (!historial.eliminarMateria(&mon, codigoMateria)) {   // Quita su registro de la pila, sin tocar los demas
        cout << "La materia " << codigoMateria << " no se asigno en esta sesion, no se puede revertir." << endl;   // Solo se revierte lo que esta en la pila
        return false;
    }
    mon.quitarMateria(codigoMateria);
    bitacora.registrar("REVERSION de la asignacion de materia " + codigoMateria + " al monitor " + mon.getUsername());   // Deja registrado en la bitacora
    cout << "Se deshizo la asignacion de " << codigoMateria << " a " << mon.getNombres() << "." << endl;
    mostrarMateriasMonitor(mon);
    return true;
}
