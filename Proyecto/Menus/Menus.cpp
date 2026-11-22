//Menus.cpp
#include "Menus.h"
#include <iostream>
#include <algorithm>
#include <limits>

using namespace std;
// Lee un entero entre minimo y maximo; repite si la entrada no es valida. En fin de entrada devuelve minimo.
static int leerRango(const string& pregunta, int minimo, int maximo) {
    int valor;
    while (true) {
        cout << pregunta;
        if (cin >> valor && valor >= minimo && valor <= maximo) return valor;
        if (cin.eof()) return minimo;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida. Ingrese un numero entre " << minimo << " y " << maximo << "." << endl;
    }
}

// Lee una opcion de menu; si la entrada no es numerica devuelve -1 (opcion invalida)
static int leerOpcion() {
    int opcion;
    cout << "Seleccione una opcion: ";
    if (cin >> opcion) return opcion;
    if (cin.eof()) return 0;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return -1;
}

// Muestra las materias ordenadas con el criterio y el algoritmo que elija el usuario.
// Ordena una COPIA para no alterar el orden por codigo del vector original.
static void listarMaterias(const vector<Materia>& materias) {
    if (materias.empty()) {
        cout << "No hay materias registradas." << endl;
        return;
    }
    int criterio = leerRango("Ordenar por (1 = codigo, 2 = nombre, 3 = creditos): ", 1, 3);
    int algoritmo = leerRango("Algoritmo (1 = insercion, 2 = merge sort, 3 = quick sort): ", 1, 3);

    vector<Materia> copia = materias;
    long comparaciones = 0;
    string nombreAlgoritmo;
    if (algoritmo == 1) { comparaciones = ordenarInsercion(copia, criterio); nombreAlgoritmo = "insercion"; }
    else if (algoritmo == 2) { comparaciones = ordenarMergeSort(copia, criterio); nombreAlgoritmo = "merge sort"; }
    else { comparaciones = ordenarQuickSort(copia, criterio); nombreAlgoritmo = "quick sort"; }

    for (int i = 0; i < copia.size(); i++) copia[i].mostrarInfo();
    cout << "(" << nombreAlgoritmo << ": " << comparaciones << " comparaciones)" << endl;
}
// Muestra el horario de una materia: cuadricula (dias x franjas) y la lista de bloques
static void verHorarioMateria(const vector<Materia>& materias) {
    string codigo;
    cout << "Codigo de la materia: ";
    cin >> codigo;
    long c = 0;
    int pos = buscarMateriaBinaria(materias, codigo, c);
    if (pos == -1) {
        cout << "La materia no existe." << endl;
        return;
    }
    cout << materias[pos].getNombre() << ":" << endl;
    mostrarMatrizHorario(materias[pos]);
    cout << "Bloques:" << endl;
    mostrarHorarioMateria(materias, codigo);
}

