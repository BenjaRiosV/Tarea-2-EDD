// tarea2.cpp

// --- INCLUDES NECESARIOS ---
#include <iostream> // Para entrada/salida (std::cout, std::cin, std::cerr)
#include <string>   // Para el manejo de cadenas de texto (std::string)
#include <fstream>  // Para el manejo de archivos (std::ifstream)
#include <random>   // Para la generación de números aleatorios (mt19937, uniform_real_distribution, random_device)
#include <limits>   // Para std::numeric_limits, útil para limpiar el buffer de entrada de std::cin
#include <cmath>    // Para std::abs, útil para el daño/curación en eventos

// Directiva using para el espacio de nombres estándar
using namespace std;

// --- DEFINICIÓN DE CLASES ---

// 1. Clase Jugador
class Jugador {
public:
    int vida;
    int ataque;
    double precision;
    int recuperacion;

    // Constructor
    Jugador(int vida_inicial, int ataque_inicial, double precision_inicial, int recuperacion_inicial) {
        vida = vida_inicial;
        ataque = ataque_inicial;
        precision = precision_inicial;
        recuperacion = recuperacion_inicial;
    }

    void recibirDanio(int cantidad) {
        vida -= cantidad;
        if (vida < 0) {
            vida = 0;
        }
        cout << "¡Has recibido " << cantidad << " de daño! Vida actual: " << vida << endl;
    }

    void curarVida(int cantidad) {
        vida += cantidad;
        cout << "Has recuperado " << cantidad << " de vida. Vida actual: " << vida << endl;
    }

    void aumentarAtaque(int cantidad) {
        ataque += cantidad;
        cout << "Tu ataque ha aumentado a " << ataque << endl;
    }

    void aumentarPrecision(double cantidad) {
        precision += cantidad;
        if (precision > 1.0) {
            precision = 1.0;
        }
        cout << "Tu precisión ha aumentado a " << precision << endl;
    }

    void aumentarRecuperacion(int cantidad) {
        recuperacion += cantidad;
        cout << "Tu recuperación ha mejorado a " << recuperacion << endl;
    }

    bool estaVivo() {
        return vida > 0;
    }
};

// 2. Clase Enemigo
class Enemigo {
public:
    string nombre;
    int vida;
    int ataque;
    double precision;

    // Constructor
    Enemigo(string n, int v, int a, double p) {
        nombre = n;
        vida = v;
        ataque = a;
        precision = p;
    }

    void recibirDanio(int cantidad) {
        vida -= cantidad;
        if (vida < 0) {
            vida = 0;
        }
    }

    bool estaVivo() {
        return vida > 0;
    }
};

// 3. Clase NodoColaEnemigos
class NodoColaEnemigos {
public:
    Enemigo* enemigo;
    NodoColaEnemigos* siguiente;

    // Constructor
    NodoColaEnemigos(Enemigo* e) {
        enemigo = e;
        siguiente = nullptr;
    }

    // Destructor
    ~NodoColaEnemigos() {
        if (enemigo != nullptr) {
            delete enemigo;
            enemigo = nullptr;
        }
    }
};

// 4. Clase ColaEnemigos
class ColaEnemigos {
private:
    NodoColaEnemigos* frente;
    NodoColaEnemigos* final;
    int tamano;

public:
    // Constructor
    ColaEnemigos() {
        frente = nullptr;
        final = nullptr;
        tamano = 0;
    }

    // Destructor
    ~ColaEnemigos() {
        while (!isEmpty()) {
            // El dequeue elimina el NodoColaEnemigos y su destructor libera el Enemigo*
            // No hacemos nada con el puntero devuelto aquí, solo aseguramos la limpieza.
            dequeue();
        }
    }

    void enqueue(Enemigo* e) {
        NodoColaEnemigos* nuevoNodo = new NodoColaEnemigos(e);

        if (isEmpty()) {
            frente = nuevoNodo;
            final = nuevoNodo;
        } else {
            final->siguiente = nuevoNodo;
            final = nuevoNodo;
        }
        tamano++;
    }

