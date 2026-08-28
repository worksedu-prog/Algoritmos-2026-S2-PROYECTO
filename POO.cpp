#include <iostream>
#include <string>
#include <vector>


using namespace std;

// 1. CLASE MATERIA
class Materia {
private:
    string codigoMateria;
    string nombre;
    int creditos;
    string tipo; // "Obligatoria" o "Electiva"
    string dependencia;
    vector<string> horarios;

public:
    Materia() {
        this->codigoMateria = "";
        this->creditos = 0;
        this->nombre = "";
        this->tipo = "";
        this->dependencia = "";
        //Falta crear el vector para guardar el horario
    }

    //Metodos getters
    string getCodigo() const { return codigoMateria; }
    string getNombre() const { return nombre; }
    string getTipo() const { return tipo; }

};

// 2. CLASE ESTUDIANTE
class Estudiante {
protected:
    string nombres, apellidos, carrera, doc, codigoEstudiante;
    int semestre;

public:
    Estudiante() {
        this->codigoEstudiante = "";
        this->doc = "";
        this->nombres = "";
        this->apellidos = "";
        this->semestre = 0;
        this->carrera = "";
    }

    //Metodos Getters
    string getCodigoEstudiante() const { return codigoEstudiante; }
    string getNombres() const { return nombres; }
    int getSemestre() const { return semestre; }
};

// 3. CLASE MONITOR (Hereda de Estudiante)
class Monitor : public Estudiante {
private:
    string codigoMonitor;
    string horarioDisponibilidad;

public:
    Monitor() : Estudiante() {
        this->codigoMonitor = "";
        this->horarioDisponibilidad = "";
        //Falta agregar la materia que va a dar el monitor
    }
    
    string getCodigoMonitor() const { return codigoMonitor; }
};


int main(){
    
}