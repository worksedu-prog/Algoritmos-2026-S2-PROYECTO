#include "../include/Menus.h"
#include "../include/Algoritmos.h"
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

// Registra un usuario nuevo de cualquier rol y lo agrega al vector
static void registrarUsuario(vector<Usuario*>& usuarios) {
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
        int semestre = leerRango("Semestre (1-12): ", 1, 12);
        codigo = maxEstudiante + 1;
        if (rol == 3) {
            nuevo = new Estudiante(nombres, apellidos, username, password, codigo, carrera, semestre);
        } else {
            string horario = leerTexto("Horario de disponibilidad: ");
            nuevo = new Monitor(nombres, apellidos, username, password, codigo, carrera, semestre,
                                maxMonitor + 1, horario);
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

void menuEstudiante(Estudiante& est, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                    ColaSolicitudes& cola, int& contadorSolicitudes) {
    int opcion = -1;
    while (opcion != 0) {
        cout << "\n--- MENU ESTUDIANTE (" << est.getNombres() << ") ---" << endl;
        cout << "1. Ver materias disponibles" << endl;
        cout << "2. Ver prerrequisitos de una materia" << endl;
        cout << "3. Crear solicitud de monitoria" << endl;
        cout << "4. Ver mis solicitudes" << endl;
        cout << "5. Cancelar una solicitud pendiente" << endl;
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

        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
}

void menuProfesor(Profesor& prof, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                  vector<Usuario*>& usuarios) {
    int opcion = -1;
    while (opcion != 0) {
        cout << "\n--- MENU PROFESOR (" << prof.getNombres() << " | " << prof.getDepartamento() << ") ---" << endl;
        cout << "1. Ver materias" << endl;
        cout << "2. Ver prerrequisitos de una materia" << endl;
        cout << "3. Ver solicitudes de una materia" << endl;
        cout << "4. Ver monitores y su materia asignada" << endl;
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
                string materia = mon->getMateriaAsignada().empty() ? "(sin asignar)" : mon->getMateriaAsignada();
                cout << mon->getNombres() << " " << mon->getApellidos() << " | Materia: " << materia
                     << " | Horario: " << mon->getHorario() << endl;
                hay = true;
            }
            if (!hay) cout << "No hay monitores registrados." << endl;

        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
}

void menuAdministrador(Administrador& admin, vector<Materia>& materias, vector<Solicitud>& solicitudes,
                       vector<Usuario*>& usuarios, PilaAsignaciones& historial) {
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
            if(tipo=="Obligatoria"){
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
            
            }else if(tipo=="Electiva"){}

            string codigo = generarCodigoMateria(nombre, materias);
            materias.push_back(Materia(codigo, nombre, creditos, tipo, previos));
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
            asignarMateriaAMonitor(*mon, codigo, historial);

        } else if (opcion == 7) {
            deshacerAsignacion(historial);
        

        } else if (opcion == 8) {
            registrarUsuario(usuarios);
        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
}
