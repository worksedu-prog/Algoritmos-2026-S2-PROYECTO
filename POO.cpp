#include <iostream>
#include <string>
#include <vector>

using namespace std;

// CLASE BASE: USUARIO
class Usuario {
protected:
    string nombres;
    string apellidos;
    string username;
    string password;
    string rol;     // "Administrador", "Estudiante" o "Monitor"
    bool activo;

public:
    //Creación del constructor por defecto y el constructor con variables
    Usuario() {
        this->nombres = "";
        this->apellidos = "";
        this->username = "";
        this->password = "";
        this->rol = "";
        this->activo = true;
    }

    Usuario(string nombres, string apellidos, string username, string password, string rol) {
        this->nombres = nombres;
        this->apellidos = apellidos;
        this->username = username;
        this->password = password;
        this->rol = rol;
        this->activo = true;
    }

    //Verificación de usuario y contraseña, además de que el usuario esté activo
    bool autenticarUsuario(string usernameIngresado, string passwordIngresado) {
        return (this->username == usernameIngresado && this->password == passwordIngresado && this->activo);
    }

    void informacion() {
        cout << "Nombre: " << nombres << " " << apellidos << endl;
        cout << "Usuario: " << username << " | Rol: " << rol << endl;
    }
    //Getters
    string getUsername() const { return username; }
    string getRol() const { return rol; }
    string getNombres() const { return nombres; }
};

// CLASE ADMINISTRADOR (Hereda de Usuario)
class Administrador : public Usuario {
private:
    long codigoAdministrador;

public:
    Administrador() : Usuario() {
        this->codigoAdministrador = 0;
    }

    Administrador(string nombres, string apellidos, string username, string password, long codigoAdministrador)
        : Usuario(nombres, apellidos, username, password, "Administrador") {
        this->codigoAdministrador = codigoAdministrador;
    }

    long getCodigoAdministrador() const { return codigoAdministrador; }
};

// CLASE MATERIA
class Materia {
private:
    string codigoMateria;
    string nombre;
    int creditos;
    string tipo;         // "Obligatoria" o "Electiva"
    //Pendiente agregar la Dependencia y el vector de horarios
public:
    Materia() {
        this->codigoMateria = "";
        this->creditos = 0;
        this->nombre = "";
        this->tipo = "";
    }

    Materia(string codigoMateria, string nombre, int creditos, string tipo) {
        this->codigoMateria = codigoMateria;
        this->nombre = nombre;
        this->creditos = creditos;
        this->tipo = tipo;
    }

    // Getters
    string getCodigo() const { return codigoMateria; }
    string getNombre() const { return nombre; }
    string getTipo() const { return tipo; }

    void mostrarInfo() const {
        cout << "[" << codigoMateria << "] " << nombre << " - " << creditos << " creditos (" << tipo << ")";
        cout << endl;
    }
};

// CLASE ESTUDIANTE (Hereda de Usuario)
class Estudiante : public Usuario {
protected:
    string carrera;
    long codigoEstudiante;
    int semestre;
    vector<string> historialSolicitudes;

public:
    Estudiante() : Usuario() {
        this->codigoEstudiante = 0;
        this->semestre = 0;
        this->carrera = "";
    }

    Estudiante(string nombres, string apellidos, string username, string password,
               long codigoEstudiante, string carrera, int semestre)
        : Usuario(nombres, apellidos, username, password, "Estudiante") {
        this->codigoEstudiante = codigoEstudiante;
        this->carrera = carrera;
        this->semestre = semestre;
    }

    void agregarHistorial() {
        //Pendiente por implementar el metodo para agregar al historial de solicitudes
    }

    void verHistorial() const {
        //Pendiente por implementar el metodo para mostrar el historial de solicitudes
    }

    // Getters
    long getCodigoEstudiante() const { return codigoEstudiante; }
    string getCarrera() const { return carrera; }
    int getSemestre() const { return semestre; }
};

// CLASE MONITOR (Hereda de Estudiante)
class Monitor : public Estudiante {
private:
    long codigoMonitor;
    string horarioDisponibilidad;
    string materiaAsignada;

public:
    Monitor() : Estudiante() {
        this->codigoMonitor = 0;
        this->horarioDisponibilidad = "";
        this->materiaAsignada = "";
    }

    Monitor(string nombres, string apellidos, string username, string password,
            long codigoEstudiante, string carrera, int semestre,long codigoMonitor, string horarioDisponibilidad)
        : Estudiante(nombres, apellidos, username, password, codigoEstudiante, carrera, semestre) {
        this->codigoMonitor = codigoMonitor;
        this->horarioDisponibilidad = horarioDisponibilidad;
        this->materiaAsignada = "";
        this->rol = "Monitor";
    }

    void asignarMateria(string codigoMateria) {
        this->materiaAsignada = codigoMateria;
    }

    long getCodigoMonitor() const { return codigoMonitor; }
    string getHorario() const { return horarioDisponibilidad; }
    string getMateriaAsignada() const { return materiaAsignada; }
};

// CLASE SOLICITUD
class Solicitud {
/* Pendiente por implementar */
};

// CLASE COLA DE SOLICITUDES
class ColaSolicitudes {
/* Pendiente por implementar la cola que use un vector ordenado por nivel de urgencia*/
};