    Enemigo* dequeue() {
        if (isEmpty()) {
            return nullptr;
        }

        NodoColaEnemigos* nodo_a_eliminar = frente;
        Enemigo* enemigo_desencolado = nodo_a_eliminar->enemigo;

        frente = frente->siguiente;

        if (frente == nullptr) {
            final = nullptr;
        }

        // Importante: Desvincular el enemigo del nodo antes de eliminar el nodo
        // para que el destructor de NodoColaEnemigos no intente liberar una memoria
        // que, si fuera a ser usada más tarde, causaría problemas.
        // Pero en nuestro diseño, el destructor de NodoColaEnemigos sí borra el enemigo.
        // Esto significa que 'enemigo_desencolado' apunta a memoria que será liberada.
        // El llamador debe ser consciente de que este puntero NO DEBE ser desreferenciado o borrado.
        nodo_a_eliminar->enemigo = nullptr; // Lo establecemos a nullptr para que el destructor del nodo no borre.
                                           // Esto transfiere la responsabilidad de borrado al puntero devuelto.
                                           // OJO: Si nodo_a_eliminar->enemigo NO se borra en ~NodoColaEnemigos(),
                                           // entonces el 'delete nodo_a_eliminar' no borrará el enemigo.
                                           // Para la tarea, el enemigo se borra cuando el nodo se borra.
                                           // Dejaré el destructor de NodoColaEnemigos para que borre el enemigo.
                                           // Por lo tanto, el puntero 'enemigo_desencolado' DEBE ser nullified aquí
                                           // para que no se borre dos veces si se usa fuera de la cola.
                                           // --> Esta es la parte más confusa de la gestión manual de memoria.
                                           // Para ser coherentes con el diseño de NodoColaEnemigos borrando su Enemigo:
                                           // Debemos permitir que el destructor de NodoColaEnemigos borre el Enemigo.
                                           // Si `dequeue` devuelve el Enemigo*, es una referencia a algo que será borrado.
                                           // La mejor práctica es que `dequeue` *no* borre el Enemigo,
                                           // sino que el código que lo recibe lo borre CUANDO HAYA TERMINADO.

        // **Ajuste para claridad del ownership (revertimos el nullify en dequeue)**
        // Si NodoColaEnemigos es dueño de Enemigo y lo borra, entonces dequeue solo saca el NodoCola.
        // El Enemigo* devuelto es una "mirada" al objeto que se borrará con el nodo.
        // Para evitar confusiones y problemas de dangling pointers en el llamador,
        // una alternativa es que `dequeue` devuelva *copias* de los datos del Enemigo (ej. un struct EnemigoData),
        // o que la gestión de memoria del Enemigo sea explícitamente del llamador de `dequeue`.
        // Mantengamos el diseño original: NodoColaEnemigos borra el Enemigo.
        // Así, el Enemigo* que devuelve dequeue es *inmediatamente inválido* después de la llamada.
        // La lógica del juego debe ser consciente de esto.

        delete nodo_a_eliminar;
        tamano--;
        return enemigo_desencolado; // Puntero a un objeto que ya está siendo liberado.
    }

    Enemigo* peek() {
        if (isEmpty()) {
            return nullptr;
        }
        return frente->enemigo;
    }

    bool isEmpty() {
        return tamano == 0;
    }

    int getTamano() {
        return tamano;
    }
};

// 5. Clase OpcionEvento
class OpcionEvento {
public:
    string enunciado;
    string descripcion;
    int vida_cambio;
    int ataque_cambio;
    double precision_cambio;

    // Constructor
    OpcionEvento(string e, string d, int vc, int ac, double pc) {
        enunciado = e;
        descripcion = d;
        vida_cambio = vc;
        ataque_cambio = ac;
        precision_cambio = pc;
    }
};

// 6. Clase Evento
class Evento {
public:
    static const int MAX_OPCIONES = 3;

    string nombre;
    string descripcion;
    double probabilidad_aparicion; // Probabilidad de que este evento ocurra

    OpcionEvento* opciones[MAX_OPCIONES];
    int num_opciones;