// Arma el horario de la materia m: 1) horas presenciales por semana, 2) en cuantos dias se reparten,
// 3) para cada dia, la franja (hora de inicio y duracion). La suma de las duraciones = las horas indicadas.
static void pedirHorario(Materia& m) {
    int horas = leerRango("Horas presenciales por semana (0 = sin horario, maximo 12): ", 0, 12);
    if (horas == 0) return;

    int restantes = horas * 2;                                  // franjas de 30 min por ubicar
    int tope = (DIAS_SEMANA < restantes) ? DIAS_SEMANA : restantes;
    int cantDias = leerRango("En cuantos dias se distribuyen (1-" + to_string(tope) + "): ", 1, tope);
    bool diaUsado[DIAS_SEMANA] = {false, false, false, false, false, false};

    for (int i = 1; i <= cantDias; i++) {
        int dia;
        while (true) {
            dia = leerRango("Dia " + to_string(i) + " (1=Lunes, 2=Martes, 3=Miercoles, 4=Jueves, 5=Viernes, 6=Sabado): ", 1, 6) - 1;
            if (!diaUsado[dia]) break;
            cout << "Ese dia ya tiene horario. Elija otro." << endl;
            if (cin.eof()) return;
        }
        diaUsado[dia] = true;

        int duracion;                                           // en franjas de 30 minutos
        if (i == cantDias) {
            duracion = restantes;                               // el ultimo dia completa las horas
            cout << "Duracion de este dia: " << duracion * 30 << " minutos (lo que falta)." << endl;
        } else {
            int maxDur = restantes - (cantDias - i);            // deja al menos 30 min para cada dia restante
            duracion = leerRango("Duracion en bloques de 30 min (1-" + to_string(maxDur) + ", 2 = 1 hora): ", 1, maxDur);
        }

        while (true) {
            int hora = leerRango("Hora de inicio, formato 24h (6-19): ", HORA_INICIO_DIA, 19);
            int minuto = leerRango("Minutos (0 o 30): ", 0, 30);
            if (minuto != 0 && minuto != 30) { cout << "Use 0 o 30." << endl; continue; }
            int inicio = (hora - HORA_INICIO_DIA) * 2 + (minuto == 30 ? 1 : 0);
            if (inicio + duracion > FRANJAS_DIA) {
                cout << "El bloque se sale del horario (termina maximo a las 8:00 PM). Intente otra hora." << endl;
                if (cin.eof()) return;
                continue;
            }
            for (int f = inicio; f < inicio + duracion; f++) m.ocuparFranja(dia, f);
            break;
        }
        restantes -= duracion;
    }
}

// NUEVO: muestra las solicitudes ordenadas por urgencia, antiguedad o materia.
// Ordena una COPIA: el vector original (orden de llegada) no se altera.
static void listarSolicitudesOrdenadas(const vector<Solicitud>& solicitudes) {
    if (solicitudes.empty()) {
        cout << "No hay solicitudes registradas." << endl;
        return;
    }
    int criterio = leerRango("Ordenar por (1 = urgencia, 2 = antiguedad, 3 = materia): ", 1, 3);

    vector<Solicitud> copia = solicitudes;
    long comparaciones = ordenarSolicitudes(copia, criterio);

    for (int i = 0; i < copia.size(); i++) copia[i].mostrarInfo();
    cout << "(merge sort: " << comparaciones << " comparaciones)" << endl;
}

// Pide un codigo y muestra su cadena de prerrequisitos (recursivo)
static void verPrerrequisitos(const vector<Materia>& materias) {
    string codigo;
    cout << "Codigo de la materia: ";
    cin >> codigo;
    long c = 0;
    int pos = buscarMateriaBinaria(materias, codigo, c);
    if (pos == -1) {
        cout << "La materia no existe." << endl;
        return;
    }
    cout << materias[pos].getNombre() << ":" << endl;
    if (materias[pos].getPrerrequisitos().empty()) cout << "  No tiene prerrequisitos." << endl;
    else mostrarPrerrequisitos(materias, codigo);
}
// Lee una linea de texto no vacia y sin comas (la coma separa los campos en usuarios.txt)
static string leerTexto(const string& pregunta) {
    string texto;
    while (true) {
        cout << pregunta;
        cin >> ws;
        if (!getline(cin, texto)) return "";
        if (!texto.empty() && texto.find(',') == string::npos) return texto;
        cout << "El texto no puede estar vacio ni contener comas." << endl;
    }
}

// Pide un username y devuelve el monitor correspondiente, o nullptr si no existe
static Monitor* buscarMonitorPorUsername(vector<Usuario*>& usuarios) {   // Evita repetir la busqueda del monitor en las opciones 13 y 14
    string username;
    cout << "Username del monitor: ";   // Pide el username)
    cin >> username;
    for (Usuario* u : usuarios) {   //Recorre los usuarios
        if (u->getRol() == "Monitor" && u->getUsername() == username) return dynamic_cast<Monitor*>(u);   // Devuelve el monitor encontrado
    }
    cout << "Monitor no encontrado." << endl; 
    return nullptr;
}