// CLASE GESTOR DE USUARIOS
class GestorUsuarios {
/* Pendiente por agregar el gestor de materias y los metodos para registrar y mostrar materias */
};

// MENUS POR ROL
void menuEstudiante(Estudiante &est) {
    int opcion = -1;
    while (opcion != 0) {
        cout << "\n--- MENU ESTUDIANTE (" << est.getNombres() << ") ---" << endl;
        cout << "1. Ver materias disponibles" << endl;
        cout << "2. Crear solicitud de monitoria" << endl;
        cout << "3. Ver mi historial de solicitudes" << endl;
        cout << "0. Cerrar sesion" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            cout << "Pendiente por implementar la visualización de materias disponibles." << endl;
        } else if (opcion == 2) {
            cout << "Pendiente por implementar la creación de la solicitud." << endl;
            //Pendiente por implementar la creación de la solicitud
        } else if (opcion == 3) {
            cout << "Pendiente por implementar la visualización del historial de solicitudes." << endl;
            //Pendiente por implementar la visualización del historial de solicitudes
        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
}

void menuMonitor(Monitor &mon) {
    int opcion = -1;
    while (opcion != 0) {
        cout << "\n--- MENU MONITOR (" << mon.getNombres() << ") ---" << endl;
        cout << "1. Ver todas las solicitudes" << endl;
        cout << "2. Atender siguiente solicitud pendiente" << endl;
        cout << "0. Cerrar sesion" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            cout << "Pendiente por implementar la visualización de todas las solicitudes." << endl;
            //Pendiente por implementar la visualización de todas las solicitudes
        } else if (opcion == 2) {
           cout << "Pendiente por implementar la atención de la siguiente solicitud pendiente." << endl;
            //Pendiente por implementar la atención de la siguiente solicitud pendiente
        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
}

void menuAdministrador(Administrador &admin) {
    int opcion = -1;
    while (opcion != 0) {
        cout << "\n--- MENU ADMINISTRADOR (" << admin.getNombres() << ") ---" << endl;
        cout << "1. Ver materias registradas" << endl;
        cout << "2. Registrar nueva materia" << endl;
        cout << "3. Ver todas las solicitudes" << endl;
        cout << "0. Cerrar sesion" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            cout << "Pendiente por implementar la visualización de materias registradas." << endl;
            //Pendiente por implementar la visualización de materias registradas
        } else if (opcion == 2) {
            cout << "Pendiente por implementar el registro de nuevas materias." << endl;
            //Pendiente por implementar el registro de nuevas materias
        } else if (opcion == 3) {
            cout << "Pendiente por implementar la visualización de todas las solicitudes." << endl;
            //Pendiente por implementar la visualización de todas las solicitudes
        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
};

void menuPruebas(){
    cout << "=====================================" << endl;
    cout << " SISTEMA DE ACOMPAÑAMIENTO ACADEMICO " << endl;
    cout << "      Universidad El Bosque          " << endl;
    cout << "=====================================" << endl;
    cout << "\nUsuarios base para realizar pruebas:" << endl;
    cout << " Administrador -> usuario: admin      | clave: admin123" << endl;
    cout << " Estudiante    -> usuario: estudiante  | clave: estu123" << endl;
    cout << " Monitor       -> usuario: monitor     | clave: mon123" << endl;

};
void menuGeneral(){
        cout << "\n--- INICIO DE SESION ---" << endl;
        cout << "1. Iniciar sesion" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
}
int main() {

    // Objetos de apoyo (todavia sin funcionalidad interna, pendientes por implementar)
    GestorUsuarios gestor;
    ColaSolicitudes cola;

    // Usuarios base para poder ingresar y probar los menus
    Administrador admin("David", "Arteaga", "admin", "admin123", 001);
    Estudiante estudiante("Tatiana", "Cabrera", "estudiante", "estu123", 001, "Ingenieria de Sistemas", 5);
    Monitor monitor("Julian", "Hernandez", "monitor", "mon123", 001, "Ingenieria de Sistemas", 8,001, "Lunes a Viernes 2pm-4pm");
    //Se muestra el menu de pruebas con los usuarios base para poder ingresar al sistema
    menuPruebas();

    int opcionGeneral = -1;
    while (opcionGeneral != 0) {
        //Se muestra el menu general para iniciar sesion o salir del sistema
        menuGeneral();
        cin >> opcionGeneral;
        //Se valida la opcion ingresada y se llama al menu correspondiente segun el rol del usuario
        if (opcionGeneral == 1) {
            string user, pass;
            cout << "Usuario: ";
            cin >> user;
            cout << "Contrasena: ";
            cin >> pass;

            // Se valida el usuario contra los tres usuarios base disponibles
            if (admin.autenticarUsuario(user, pass)) {
                admin.informacion();
                menuAdministrador(admin);
            } else if (estudiante.autenticarUsuario(user, pass)) {
                estudiante.informacion();
                menuEstudiante(estudiante);
            } else if (monitor.autenticarUsuario(user, pass)) {
                monitor.informacion();
                menuMonitor(monitor);
            } else {
                cout << "Usuario o contrasena incorrectos." << endl;
            }
        } else if (opcionGeneral != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
    cout << "\nGracias por usar el sistema. Hasta pronto!" << endl;

    return 0;
}