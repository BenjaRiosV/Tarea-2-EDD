#include <iostream>
#include <string>
#include <fstream>
#include <random>

using namespace std;

// Clases
class Jugador {
public:
    int vida;
    int ataque;
    double precision;
    int recuperacion;

    Jugador(int vida_inicial, int ataque_inicial, double precision_inicial, int recuperacion_inicial) {
        vida = vida_inicial;
        ataque = ataque_inicial;
        precision = precision_inicial;
        recuperacion = recuperacion_inicial;
    }

    // Reduce la vida del jugador.
    void recibirDanio(int cantidad) {
        vida -= cantidad;
        if (vida < 0) {
            vida = 0;
        }
    }

    // Aumenta la vida del jugador.
    void curarVida(int cantidad) {
        vida += cantidad;
    }

    // Aumenta el valor de ataque del jugador.
    void aumentarAtaque(int cantidad) {
        ataque += cantidad;
    }

    // Aumenta la precisión del jugador.
    void aumentarPrecision(double cantidad) {
        precision += cantidad;
        if (precision > 1.0) {
            precision = 1.0;
        }
    }

    // Aumenta la capacidad de recuperación del jugador.
    void aumentarRecuperacion(int cantidad) {
        recuperacion += cantidad;
    }

    // Verifica si el jugador aún tiene vida.
    bool estaVivo() {
        return vida > 0;
    }
};

class Enemigo {
public:
    string nombre;
    int vida;
    int ataque;
    double precision;
    double probabilidad;

    Enemigo(string n, int v, int a, double p, double pro) {
        nombre = n;
        vida = v;
        ataque = a;
        precision = p;
        probabilidad = pro;
    }

    // Reduce la vida del enemigo.
    void recibirDanio(int cantidad) {
        vida -= cantidad;
        if (vida < 0) {
            vida = 0;
        }
    }

    // Verifica si el enemigo aún tiene vida.
    bool estaVivo() {
        return vida > 0;
    }
};

class NodoColaEnemigos {
public:
    Enemigo* enemigo;

    NodoColaEnemigos* next;

    // Constructor
    NodoColaEnemigos(Enemigo* e) {
        enemigo = e;
        next = nullptr;
    }

    // Destructor
    ~NodoColaEnemigos() {
        if (enemigo != nullptr) {
            delete enemigo;
            enemigo = nullptr;
        }
    }
};

class ColaEnemigos {
private:
    NodoColaEnemigos* frente;
    NodoColaEnemigos* final;
    int size;

public:
    // Constructor: Inicializa una cola vacía.
    ColaEnemigos() {
        frente = nullptr;
        final = nullptr;
        size = 0;
    }

    // Destructor: Libera toda la memoria ocupada por los nodos de la cola.
    ~ColaEnemigos() {
        while (!isEmpty()) {
            Enemigo* enemigo_desencolado = dequeue();
            if (enemigo_desencolado != nullptr) {
            }
        }
    }

    // Método para agragar un enemigo al final de la cola.
    void enqueue(Enemigo* e) {
        NodoColaEnemigos* nuevoNodo = new NodoColaEnemigos(e);

        if (isEmpty()) {
            frente = nuevoNodo;
            final = nuevoNodo;
        } else {
            final->next = nuevoNodo;
            final = nuevoNodo;
        }
        size++;
    }

    // Método quitar un enemigo de la cola.
    Enemigo* dequeue() {
        if (isEmpty()) {
            return nullptr;
        }

        NodoColaEnemigos* nodo_a_eliminar = frente;
        Enemigo* enemigo_desencolado = nodo_a_eliminar->enemigo;

        frente = frente->next;

        if (frente == nullptr) {
            final = nullptr;
        }

        delete nodo_a_eliminar;

        size--;

        return enemigo_desencolado;
    }

    // Método para ver el enemigo del frente sin eliminarlo.
    Enemigo* peek() {
        if (isEmpty()) {
            return nullptr;
        }
        return frente->enemigo;
    }

    // Método para verificar si la cola está vacía.
    bool isEmpty() {
        return size == 0;
    }

    // Método para obtener el tamaño actual de la cola.
    int size() {
        return size;
    }
};

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

class Evento {
public:
    static const int MAX_OPCIONES = 3;
    string nombre;
    string descripcion;
    double probabilidad_aparicion;

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

class NodoHabitacion {
public:
    static const int MAX_SALIDAS = 3;
    int id;
    string descripcion;
    string tipo;

    NodoHabitacion* siguiente[MAX_SALIDAS];
    int num_siguientes;

    ColaEnemigos* enemigos_combate; // Puntero a una cola de enemigos si es COMBATE
    Evento* evento_habitacion;      // Puntero a un evento si es EVENTO

    // Constructor
    NodoHabitacion(int i, string d, string t) {
        id = i;
        tipo = t;
        descripcion = d;
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

    // Método para agregar una conexión.
    void agregarConexion(NodoHabitacion* destino) {
        if (num_siguientes < MAX_SALIDAS) {
            siguiente[num_siguientes] = destino;
            num_siguientes++;
        }
    }

    // Método para asignar una cola de enemigos a la habitación.
    void asignarEnemigos(ColaEnemigos* cola) {
        if (tipo == "COMBATE") {
            enemigos_combate = cola;
        } else {
            delete cola;
        }
    }

    // Método para asignar un evento a la habitación.
    void asignarEvento(Evento* evento) {
        if (tipo == "EVENTO") {
            evento_habitacion = evento;
        } else {
            delete evento;
        }
    }
};

class Mapa {
private:
    static const int MAX_HABITACIONES = 100;
    // Arreglo de punteros a todas las habitaciones, indexado por su ID.
    NodoHabitacion* raiz;
    NodoHabitacion* todas_las_habitaciones[MAX_HABITACIONES];
    int total_habitaciones_cargadas; // Contador de habitaciones realmente cargadas
    
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

    // Método para cargar la estructura completa del mapa desde un archivo.
    void cargarMapaDesdeArchivo(const string& nombre_archivo) {
        cout << "Iniciando carga del mapa desde: " << nombre_archivo << "..." << endl;
    
        ifstream archivo(nombre_archivo);
        if (!archivo.is_open()) {
            cerr << "ERROR: No se pudo abrir el archivo de mapa: " << nombre_archivo << endl;
            return;
        }
        string linea;
        while (linea != "FIN DEL ARCHIVO") {
            getline(archivo, linea);
            } 
        }
    };