    // Constructor
    Evento(string n, string d, double prob) {
        nombre = n;
        descripcion = d;
        probabilidad_aparicion = prob;
        num_opciones = 0;
        for (int i = 0; i < MAX_OPCIONES; ++i) {
            opciones[i] = nullptr;
        }
    }

    // Destructor
    ~Evento() {
        for (int i = 0; i < num_opciones; ++i) {
            if (opciones[i] != nullptr) {
                delete opciones[i];
                opciones[i] = nullptr;
            }
        }
    }

    void agregarOpcion(OpcionEvento* opcion) {
        if (num_opciones < MAX_OPCIONES) {
            opciones[num_opciones] = opcion;
            num_opciones++;
        }
    }

    OpcionEvento* getOpcion(int indice) {
        if (indice >= 0 && indice < num_opciones) {
            return opciones[indice];
        }
        return nullptr;
    }

    int getNumOpciones() {
        return num_opciones;
    }
};

// 7. Clase NodoHabitacion
class NodoHabitacion {
public:
    static const int MAX_SALIDAS = 3;

    int id;
    string descripcion;
    string tipo;

    NodoHabitacion* siguiente[MAX_SALIDAS];
    int num_siguientes;

    ColaEnemigos* enemigos_combate;
    Evento* evento_habitacion;

    // Constructor
    NodoHabitacion(int i, string d, string t) {
        id = i;
        descripcion = d;
        tipo = t;
        num_siguientes = 0;
        enemigos_combate = nullptr;
        evento_habitacion = nullptr;
        for (int j = 0; j < MAX_SALIDAS; ++j) {
            siguiente[j] = nullptr;
        }
    }

    // Destructor
    ~NodoHabitacion() {
        if (enemigos_combate != nullptr) {
            delete enemigos_combate;
            enemigos_combate = nullptr;
        }
        if (evento_habitacion != nullptr) {
            delete evento_habitacion;
            evento_habitacion = nullptr;
        }
    }

    void agregarConexion(NodoHabitacion* destino) {
        if (num_siguientes < MAX_SALIDAS) {
            siguiente[num_siguientes] = destino;
            num_siguientes++;
        }
    }

    void asignarEnemigos(ColaEnemigos* cola) {
        if (tipo == "COMBATE") {
            enemigos_combate = cola;
        } else {
            delete cola;
        }
    }

    void asignarEvento(Evento* evento) {
        if (tipo == "EVENTO") {
            evento_habitacion = evento;
        } else {
            delete evento;
        }
    }
};

// 8. Clase Mapa
class Mapa {
private:
    static const int MAX_HABITACIONES = 100;

    NodoHabitacion* todas_las_habitaciones[MAX_HABITACIONES];
    int total_habitaciones_cargadas;
    NodoHabitacion* raiz;

public:
    // Constructor
    Mapa() {
        total_habitaciones_cargadas = 0;
        raiz = nullptr;
        for (int i = 0; i < MAX_HABITACIONES; ++i) {
            todas_las_habitaciones[i] = nullptr;
        }
    }

    // Destructor
    ~Mapa() {
        for (int i = 0; i < MAX_HABITACIONES; ++i) {
            if (todas_las_habitaciones[i] != nullptr) {
                delete todas_las_habitaciones[i];
                todas_las_habitaciones[i] = nullptr;
            }
        }
    }