// Texto con las materias de un monitor
static string textoMaterias(const Monitor& mon) {
    const vector<string>& lista = mon.getMaterias();   // Materias del monitor
    if (lista.empty()) return "(sin asignar)";   // Si no tiene ninguna
    string texto;
    for (size_t i = 0; i < lista.size(); i++) {   // Recorre la lista
        if (i > 0) texto += ", ";
        texto += lista[i];
    }
    return texto;
}

// Agrega franjas de disponibilidad (dia, inicio y fin) a un monitor
static void pedirDisponibilidad(Monitor& mon) {   // Captura la disponibilidad como franjas validadas
    string actual = mon.getHorario();
    cout << "Disponibilidad actual: " << (actual.empty() ? "(sin disponibilidad)" : actual) << endl;
    while (true) {
        int dia = leerRango("Dia de la franja (1=Lunes ... 6=Sabado, 0 para terminar): ", 0, 6);   // Dia de la franja
        if (dia == 0) break;
        int hIni = leerRango("Hora de inicio, formato 24h (6-19): ", HORA_INICIO_DIA, 19);   // Hora de inicio
        int mIni = leerRango("Minutos de inicio (0 o 30): ", 0, 30);   // Minutos de inicio
        int hFin = leerRango("Hora de fin, formato 24h (6-20): ", HORA_INICIO_DIA, 20);   //Hora de fin
        int mFin = leerRango("Minutos de fin (0 o 30): ", 0, 30);   //Minutos de fin
        if ((mIni != 0 && mIni != 30) || (mFin != 0 && mFin != 30)) {   //Solo se aceptan bloques de 30 minutos
            cout << "Los minutos deben ser 0 o 30." << endl;
            continue;   // Vuelve a pedir otra franja
        }
        int inicio = (hIni - HORA_INICIO_DIA) * 2 + (mIni == 30 ? 1 : 0);   // Convierte la hora de inicio a franja de 30 min
        int fin = (hFin - HORA_INICIO_DIA) * 2 + (mFin == 30 ? 1 : 0);   // Convierte la hora de fin a franja de 30 min
        if (fin <= inicio) {
            cout << "La hora de fin debe ser posterior a la de inicio. Franja rechazada." << endl;
            continue;   // Vuelve a pedir otra franja
        }
        if (fin > FRANJAS_DIA) {   // El horario termina maximo a las 8:00 PM
            cout << "La franja se sale del horario (maximo 8:00 PM)." << endl;
            continue;   // Vuelve a pedir otra franja
        } 
        if (!mon.agregarFranja(dia - 1, inicio, fin)) {
            cout << "La franja se cruza con otra ya registrada." << endl;
            continue;   // Vuelve a pedir otra franja
        }
        cout << "Franja agregada: " << nombreDia(dia - 1) << " " << formatoHora(inicio) << " - " << formatoHora(fin) << endl;
    }
    string nueva = mon.getHorario();
    cout << "Disponibilidad del monitor: " << (nueva.empty() ? "(sin disponibilidad)" : nueva) << endl;
}

// Pide las materias iniciales de un monitor recien registrado (no quedan en la pila de deshacer)
static void pedirMateriasMonitor(Monitor& mon, const vector<Materia>& materias) {
    while (true) {
        string codigo;
        cout << "Codigo de una materia que atiende el monitor (0 para terminar): ";
        if (!(cin >> codigo) || codigo == "0") break;
        long c = 0;
        if (buscarMateriaBinaria(materias, codigo, c) == -1) cout << "No existe esa materia." << endl; 
        else if (!mon.agregarMateria(codigo)) cout << "El monitor ya tiene esa materia." << endl;   // Rechaza repetidas
        else cout << "Materia " << codigo << " agregada." << endl;   // Confirma la materia
    }
    cout << "Materias del monitor: " << textoMaterias(mon) << endl;
}

