# Sistema de Acompañamiento Académico

Aplicación de consola en **C++17** para gestionar solicitudes de monitorías académicas. No usa internet ni un motor de base de datos: toda la información se guarda en archivos de texto, y las estructuras de datos (cola de prioridad, pila, tabla hash, árbol y grafo) son implementaciones propias.

Autores: Julian David Hernández y David Santiago Arteaga. Curso: Algoritmos y Estructuras de Datos, Universidad del Rosario. Docente: Tatiana Cabrera.

## Funcionalidades

- **Cuatro roles:** Administrador (30 opciones), Estudiante, Monitor y Profesor, cada uno con su propio menú.
- **Solicitudes con prioridad calculada:** P = 0,30·semestre + 0,40·dependencia + 0,30·antigüedad. El estudiante ya no elige la urgencia.
- **Emparejamiento automático:** asigna la solicitud a un monitor libre que domine la materia, en un bloque de 1 h que no se cruce con la clase ni con otras sesiones.
- **Agenda semanal, informe de cobertura y bitácora:** la bitácora solo acumula registros, y la pila permite deshacer asignaciones.
- **Red de prerrequisitos y jerarquía:** muestra qué materias quedan bloqueadas al reprobar o retirar una, y organiza el catálogo en un árbol Área → Nivel → Materia.
- **Búsqueda en O(1) promedio:** estudiantes y monitores se buscan por código en una tabla hash.

## Novedades respecto a la versión del 18 de octubre de 2026

| Cambio | Detalle |
|---|---|
| Nuevo: `TablaHashUsuarios` | Encadenamiento, 101 cubetas iniciales, redimensiona al superar un factor de carga de 0,75 (RF-05, RNF-02) |
| Nuevo: `JerarquiaMaterias` | Árbol hijo-izquierdo/hermano-derecho; `Materia` ahora tiene `area` y `nivel` (RF-07) |
| Nuevo: `GrafoPrerrequisitos` | Lista de adyacencia, BFS, conteo de dependientes, materias afectadas, detección de ciclos (RF-08) |
| Nuevo: prioridad de 3 criterios | `Solicitud` ahora tiene `fechaCreacion` y `prioridad`; la cola ordena por puntaje |
| Nuevo: disponibilidad en franjas | Monitor y estudiante; validación de cruces |
| Nuevo: emparejamiento, reasignación, agenda, cobertura | Opciones 21, 22, 25 y 29 del administrador |
| Nuevo: estados académicos | Aprobada, Reprobada o Retirada por estudiante, en `estados_materias.txt` |
| Nuevo: activar o desactivar usuarios | Las sesiones de un monitor inactivo vuelven a la cola |
| Cambio: monitor con varias materias | La lista `materiasMonitor` reemplaza a `materiaAsignada` |
| Cambio: persistencia completa | Se guardan `bitacora.txt` y `asignaciones.txt`, y se guarda tras cada acción |
| Cambio: estructura de carpetas | Una carpeta por módulo (antes `include/` y `src/`); `Algoritmos.cpp` se eliminó |
| Nuevo: `verificarEntorno()` | Avisa si falta `data/` o `usuarios.txt` antes de iniciar |

## Estructura del repositorio

```
Proyecto/
├── main.cpp                 Carga datos, sanea asignaciones, recalcula la cola, login y bucle principal
├── Usuario/                 Jerarquía de usuarios, TablaHashUsuarios, estados de materias
├── Materia/                 Materia, horario 6x28, búsqueda secuencial/binaria, inserción/merge/quick sort
├── Solicitud/               Solicitud, ColaSolicitudes, merge sort de solicitudes, cobertura
├── Asignaciones/            Pila de deshacer, bitácora, bloque común, emparejamiento, reasignación
├── Grafo/                   Red de prerrequisitos (BFS) y cálculo de prioridad
├── Jerarquia/               Árbol Área -> Nivel -> Materia
├── GestorArchivos/          Lectura y escritura de los archivos de data/
├── Menus/                   Menús por rol y guardado automático
├── data/                    usuarios, materias, solicitudes, asignaciones, estados_materias, bitacora (.txt)
└── docs/informe/            Informe IEEE (informe.tex, informe.pdf)
```

`programa.exe` y `Software.exe` son binarios de Windows compilados previamente; no forman parte del código fuente y se pueden regenerar.

## Requisitos

- Un compilador con soporte de C++17 (probado con **g++ 13.3**; en Windows sirve MinGW-w64).
- Solo se usa la biblioteca estándar; no hay dependencias externas.
- Opcional, para recompilar el informe: TeX Live con `IEEEtran`, `babel-spanish` y `tikz`.

## Compilación y ejecución

```bash
git clone <url-del-repositorio>
cd Proyecto
g++ -std=c++17 -Wall -o programa main.cpp */*.cpp
./programa            # en Windows: programa.exe
```

Ejecute el programa desde la carpeta `Proyecto/` para que encuentre `data/`.

### Usuarios de prueba

| Rol | Usuario | Contraseña |
|---|---|---|
| Administrador | `admin` | `admin123` |
| Estudiante | `estudiante` | `estu123` |
| Monitor | `monitor` | `mon123` |
| Profesor | `profesor` | `prof123` |

### Ejemplo: materias afectadas al reprobar

```
1 -> admin / admin123 -> 19 -> MAT101 -> 1 (reprobar)

Al reprobar MAT101 quedan bloqueadas 3 materia(s):
  nivel 1 (directas):   FIS201 Fisica Mecanica
  nivel 2 (indirectas): FIS105 Fisica diferencial
  nivel 3 (indirectas): CAL106 Calculo 1
```

## Formato de los archivos de datos

| Archivo | Columnas |
|---|---|
| `usuarios.txt` | `Rol,Nombres,Apellidos,Usuario,Clave,Activo,...` y columnas propias de cada rol |
| `materias.txt` | `Codigo,Nombre,Creditos,Tipo,Prerreq(;),Horario(dia@ini-fin\|...),Area,Nivel` |
| `solicitudes.txt` | `Id,CodEstudiante,CodMateria,Urgencia,Estado,FechaCreacion(epoch),Descripcion` |
| `asignaciones.txt` | `IdSolicitud,CodMonitor,dia@inicio-fin` |
| `estados_materias.txt` | `CodEstudiante,CodMateria,Estado` |

## Limitaciones conocidas

- Las contraseñas se guardan en texto plano.
- `Menus.cpp:589` tiene una versión antigua de `cambiarEstadoUsuario` que no se usa, y es la causa de la única advertencia del compilador.
- `GrafoPrerrequisitos::mostrarCadena` está implementado, pero ningún menú lo llama.
- Los datos de prueba no alcanzan los 500 registros (RNF-01) y no existe la versión en Python (RNF-05), por acuerdo con el equipo docente.

## Documentación

El informe técnico en formato IEEE está en [`docs/informe/informe.pdf`](docs/informe/informe.pdf), con su fuente en `informe.tex`. Incluye la arquitectura, las novedades, el estado de los requerimientos, el análisis de complejidad y el resumen de 68 casos de prueba (59 pasan, 9 pendientes).