    void cargarMapaDesdeArchivo(const string& nombre_archivo) {
        cout << "Iniciando carga del mapa desde: " << nombre_archivo << "..." << endl;

        // --- INICIO DEL PARSEO REAL DEL MAPA ---
        ifstream archivo(nombre_archivo);
        if (!archivo.is_open()) {
            cerr << "ERROR: No se pudo abrir el archivo de mapa: " << nombre_archivo << endl;
            return;
        }

        string linea;
        int seccion = 0; // 0: NADA, 1: HABITACIONES, 2: CONEXIONES, 3: EVENTOS, 4: ENEMIGOS

        while (getline(archivo, linea)) {
            // Eliminar espacios en blanco al inicio y final de la línea
            size_t first = linea.find_first_not_of(" \t\n\r\f\v");
            if (string::npos == first) continue; // Línea vacía
            size_t last = linea.find_last_not_of(" \t\n\r\f\v");
            linea = linea.substr(first, (last - first + 1));

            if (linea.empty()) continue; // Línea vacía después de limpiar

            if (linea == "HABITACIONES") {
                seccion = 1;
                continue;
            } else if (linea == "CONEXIONES") {
                seccion = 2;
                continue;
            } else if (linea == "EVENTOS") {
                seccion = 3;
                continue;
            } else if (linea == "ENEMIGOS") {
                seccion = 4;
                continue;
            }

            // Procesar líneas según la sección actual
            if (seccion == 1) { // HABITACIONES
                // Formato: id;tipo;descripcion
                size_t pos1 = linea.find(';');
                size_t pos2 = linea.find(';', pos1 + 1);

                if (pos1 != string::npos && pos2 != string::npos) {
                    int id = stoi(linea.substr(0, pos1));
                    string tipo = linea.substr(pos1 + 1, pos2 - (pos1 + 1));
                    string descripcion = linea.substr(pos2 + 1);
                    
                    NodoHabitacion* nueva_habitacion = new NodoHabitacion(id, tipo, descripcion); // Ajuste: tipo primero, luego desc
                    agregarHabitacion(nueva_habitacion); // Agrega y maneja la raíz
                }
            } else if (seccion == 2) { // CONEXIONES
                // Formato: id_origen;id_destino1;id_destino2;id_destino3
                size_t current_pos = 0;
                size_t next_pos = linea.find(';', current_pos);
                int id_origen = stoi(linea.substr(current_pos, next_pos - current_pos));
                NodoHabitacion* origen = getHabitacion(id_origen);

                if (origen == nullptr) {
                    cerr << "ADVERTENCIA: Habitacion de origen " << id_origen << " no encontrada para conexion." << endl;
                    continue;
                }

                current_pos = next_pos + 1;
                for (int i = 0; i < NodoHabitacion::MAX_SALIDAS; ++i) {
                    next_pos = linea.find(';', current_pos);
                    string id_str = linea.substr(current_pos, next_pos - current_pos);
                    
                    if (id_str == "-1") { // No hay más conexiones
                        break;
                    }
                    
                    int id_destino = stoi(id_str);
                    NodoHabitacion* destino = getHabitacion(id_destino);
                    
                    if (destino != nullptr) {
                        origen->agregarConexion(destino);
                    } else {
                        cerr << "ADVERTENCIA: Habitacion de destino " << id_destino << " no encontrada para conexion desde " << id_origen << "." << endl;
                    }
                    current_pos = next_pos + 1;
                    if (next_pos == string::npos) break; // No more semicolons
                }

            } else if (seccion == 3) { // EVENTOS
                // Formato: id_habitacion;nombre_evento;probabilidad;descripcion_evento;num_opciones;
                // enunciado1;descripcion1;vidac1;ataquec1;prec1;
                // enunciado2;descripcion2;vidac2;ataquec2;prec2;
                // ...
                size_t p = linea.find(';');
                int id_habitacion = stoi(linea.substr(0, p));
                string resto = linea.substr(p + 1);

                NodoHabitacion* habitacion = getHabitacion(id_habitacion);
                if (habitacion == nullptr || habitacion->tipo != "EVENTO") {
                    cerr << "ADVERTENCIA: Habitacion " << id_habitacion << " no encontrada o no es de tipo EVENTO para asignar evento." << endl;
                    continue;
                }
                
                // Parsear el evento (nombre, prob, desc, num_opciones)
                p = resto.find(';'); string nombre_evento = resto.substr(0, p); resto = resto.substr(p + 1);
                p = resto.find(';'); double probabilidad = stod(resto.substr(0, p)); resto = resto.substr(p + 1);
                p = resto.find(';'); string descripcion_evento = resto.substr(0, p); resto = resto.substr(p + 1);
                p = resto.find(';'); int num_opciones_evento = stoi(resto.substr(0, p)); resto = resto.substr(p + 1);

                Evento* nuevo_evento = new Evento(nombre_evento, descripcion_evento, probabilidad);
                
                // Parsear las opciones
                size_t current_pos = 0;
                for(int i = 0; i < num_opciones_evento; ++i) {
                    // enunciado;descripcion;vidac;ataquec;prec;
                    size_t p1 = resto.find(';', current_pos); string en = resto.substr(current_pos, p1 - current_pos); current_pos = p1 + 1;
                    size_t p2 = resto.find(';', current_pos); string de = resto.substr(current_pos, p2 - current_pos); current_pos = p2 + 1;
                    size_t p3 = resto.find(';', current_pos); int vc = stoi(resto.substr(current_pos, p3 - current_pos)); current_pos = p3 + 1;
                    size_t p4 = resto.find(';', current_pos); int ac = stoi(resto.substr(current_pos, p4 - current_pos)); current_pos = p4 + 1;
                    size_t p5 = resto.find(';', current_pos); double pc = stod(resto.substr(current_pos, p5 - current_pos)); current_pos = p5 + 1;

                    OpcionEvento* nueva_opcion = new OpcionEvento(en, de, vc, ac, pc);
                    nuevo_evento->agregarOpcion(nueva_opcion);
                }
                habitacion->asignarEvento(nuevo_evento);

            } else if (seccion == 4) { // ENEMIGOS
                // Formato: id_habitacion;nombre_e1;vida_e1;ataque_e1;prec_e1;nombre_e2;...
                size_t p = linea.find(';');
                int id_habitacion = stoi(linea.substr(0, p));
                string resto = linea.substr(p + 1);

                NodoHabitacion* habitacion = getHabitacion(id_habitacion);
                if (habitacion == nullptr || habitacion->tipo != "COMBATE") {
                    cerr << "ADVERTENCIA: Habitacion " << id_habitacion << " no encontrada o no es de tipo COMBATE para asignar enemigos." << endl;
                    continue;
                }

                ColaEnemigos* cola_enemigos_sala = new ColaEnemigos();
                size_t current_pos = 0;
                while (current_pos < resto.length() && resto.find(';', current_pos) != string::npos) {
                    size_t p1 = resto.find(';', current_pos); string n = resto.substr(current_pos, p1 - current_pos); current_pos = p1 + 1;
                    size_t p2 = resto.find(';', current_pos); int v = stoi(resto.substr(current_pos, p2 - current_pos)); current_pos = p2 + 1;
                    size_t p3 = resto.find(';', current_pos); int a = stoi(resto.substr(current_pos, p3 - current_pos)); current_pos = p3 + 1;
                    size_t p4 = resto.find(';', current_pos); double pr = stod(resto.substr(current_pos, p4 - current_pos)); current_pos = p4 + 1;

                    Enemigo* nuevo_enemigo = new Enemigo(n, v, a, pr);
                    cola_enemigos_sala->enqueue(nuevo_enemigo);
                }
                habitacion->asignarEnemigos(cola_enemigos_sala);
            }
        }
        archivo.close();
        cout << "Carga del mapa completada. Total habitaciones cargadas: " << total_habitaciones_cargadas << endl;
        // --- FIN DEL PARSEO REAL DEL MAPA ---
    }