// Opcion 7 del administrador: revierte la ultima asignacion; si es una materia de un monitor con varias, deja elegir cual
static void revertirAsignacion(PilaAsignaciones& historial, vector<Solicitud>& solicitudes,
                               ColaSolicitudes& cola, BitacoraAsignaciones& bitacora) {
    if (historial.isEmpty()) {   // Si no hay nada, deshacerAsignacion ya muestra el mensaje
        deshacerAsignacion(historial, solicitudes, cola, bitacora);
        return;
    }
    Asignacion ultima = historial.top();   // Lee la ultima asignacion sin sacarla
    Monitor* mon = ultima.monitor;   // Monitor afectado
    if (ultima.idSolicitud != 0 || mon->getMaterias().size() <= 1) {
        deshacerAsignacion(historial, solicitudes, cola, bitacora);
        return;
    }
    vector<string> opciones = historial.materiasRevertibles(mon);
    cout << "La ultima asignacion fue la materia " << ultima.materiaNueva << " del monitor " << mon->getUsername() << "." << endl;
    cout << "Materias actuales del monitor: " << textoMaterias(*mon) << endl;
    cout << "¿Que materia desea revertir?" << endl;
    for (size_t i = 0; i < opciones.size(); i++) {
        cout << "  " << (i + 1) << ". " << opciones[i] << (i == 0 ? " (ultima asignada)" : "") << endl;
    }
    cout << "  0. Cancelar" << endl;
    int eleccion = leerRango("Seleccione una opcion: ", 0, static_cast<int>(opciones.size()));
    if (eleccion == 0) {   //El usuario cancela
        cout << "Reversion cancelada. No se modifico nada." << endl;
        return;
    }
    deshacerMateriaElegida(*mon, opciones[eleccion - 1], solicitudes, historial, bitacora);   // Revierte solo la materia elegida
}

// Registra un usuario nuevo de cualquier rol y lo agrega al vector
static void registrarUsuario(vector<Usuario*>& usuarios, const vector<Materia>& materias) {
    int rol = leerRango("Rol (1 = Administrador, 2 = Profesor, 3 = Estudiante, 4 = Monitor): ", 1, 4);
    string nombres = leerTexto("Nombres: ");
    string apellidos = leerTexto("Apellidos: ");

    string username;
    bool repetido = true;
    while (repetido) {
        username = leerTexto("Username: ");
        repetido = false;
        for (size_t i = 0; i < usuarios.size(); i++) {
            if (usuarios[i]->getUsername() == username) repetido = true;
        }
        if (repetido) cout << "Ese username ya existe." << endl;
    }
    string password = leerTexto("Contrasena: ");
    if (nombres.empty() || apellidos.empty() || username.empty() || password.empty()) {
        cout << "Registro cancelado." << endl;
        return;
    }

    // Mayor codigo existente de cada tipo, para generar el siguiente
    long maxAdmin = 0, maxEstudiante = 0, maxMonitor = 0;
    for (size_t i = 0; i < usuarios.size(); i++) {
        if (Administrador* a = dynamic_cast<Administrador*>(usuarios[i]))
            maxAdmin = max(maxAdmin, a->getCodigoAdministrador());
        if (Estudiante* e = dynamic_cast<Estudiante*>(usuarios[i]))     // incluye a los monitores
            maxEstudiante = max(maxEstudiante, e->getCodigoEstudiante());
        if (Monitor* m = dynamic_cast<Monitor*>(usuarios[i]))
            maxMonitor = max(maxMonitor, m->getCodigoMonitor());
    }

    Usuario* nuevo = nullptr;
    long codigo = 0;
    if (rol == 1) {
        codigo = maxAdmin + 1;
        nuevo = new Administrador(nombres, apellidos, username, password, codigo);
    } else if (rol == 2) {
        string departamento = leerTexto("Departamento: ");
        nuevo = new Profesor(nombres, apellidos, username, password, departamento);
    } else {
        string carrera = leerTexto("Carrera: ");
        int semestre = leerRango("Semestre (1-9): ", 1, 9);
        codigo = maxEstudiante + 1;
        if (rol == 3) {
            nuevo = new Estudiante(nombres, apellidos, username, password, codigo, carrera, semestre);
        } else {
            string horario = "";
            nuevo = new Monitor(nombres, apellidos, username, password, codigo, carrera, semestre,
                                maxMonitor + 1, horario);
            Monitor* nuevoMonitor = static_cast<Monitor*>(nuevo);
            pedirDisponibilidad(*nuevoMonitor);
            while (nuevoMonitor->getDisponibilidad().empty() && !cin.eof()) {
                cout << "El monitor necesita al menos una franja de disponibilidad." << endl;
                pedirDisponibilidad(*nuevoMonitor);
            }
            pedirMateriasMonitor(*nuevoMonitor, materias);
        }
    }

    usuarios.push_back(nuevo);
    cout << "Usuario " << username << " registrado como " << nuevo->getRol();
    if (codigo != 0) cout << " (codigo " << codigo << ")";
    cout << "." << endl;
}

