// Asignaciones.cpp
#include "../include/Asignaciones.h"
#include "../include/Algoritmos.h"

void BitacoraAsignaciones::mostrar() const {
    if (registros.empty()) {
        cout << "La bitacora esta vacia." << endl;
        return;
    }
    for (int i = 0; i < registros.size(); i++) {
        cout << (i + 1) << ". " << registros[i] << endl;
    }
}

void asignarMateriaAMonitor(Monitor& mon, const string& codigoMateria,
                            PilaAsignaciones& historial, BitacoraAsignaciones& bitacora) {
    Asignacion a;
    a.monitor = &mon;
    a.materiaAnterior = mon.getMateriaAsignada();
    a.materiaNueva = codigoMateria;
    a.idSolicitud = 0;                 // solo cambia la materia
    historial.push(a);                 // se guarda ANTES de cambiar, para poder deshacer
    mon.asignarMateria(codigoMateria);

    bitacora.registrar("ASIGNACION de materia " + codigoMateria + " al monitor " + mon.getUsername());
    cout << "Se asigno la materia " << codigoMateria << " a " << mon.getNombres() << "." << endl;
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
    if (mon.getMateriaAsignada() != solicitudes[pos].getCodigoMateria()) {
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
    a.materiaAnterior = mon.getMateriaAsignada();
    a.materiaNueva = mon.getMateriaAsignada();
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

    // Caso 1: asignacion de materia -> restaurar la materia anterior
    if (ultima.idSolicitud == 0) {
        mon->asignarMateria(ultima.materiaAnterior);
        bitacora.registrar("REVERSION de la asignacion de materia " + ultima.materiaNueva +
                           " al monitor " + mon->getUsername());
        cout << "Se deshizo la asignacion de " << ultima.materiaNueva
             << " a " << mon->getNombres() << "." << endl;
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