    // Método para obtener un puntero a una habitación por su ID.
    NodoHabitacion* getHabitacion(int id) {
        if (id >= 0 && id < MAX_HABITACIONES && todas_las_habitaciones[id] != nullptr) {
            return todas_las_habitaciones[id];
        }
        return nullptr;
    }

    // Método para obtener el puntero a la habitación raíz (inicial).
    NodoHabitacion* getRaiz() {
        return raiz;
    }

    void agregarHabitacion(NodoHabitacion* nueva_habitacion) {
        if (nueva_habitacion != nullptr && nueva_habitacion->id >= 0 && nueva_habitacion->id < MAX_HABITACIONES) {
            if (todas_las_habitaciones[nueva_habitacion->id] == nullptr) {
                todas_las_habitaciones[nueva_habitacion->id] = nueva_habitacion;
                total_habitaciones_cargadas++;
                if (nueva_habitacion->id == 0) {
                    raiz = nueva_habitacion;
                }
            } else {
                cerr << "ADVERTENCIA: Se intento agregar una habitacion con ID " << nueva_habitacion->id << " que ya existe." << endl;
                delete nueva_habitacion;
            }
        } else {
            cerr << "ADVERTENCIA: Intentando agregar una habitacion invalida o con ID fuera de rango." << endl;
            delete nueva_habitacion;
        }
    }
};