void menuPruebas() {
    cout << "-------------------------------------" << endl;
    cout << " SISTEMA DE ACOMPAÑAMIENTO ACADEMICO " << endl;
    cout << "      Universidad El Bosque          " << endl;
    cout << "-------------------------------------" << endl;
    cout << "\nUsuarios base para realizar pruebas:" << endl;
    cout << " Administrador -> usuario: admin       | clave: admin123" << endl;
    cout << " Estudiante    -> usuario: estudiante  | clave: estu123" << endl;
    cout << " Monitor       -> usuario: monitor     | clave: mon123" << endl;
    cout << " Profesor      -> usuario: profesor    | clave: prof123" << endl;
}

void menuGeneral() {
    cout << "\n--- INICIO DE SESION ---" << endl;
    cout << "1. Iniciar sesion" << endl;
    cout << "0. Salir" << endl;
}

void menuEstudiante(Estudiante& est, vector<Materia>& materias, vector<Solicitud>& solicitudes, ColaSolicitudes& cola, int& contadorSolicitudes) {
    int opcion = -1;
    while (opcion != 0) {
        cout << "\n--- MENU ESTUDIANTE (" << est.getNombres() << ") ---" << endl;
        cout << "1. Ver materias disponibles" << endl;
        cout << "2. Ver prerrequisitos de una materia" << endl;
        cout << "3. Crear solicitud de monitoria" << endl;
        cout << "4. Ver mis solicitudes" << endl;
        cout << "5. Cancelar una solicitud pendiente" << endl;
        cout << "6. Ver horario de una materia" << endl;
        cout << "0. Cerrar sesion" << endl;
        opcion = leerOpcion();

        if (opcion == 1) {
            listarMaterias(materias);

        } else if (opcion == 2) {
            for (int i = 0; i < materias.size(); i++) materias[i].mostrarCodigo();
            verPrerrequisitos(materias);

        } else if (opcion == 3) {
            string codigoMateria, descripcion;
            for (int i = 0; i < materias.size(); i++) materias[i].mostrarCodigo();
            cout << "Codigo de la materia: ";
            cin >> codigoMateria;
            long c = 0;
            if (buscarMateriaBinaria(materias, codigoMateria, c) == -1) {
                cout << "La materia no existe." << endl;
                continue;
            }
            //AGREGADO TEMPORALMENTE PARA PRUEBAS!!!!!!!
            int urgencia = leerRango("Urgencia (1 = baja, 2 = media, 3 = alta): ", 1, 3);

            cout << "Describe en que necesitas ayuda: ";
            cin >> ws;
            getline(cin, descripcion);

            contadorSolicitudes++;
            solicitudes.push_back(Solicitud(contadorSolicitudes, est.getCodigoEstudiante(),
                                            codigoMateria, urgencia, descripcion));
            cola.encolar(contadorSolicitudes, urgencia);
            cout << "Solicitud #" << contadorSolicitudes << " creada. Posicion de la cola: "
                 << cola.tamano() << " pendientes en total." << endl;

        } else if (opcion == 4) {
            bool hay = false;
            for (int i = 0; i < solicitudes.size(); i++) {
                if (solicitudes[i].getCodigoEstudiante() == est.getCodigoEstudiante()) {
                    solicitudes[i].mostrarInfo();
                    hay = true;
                }
            }
            if (!hay) cout << "Aun no tienes solicitudes." << endl;

        } else if (opcion == 5) {
            int id = leerRango("Id de la solicitud a cancelar: ", 1, numeric_limits<int>::max());
            int pos = buscarSolicitudSecuencial(solicitudes, id);
            if (pos == -1 || solicitudes[pos].getCodigoEstudiante() != est.getCodigoEstudiante()) {
                cout << "No tienes una solicitud con ese id." << endl;
            } else if (solicitudes[pos].getEstado() != "Pendiente") {
                cout << "Solo se pueden cancelar solicitudes pendientes." << endl;
            } else {
                solicitudes[pos].setEstado("Cancelada");
                cola.eliminar(id);
                cout << "Solicitud #" << id << " cancelada." << endl;
            }

        } else if (opcion == 6) {
            verHorarioMateria(materias);
        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
}

void menuMonitor(Monitor& mon, vector<Solicitud>& solicitudes, ColaSolicitudes& cola) {
    int opcion = -1;
    while (opcion != 0) {
        cout << "\n--- MENU MONITOR (" << mon.getNombres() << ") ---" << endl;
        cout << "1. Ver todas las solicitudes" << endl;
        cout << "2. Ver cola de pendientes (en orden de atencion)" << endl;
        cout << "3. Atender siguiente solicitud" << endl;
        cout << "4. Terminar mi solicitud asignada" << endl;          // NUEVO
        cout << "5. Ver solicitudes ordenadas" << endl;               // NUEVO
        cout << "0. Cerrar sesion" << endl;
        opcion = leerOpcion();

        if (opcion == 1) {
            if (solicitudes.empty()) cout << "No hay solicitudes registradas." << endl;
            for (int i = 0; i < solicitudes.size(); i++) solicitudes[i].mostrarInfo();

        } else if (opcion == 2) {
            const vector<ItemCola>& items = cola.getElementos();
            if (items.empty()) cout << "No hay solicitudes pendientes." << endl;
            for (int i = 0; i < items.size(); i++) {
                int pos = buscarSolicitudSecuencial(solicitudes, items[i].id);
                cout << (i + 1) << ". ";
                if (pos != -1) solicitudes[pos].mostrarInfo();
            }

        } else if (opcion == 3) {
            int id = cola.frente();
            int pos = (id == -1) ? -1 : buscarSolicitudSecuencial(solicitudes, id);
            if (pos == -1) {
                cout << "No hay solicitudes pendientes." << endl;
            } else {
                cola.desencolar();
                solicitudes[pos].setEstado("Atendida");
                cout << "Solicitud atendida:" << endl;
                solicitudes[pos].mostrarInfo();
            }

        } else if (opcion == 4) {
            // NUEVO: el monitor termina la solicitud que le asigno el administrador
            int id = mon.getSolicitudActual();
            int pos = (id == 0) ? -1 : buscarSolicitudSecuencial(solicitudes, id);
            if (pos == -1) {
                cout << "No tienes ninguna solicitud asignada." << endl;
            } else {
                solicitudes[pos].setEstado("Atendida");
                mon.setSolicitudActual(0);          // la franja queda libre
                cout << "Solicitud atendida:" << endl;
                solicitudes[pos].mostrarInfo();
            }

        } else if (opcion == 5) {
            // NUEVO: ordenar solicitudes por urgencia, antiguedad o materia
            listarSolicitudesOrdenadas(solicitudes);

        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
}

void menuProfesor(Profesor& prof, vector<Materia>& materias, vector<Solicitud>& solicitudes,vector<Usuario*>& usuarios) {
    int opcion = -1;
    while (opcion != 0) {
        cout << "\n--- MENU PROFESOR (" << prof.getNombres() << " | " << prof.getDepartamento() << ") ---" << endl;
        cout << "1. Ver materias" << endl;
        cout << "2. Ver prerrequisitos de una materia" << endl;
        cout << "3. Ver solicitudes de una materia" << endl;
        cout << "4. Ver monitores y su materia asignada" << endl;
        cout << "5. Ver horario de una materia" << endl;
        cout << "0. Cerrar sesion" << endl;
        opcion = leerOpcion();

        if (opcion == 1) {
            listarMaterias(materias);

        } else if (opcion == 2) {
            verPrerrequisitos(materias);

        } else if (opcion == 3) {
            string codigo;
            cout << "Codigo de la materia: ";
            cin >> codigo;
            long c = 0;
            if (buscarMateriaBinaria(materias, codigo, c) == -1) {
                cout << "La materia no existe." << endl;
                continue;
            }
            bool hay = false;
            for (int i = 0; i < solicitudes.size(); i++) {
                if (solicitudes[i].getCodigoMateria() == codigo) {
                    solicitudes[i].mostrarInfo();
                    hay = true;
                }
            }
            if (!hay) cout << "No hay solicitudes para esa materia." << endl;

        } else if (opcion == 4) {
            bool hay = false;
            for (Usuario* u : usuarios) {
                Monitor* mon = dynamic_cast<Monitor*>(u);
                if (!mon) continue;
                string materia = textoMaterias(*mon);
                cout << mon->getNombres() << " " << mon->getApellidos() << " | Materias: " << materia
                     << " | Horario: " << mon->getHorario() << endl;
                hay = true;
            }
            if (!hay) cout << "No hay monitores registrados." << endl;

        } else if (opcion == 5) {
            verHorarioMateria(materias);
        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
}

// CAMBIO: recibe tambien la cola y la bitacora
void menuAdministrador(Administrador& admin, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                       vector<Usuario*>& usuarios, PilaAsignaciones& historial,
                       ColaSolicitudes& cola, BitacoraAsignaciones& bitacora) {
    int opcion = -1;
    while (opcion != 0) {
        cout << "\n--- MENU ADMINISTRADOR (" << admin.getNombres() << ") ---" << endl;
        cout << "1. Ver materias registradas" << endl;
        cout << "2. Registrar nueva materia" << endl;
        cout << "3. Buscar materia por codigo" << endl;
        cout << "4. Ver prerrequisitos de una materia" << endl;
        cout << "5. Ver todas las solicitudes" << endl;
        cout << "6. Asignar materia a un monitor" << endl;
        cout << "7. Deshacer ultima asignacion" << endl;
        cout << "8. Registrar nuevo usuario" << endl;
        cout << "9. Asignar solicitud pendiente a un monitor" << endl;     
        cout << "10. Ver bitacora de asignaciones" << endl;                
        cout << "11. Ver solicitudes ordenadas" << endl;                   
        cout << "12. Ver horario de una materia" << endl;
        cout << "13. Ver materias de un monitor" << endl;
        cout << "14. Definir disponibilidad de un monitor" << endl;
        cout << "0. Cerrar sesion" << endl;
        opcion = leerOpcion();

        if (opcion == 1) {
            listarMaterias(materias);

        } else if (opcion == 2) {
            string nombre;
            cout << "Nombre: ";
            cin >> ws;
            getline(cin, nombre);
            if (nombre.empty()) {
                cout << "El nombre no puede estar vacio." << endl;
                continue;
            }
            int creditos = leerRango("Creditos (1-5): ", 1, 5);
            string tipo = (leerRango("Tipo (1 = Obligatoria, 2 = Electiva): ", 1, 2) == 1) ? "Obligatoria" : "Electiva";
            vector<string> previos;
            //Asignación de prerequisitos
            if (!materias.empty()) {
                    for (int i = 0; i < materias.size(); i++) materias[i].mostrarInfo();
                    while (true) {
                        string codigoPre;
                        cout << "Codigo de un prerrequisito (0 para terminar): ";
                        cin >> codigoPre;
                        if (codigoPre == "0" || cin.eof()) break;
                        long c = 0;
                        if (buscarMateriaBinaria(materias, codigoPre, c) == -1) cout << "No existe esa materia." << endl;
                        else if (find(previos.begin(), previos.end(), codigoPre) != previos.end()) cout << "Ya lo agregaste." << endl;
                        else previos.push_back(codigoPre);
                    }
            }
            

            string codigo = generarCodigoMateria(nombre, materias);
            Materia nueva(codigo, nombre, creditos, tipo, previos);
            cout << "--- Horario de la materia ---" << endl;
            pedirHorario(nueva);
            materias.push_back(nueva);
            ordenarInsercion(materias, POR_CODIGO);     // casi ordenado: deja la nueva en su lugar
            cout << "Materia registrada. Codigo: " << codigo << endl;

        } else if (opcion == 3) {
            string codigo;
            cout << "Codigo a buscar: ";
            cin >> codigo;
            long compSec = 0, compBin = 0;
            int posSec = buscarMateriaSecuencial(materias, codigo, compSec);
            int posBin = buscarMateriaBinaria(materias, codigo, compBin);
            if (posBin == -1) cout << "No se encontro la materia." << endl;
            else materias[posBin].mostrarInfo();
            cout << "(secuencial: " << compSec << " comparaciones | binaria: " << compBin << " comparaciones)" << endl;
            (void)posSec;

        } else if (opcion == 4) {
            verPrerrequisitos(materias);

        } else if (opcion == 5) {
            if (solicitudes.empty()) cout << "No hay solicitudes registradas." << endl;
            for (int i = 0; i < solicitudes.size(); i++) solicitudes[i].mostrarInfo();

        } else if (opcion == 6) {
            string username, codigo;
            cout << "Username del monitor: ";
            cin >> username;
            Monitor* mon = nullptr;
            for (Usuario* u : usuarios) {
                if (u->getRol() == "Monitor" && u->getUsername() == username) {
                    mon = dynamic_cast<Monitor*>(u);
                    break;
                }
            }
            if (!mon) {
                cout << "Monitor no encontrado." << endl;
                continue;
            }
            cout << "Codigo de la materia: ";
            cin >> codigo;
            long c = 0;
            if (buscarMateriaBinaria(materias, codigo, c) == -1) {
                cout << "La materia no existe." << endl;
                continue;
            }
            //Ahora tambien recibe la bitacora
            asignarMateriaAMonitor(*mon, codigo, historial, bitacora);

        } else if (opcion == 7) {
            revertirAsignacion(historial, solicitudes, cola, bitacora);

        } else if (opcion == 8) {
            registrarUsuario(usuarios, materias);

        } else if (opcion == 9) {
            // NUEVO: asignar una solicitud pendiente a un monitor
            string username;
            cout << "Username del monitor: ";
            cin >> username;
            Monitor* mon = nullptr;
            for (Usuario* u : usuarios) {
                if (u->getRol() == "Monitor" && u->getUsername() == username) {
                    mon = dynamic_cast<Monitor*>(u);
                    break;
                }
            }
            if (!mon) {
                cout << "Monitor no encontrado." << endl;
                continue;
            }
            int id = leerRango("Id de la solicitud pendiente: ", 1, numeric_limits<int>::max());
            asignarSolicitudAMonitor(*mon, id, solicitudes, cola, historial, bitacora);

        } else if (opcion == 10) {
            // NUEVO: ver el registro de asignaciones y reversiones
            bitacora.mostrar();

        } else if (opcion == 11) {
            // NUEVO: ordenar solicitudes por urgencia, antiguedad o materia
            listarSolicitudesOrdenadas(solicitudes);

        } else if (opcion == 12) {
            verHorarioMateria(materias);
        } else if (opcion == 13) {   // Ver las materias de un monitor)
            Monitor* mon = buscarMonitorPorUsername(usuarios);
            if (mon) cout << "Materias de " << mon->getUsername() << ": " << textoMaterias(*mon) << endl; 

        } else if (opcion == 14) {   // Definir la disponibilidad de un monitor)
            Monitor* mon = buscarMonitorPorUsername(usuarios); 
            
            if (mon) pedirDisponibilidad(*mon);
        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
}
