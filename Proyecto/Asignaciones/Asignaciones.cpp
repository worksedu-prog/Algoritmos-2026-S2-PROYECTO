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
                              BitacoraAsignaciones& bitacora,
                              const vector<Usuario*>& usuarios, const vector<Materia>& materias) {
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
    // solo se asigna si hay un bloque de horario compatible
    const Estudiante* solicitante = buscarEstudiantePorCodigo(usuarios, solicitudes[pos].getCodigoEstudiante());
    BloqueHorario bloque;
    if (!buscarBloqueComun(mon, solicitante, solicitudes[pos].getCodigoMateria(), solicitudes, materias, 0, bloque)) {
        cout << "No hay un bloque de " << DURACION_SESION * 30 << " min compatible entre el monitor y el estudiante."
             << " La solicitud sigue pendiente." << endl;
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
    solicitudes[pos].setSesion(mon.getCodigoMonitor(), bloque);

    bitacora.registrar("ASIGNACION de la solicitud #" + to_string(idSolicitud) +
                       " (materia " + solicitudes[pos].getCodigoMateria() + ") al monitor " + mon.getUsername() +
                       " | " + nombreDia(bloque.dia) + " " + formatoHora(bloque.inicio) + " - " + formatoHora(bloque.fin));
    cout << "Solicitud #" << idSolicitud << " asignada a " << mon.getNombres() << " para el "
         << nombreDia(bloque.dia) << " " << formatoHora(bloque.inicio) << " - " << formatoHora(bloque.fin) << "." << endl;
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
    cola.encolar(solicitudes[pos].getId(), solicitudes[pos].getPrioridad());   // vuelve a la cola con su prioridad
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

// Emparejamiento con horario compatible
// Dos bloques se cruzan si son el mismo dia y sus horas se solapan
static bool seCruzan(const BloqueHorario& primero, const BloqueHorario& segundo) {
    return primero.dia == segundo.dia && primero.inicio < segundo.fin && segundo.inicio < primero.fin;
}

// true si en ese bloque el estudiante esta en la clase de la materia
static bool chocaConClase(const BloqueHorario& bloque, const Materia* materia) {
    if (materia == nullptr) return false;
    for (int franja = bloque.inicio; franja < bloque.fin; franja++) {
        if (materia->franjaOcupada(bloque.dia, franja)) return true;
    }
    return false;
}

// true si el monitor o el estudiante ya tienen otra sesion asignada que se cruza con el bloque
static bool chocaConOtraSesion(const BloqueHorario& bloque, const Monitor& monitor, const Estudiante* estudiante,
                               const vector<Solicitud>& solicitudes, int idIgnorar) {
    for (size_t i = 0; i < solicitudes.size(); i++) {
        const Solicitud& otra = solicitudes[i];
        bool tieneSesionVigente = (otra.getEstado() == "Asignada" && otra.tieneBloque());
        if (otra.getId() == idIgnorar || !tieneSesionVigente) continue;

        bool mismoMonitor = (otra.getCodigoMonitor() == monitor.getCodigoMonitor());
        bool mismoEstudiante = (estudiante != nullptr && otra.getCodigoEstudiante() == estudiante->getCodigoEstudiante());
        if ((mismoMonitor || mismoEstudiante) && seCruzan(otra.getBloque(), bloque)) return true;
    }
    return false;
}

bool buscarBloqueComun(const Monitor& mon, const Estudiante* est, const string& codigoMateria,
                       const vector<Solicitud>& solicitudes, const vector<Materia>& materias,
                       int idIgnorar, BloqueHorario& resultado) {
    long comparaciones = 0;
    int posMateria = buscarMateriaBinaria(materias, codigoMateria, comparaciones);
    const Materia* materia = (posMateria == -1) ? nullptr : &materias[posMateria];

    // Si el estudiante no registro disponibilidad, solo se usa la del monitor
    bool revisarEstudiante = (est != nullptr && !est->getDisponibilidadEstudiante().empty());

    // Se prueba cada posible inicio dentro de cada franja del monitor, en orden
    const vector<BloqueHorario>& franjasMonitor = mon.getDisponibilidad();
    for (size_t f = 0; f < franjasMonitor.size(); f++) {
        const BloqueHorario& franja = franjasMonitor[f];
        for (int inicio = franja.inicio; inicio + DURACION_SESION <= franja.fin; inicio++) {
            BloqueHorario candidato{franja.dia, inicio, inicio + DURACION_SESION};

            if (revisarEstudiante && !est->disponibleEn(candidato.dia, candidato.inicio, candidato.fin)) continue;
            if (chocaConClase(candidato, materia)) continue;
            if (chocaConOtraSesion(candidato, mon, est, solicitudes, idIgnorar)) continue;

            resultado = candidato;
            return true;
        }
    }
    return false;
}

bool emparejarSolicitud(int idSolicitud, vector<Solicitud>& solicitudes, ColaSolicitudes& cola,
                        PilaAsignaciones& historial, BitacoraAsignaciones& bitacora,
                        vector<Usuario*>& usuarios, const vector<Materia>& materias) {
    int pos = buscarSolicitudSecuencial(solicitudes, idSolicitud);
    if (pos == -1 || solicitudes[pos].getEstado() != "Pendiente") {
        cout << "No hay una solicitud pendiente con ese id." << endl;
        return false;
    }
    const Estudiante* solicitante = buscarEstudiantePorCodigo(usuarios, solicitudes[pos].getCodigoEstudiante());
    string codigoMateria = solicitudes[pos].getCodigoMateria();

    // Se revisan los monitores en el orden de la lista y se toma el primero que cumpla todo
    for (size_t i = 0; i < usuarios.size(); i++) {
        if (!esMonitor(usuarios[i])) continue;
        Monitor* monitor = static_cast<Monitor*>(usuarios[i]);   // el rol confirma que es Monitor

        bool puedeAtender = monitor->isActivo() && monitor->estaLibre() && monitor->dominaMateria(codigoMateria);
        if (!puedeAtender) continue;

        bool esElMismoEstudiante = (solicitante != nullptr &&
                                    solicitante->getCodigoEstudiante() == monitor->getCodigoEstudiante());
        if (esElMismoEstudiante) continue;   // un monitor no atiende su propia solicitud

        BloqueHorario bloqueLibre;
        if (!buscarBloqueComun(*monitor, solicitante, codigoMateria, solicitudes, materias, 0, bloqueLibre)) continue;

        cout << "Monitor encontrado: " << monitor->getUsername() << endl;
        return asignarSolicitudAMonitor(*monitor, idSolicitud, solicitudes, cola, historial, bitacora, usuarios, materias);
    }
    cout << "No hay monitor disponible (libre, que domine " << solicitudes[pos].getCodigoMateria()
         << " y con horario compatible). La solicitud #" << idSolicitud << " sigue pendiente." << endl;
    return false;
}

bool reasignarSolicitud(int idSolicitud, Monitor& nuevo, vector<Solicitud>& solicitudes,
                        PilaAsignaciones& historial, BitacoraAsignaciones& bitacora,
                        vector<Usuario*>& usuarios, const vector<Materia>& materias) {
    int pos = buscarSolicitudSecuencial(solicitudes, idSolicitud);
    if (pos == -1 || solicitudes[pos].getEstado() != "Asignada") {
        cout << "Solo se puede reasignar una solicitud en estado Asignada." << endl;
        return false;
    }
    Monitor* anterior = buscarMonitorConSolicitud(usuarios, idSolicitud);
    if (anterior == &nuevo) { cout << "La solicitud ya esta asignada a ese monitor." << endl; return false; }
    // El nuevo monitor domina la materia, esta libre y tiene horario compatible
    if (!nuevo.dominaMateria(solicitudes[pos].getCodigoMateria())) {
        cout << "El nuevo monitor no atiende la materia " << solicitudes[pos].getCodigoMateria() << "." << endl;
        return false;
    }
    if (!nuevo.estaLibre()) {
        cout << "El nuevo monitor ya tiene asignada la solicitud #" << nuevo.getSolicitudActual() << "." << endl;
        return false;
    }
    const Estudiante* solicitante = buscarEstudiantePorCodigo(usuarios, solicitudes[pos].getCodigoEstudiante());
    BloqueHorario bloqueNuevo;
    if (!buscarBloqueComun(nuevo, solicitante, solicitudes[pos].getCodigoMateria(), solicitudes, materias, idSolicitud, bloqueNuevo)) {
        cout << "El nuevo monitor no tiene un bloque compatible con el estudiante. No se reasigno." << endl;
        return false;
    }
    if (anterior) anterior->setSolicitudActual(0);   // la franja del anterior queda libre
    nuevo.setSolicitudActual(idSolicitud);
    solicitudes[pos].setSesion(nuevo.getCodigoMonitor(), bloqueNuevo);
    historial.actualizarMonitor(idSolicitud, &nuevo);  // no se deshacen asignaciones posteriores
    bitacora.registrar("REASIGNACION de la solicitud #" + to_string(idSolicitud) + " del monitor " +
                       (anterior ? anterior->getUsername() : string("(ninguno)")) + " al monitor " + nuevo.getUsername() +
                       " | " + nombreDia(bloqueNuevo.dia) + " " + formatoHora(bloqueNuevo.inicio) + " - " + formatoHora(bloqueNuevo.fin));
    cout << "Solicitud #" << idSolicitud << " reasignada a " << nuevo.getNombres() << " para el "
         << nombreDia(bloqueNuevo.dia) << " " << formatoHora(bloqueNuevo.inicio) << " - " << formatoHora(bloqueNuevo.fin) << "." << endl;
    return true;
}

int franjasLibresDeClase(const Monitor& mon, const Materia& materia) {
    int libres = 0;
    const vector<BloqueHorario>& franjas = mon.getDisponibilidad();
    for (size_t f = 0; f < franjas.size(); f++)
        for (int k = franjas[f].inicio; k < franjas[f].fin; k++)
            if (!materia.franjaOcupada(franjas[f].dia, k)) libres++;
    return libres;
}