// 9. Clase Juego
class Juego {
private:
    Jugador* maya;
    Mapa* mapa_aventura;
    NodoHabitacion* habitacion_actual;

    mt19937 rng; // Motor de aleatoriedad
    uniform_real_distribution<double> dist_probabilidad; // Distribución para 0.0 a 1.0

public:
    // Constructor
    Juego() {
        maya = new Jugador(30, 5, 0.7, 2); // Atributos iniciales base
        mapa_aventura = new Mapa();
        habitacion_actual = nullptr;

        random_device rd; // Obtener una semilla aleatoria del sistema operativo
        rng.seed(rd()); // Inicializar el generador de números aleatorios
        dist_probabilidad = uniform_real_distribution<double>(0.0, 1.0); // Distribución para probabilidades
    }

    // Destructor
    ~Juego() {
        delete maya;
        delete mapa_aventura;
    }

    void iniciarJuego() {
        cout << "-----------------------------------------------------" << endl;
        cout << "         The Last of Us - El Ultimo Aliento          " << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "La penumbra se cierne sobre tu pequeno apartamento dentro de la zona de cuarentena de FEDRA. "
             << "El aire es pesado con el silencio roto solo por la respiracion irregular de tu madre en la habitacion contigua. "
             << "Atada a la cama, su cuerpo convulsiona ocasionalmente, las grotescas formaciones fungicas comenzando a brotar de su piel. "
             << "El miedo y la desesperacion te corroen. Tienes tu cuchillo en la mano, la unica forma de acabar con su sufrimiento... "
             << "o la unica forma de protegerla mientras buscas una imposible esperanza." << endl << endl;
        manejarInicio();
    }

    void manejarInicio() {
        int opcion;
        cout << "Que decides hacer?" << endl;
        cout << "1. Buscar una cura." << endl;
        cout << "2. Liberar a tu madre (matarla)." << endl;
        cout << "Elige una opcion: ";

        while (!(cin >> opcion) || (opcion != 1 && opcion != 2)) {
            cout << "Opcion invalida. Por favor, elige 1 o 2: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (opcion == 1) {
            cout << "\nNo, no la abandonare. Hay una posibilidad. Lo hare por ti, mama. Tomas tu cuchillo, el corazon latiendote a mil. "
                      << "Sales sigilosamente de la habitacion, decidido a encontrar una cura a toda costa. "
                      << "Justo al abrir la puerta del apartamento, te encuentras de frente con un militar de FEDRA, patrullando el pasillo. "
                      << "Te ha visto. Sin dudarlo, te abalanzas sobre el con tu cuchillo antes de que pueda reaccionar, "
                      << "lo apuñalas y lo matas. Robas su fusil y sus provisiones. Con el arma en mano, te sientes un poco mas segura, "
                      << "pero tambien mas lejos de la ley y mas sola que nunca. El camino es ahora irreversible." << endl;
            
            maya->ataque = 10;
            maya->precision = 0.8;
            maya->recuperacion = 3;
            
            mapa_aventura->cargarMapaDesdeArchivo("data.map");
            habitacion_actual = mapa_aventura->getRaiz();

            jugar();
        } else {
            cout << "\nLo siento, mama. No puedo dejarte asi. Mereces paz. Te acercas a la cama, el cuchillo temblandole en la mano. "
                      << "Cuando te inclinas para acabar con su miseria, tu madre, con una fuerza repentina y antinatural, se abalanza sobre ti, "
                      << "el hongo en su boca extendiendose. Sientes un mordisco agonizante antes de que tu vista se nuble. "
                      << "En ese momento, la puerta del apartamento se abre de golpe. Un militar de FEDRA entra, lo ves todo borroso, "
                      << "su rostro de horror, y luego la oscuridad. El te ha visto infectada y no duda un segundo." << endl;
            mostrarFinal("FINAL 1: Matas a tu madre y mueres.");
        }
    }

    void jugar() {
        while (maya->estaVivo() && habitacion_actual != nullptr && habitacion_actual->tipo != "FIN") {
            mostrarEstadoJugador();
            cout << "\n-----------------------------------------------------" << endl;
            cout << "Estas en: " << habitacion_actual->descripcion << endl;
            cout << "Tipo de habitacion: " << habitacion_actual->tipo << endl;
            cout << "-----------------------------------------------------" << endl;

            if (habitacion_actual->tipo == "COMBATE") {
                if (habitacion_actual->enemigos_combate != nullptr && !habitacion_actual->enemigos_combate->isEmpty()) {
                     manejarCombate(habitacion_actual->enemigos_combate);
                } else {
                    cout << "La habitacion de combate esta extranamente vacia de enemigos. Avanzas." << endl;
                }
            } else if (habitacion_actual->tipo == "EVENTO") {
                if (habitacion_actual->evento_habitacion != nullptr) {
                    manejarEvento(habitacion_actual->evento_habitacion);
                } else {
                    cout << "Parece que no hay evento en esta habitacion. Continuas." << endl;
                }
            }
            // "INICIO" se maneja en manejarInicio y luego se avanza.
            // "NORMAL" no requiere accion especifica, solo navegacion.

            if (!maya->estaVivo()) {
                mostrarFinal("HAS MUERTO. La busqueda de la cura termino en tragedia.");
                break; // Sale del bucle de juego si Maya muere
            }

            // Si es una habitacion FIN, el bucle se detendrá en la siguiente iteración.
            if (habitacion_actual->tipo == "FIN") {
                mostrarFinal(habitacion_actual->descripcion);
                break;
            }

            // Opciones de navegacion
            if (habitacion_actual->num_siguientes > 0) {
                cout << "\nCaminos disponibles:" << endl;
                for (int i = 0; i < habitacion_actual->num_siguientes; ++i) {
                    cout << (i + 1) << ". Ir a: " << habitacion_actual->siguiente[i]->descripcion << endl;
                }
                cout << "Elige tu siguiente camino: ";
                int eleccion;
                while (!(cin >> eleccion) || eleccion < 1 || eleccion > habitacion_actual->num_siguientes) {
                    cout << "Opcion invalida. Elige un numero entre 1 y " << habitacion_actual->num_siguientes << ": ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                habitacion_actual = habitacion_actual->siguiente[eleccion - 1];
                cout << "Viajando a " << habitacion_actual->descripcion << "..." << endl;
            } else if (habitacion_actual->tipo != "FIN") {
                cout << "No hay mas caminos disponibles desde esta habitacion. Tu aventura termina aqui." << endl;
                mostrarFinal("FINAL INESPERADO: Sin caminos disponibles.");
                habitacion_actual = nullptr; // Fuerza la salida del bucle
            }
        }

        if (habitacion_actual != nullptr && habitacion_actual->tipo == "FIN") {
            mostrarFinal(habitacion_actual->descripcion); // Si el bucle termino por FIN
        } else if (!maya->estaVivo()) {
            mostrarFinal("HAS MUERTO. La busqueda de la cura termino en tragedia.");
        }
    }

    void mostrarEstadoJugador() {
        cout << "\n--- Estado de Maya ---" << endl;
        cout << "Vida: " << maya->vida << endl;
        cout << "Ataque: " << maya->ataque << endl;
        cout << "Precision: " << maya->precision << endl;
        cout << "Recuperacion: " << maya->recuperacion << endl;
        cout << "----------------------" << endl;
    }

    void manejarCombate(ColaEnemigos* enemigos_en_sala) {
        cout << "\n--- ¡Combate! ---" << endl;
        while (maya->estaVivo() && !enemigos_en_sala->isEmpty()) {
            Enemigo* enemigo_actual = enemigos_en_sala->peek();
            cout << "\n--- Enemigo actual: " << enemigo_actual->nombre
                 << " (Vida: " << enemigo_actual->vida << ", Ataque: " << enemigo_actual->ataque << ") ---" << endl;

            // Turno de Maya
            cout << "Tu turno para atacar a " << enemigo_actual->nombre << "." << endl;
            if (generarProbabilidad() < maya->precision) {
                cout << "¡Acertaste! " << maya->ataque << " de dano a " << enemigo_actual->nombre << "." << endl;
                enemigo_actual->recibirDanio(maya->ataque);
            } else {
                cout << "¡Fallaste tu ataque!" << endl;
            }

            if (!enemigo_actual->estaVivo()) {
                cout << "Has derrotado a " << enemigo_actual->nombre << "!" << endl;
                enemigos_en_sala->dequeue(); // Eliminar enemigo derrotado.
                                             // El destructor de NodoColaEnemigos se encarga de borrar el Enemigo*.
                if (!enemigos_en_sala->isEmpty()) {
                    cout << "Preparate, otro enemigo se acerca!" << endl;
                }
                continue; // Pasa al siguiente enemigo o fin de combate si no hay más
            }

            // Turno del Enemigo (solo si sigue vivo después del ataque de Maya)
            cout << enemigo_actual->nombre << "'s turno para atacar." << endl;
            if (generarProbabilidad() < enemigo_actual->precision) {
                cout << enemigo_actual->nombre << " te ataca! ";
                maya->recibirDanio(enemigo_actual->ataque);
            } else {
                cout << enemigo_actual->nombre << " ha fallado su ataque!" << endl;
            }

            if (!maya->estaVivo()) {
                cout << "Has sido derrotado en combate." << endl;
                break; // Maya ha muerto, salir del combate
            }
        }

        if (maya->estaVivo() && enemigos_en_sala->isEmpty()) {
            cout << "\n--- Has ganado el combate! ---" << endl;
            maya->curarVida(maya->recuperacion); // Recupera vida después de ganar
        } else if (!maya->estaVivo()) {
            cout << "\n--- Has perdido el combate! ---" << endl;
            // El bucle 'jugar' se encargará de mostrar el final del juego
        }
    }

    void manejarEvento(Evento* evento_actual) {
        cout << "\n--- ¡Evento! ---" << endl;
        cout << evento_actual->descripcion << endl;
        cout << "Opciones disponibles:" << endl;

        for (int i = 0; i < evento_actual->getNumOpciones(); ++i) {
            OpcionEvento* opcion = evento_actual->getOpcion(i);
            if (opcion != nullptr) {
                cout << (i + 1) << ". " << opcion->enunciado << endl;
            }
        }

        int eleccion;
        cout << "Elige una opcion: ";
        while (!(cin >> eleccion) || eleccion < 1 || eleccion > evento_actual->getNumOpciones()) {
            cout << "Opcion invalida. Elige un numero entre 1 y " << evento_actual->getNumOpciones() << ": ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        OpcionEvento* opcion_elegida = evento_actual->getOpcion(eleccion - 1);
        if (opcion_elegida != nullptr) {
            cout << "\n" << opcion_elegida->descripcion << endl;

            if (opcion_elegida->vida_cambio > 0) {
                maya->curarVida(opcion_elegida->vida_cambio);
            } else if (opcion_elegida->vida_cambio < 0) {
                maya->recibirDanio(abs(opcion_elegida->vida_cambio));
            }

            if (opcion_elegida->ataque_cambio != 0) {
                maya->aumentarAtaque(opcion_elegida->ataque_cambio);
            }
            if (opcion_elegida->precision_cambio != 0.0) {
                maya->aumentarPrecision(opcion_elegida->precision_cambio);
            }
        } else {
            cout << "ERROR: Opcion de evento invalida." << endl;
        }
    }

    void mostrarFinal(string mensaje_final) {
        cout << "\n=====================================================" << endl;
        cout << "                   FIN DEL JUEGO                   " << endl;
        cout << mensaje_final << endl;
        cout << "=====================================================" << endl;
    }

    // Funcion auxiliar para generar un numero aleatorio entre 0.0 y 1.0
    double generarProbabilidad() {
        return dist_probabilidad(rng);
    }
};

// --- FUNCIÓN PRINCIPAL DEL PROGRAMA ---
int main() {
    Juego miJuego;
    miJuego.iniciarJuego();
    return 0;
}