// Main de la Tarea 2 EDD

#include <iostream>
#include <string>
#include <fstream> // Para manejo de archivos
#include <random>  // Para números aleatorios (permitido por la tarea)
#include <limits>  // Para numeric_limits (permitido por la tarea)
#include <cctype>  // Para isdigit (permitido por la tarea)

using namespace std;

// Generadores de números aleatorios globales (permitido por la tarea)
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0.0, 1.0); // Distribución uniforme entre 0.0 y 1.0 para precisión y probabilidad

// --- Clases de Juego ---

class Jugador {
public:
    int vida;
    int ataque;
    double precision;
    int recuperacion; // Capacidad de recuperación de vida post-combate/evento

    // Constructor usando asignación en el cuerpo (según tu preferencia)
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
        // Podrías añadir un límite máximo de vida si lo deseas, por ahora no está en el PDF
    }

    // Aumenta el valor de ataque del jugador.
    void aumentarAtaque(int cantidad) {
        ataque += cantidad;
    }

    // Aumenta la precisión del jugador.
    void aumentarPrecision(double cantidad) {
        precision += cantidad;
        if (precision > 1.0) { // La precisión no puede exceder 1.0 (100%)
            precision = 1.0;
        }
    }

    // Aumenta la capacidad de recuperación del jugador.
    void aumentarRecuperacion(int cantidad) {
        recuperacion += cantidad;
        if (recuperacion < 0) { // La recuperación no debería ser negativa
            recuperacion = 0;
        }
    }

    // Verifica si el jugador aún tiene vida.
    bool estaVivo() {
        return vida > 0;
    }

    // Realiza un ataque a un enemigo.
    // Retorna true si el ataque es exitoso (impacta), false si falla.
    bool atacar(class Enemigo* enemigo); // Declaración forward para evitar dependencia circular inmediata
};

class Enemigo {
public:
    string nombre;
    int vida;
    int ataque;
    double precision;

    // Constructor usando asignación en el cuerpo
    Enemigo(string n, int v, int a, double p) {
        nombre = n;
        vida = v;
        ataque = a;
        precision = p;
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

    // Realiza un ataque a un jugador.
    // Retorna true si el ataque es exitoso (impacta), false si falla.
    bool atacar(Jugador* jugador); // Declaración forward
};

// Implementaciones de los métodos atacar fuera de la clase para evitar forward declaration issues si se definen dentro
bool Jugador::atacar(Enemigo* enemigo) {
    double probabilidad_generada = dis(gen); // Genera un número aleatorio entre 0.0 y 1.0
    if (probabilidad_generada <= precision) { // Si el número aleatorio es menor o igual a la precisión del jugador
        enemigo->recibirDanio(ataque); // El ataque impacta y el enemigo recibe daño
        return true; // Ataque exitoso
    }
    return false; // Ataque fallido
}

bool Enemigo::atacar(Jugador* jugador) {
    double probabilidad_generada = dis(gen); // Genera un número aleatorio entre 0.0 y 1.0
    if (probabilidad_generada <= precision) { // Si el número aleatorio es menor o igual a la precisión del enemigo
        jugador->recibirDanio(ataque); // El ataque impacta y el jugador recibe daño
        return true; // Ataque exitoso
    }
    return false; // Ataque fallido
}

// --- Cola de Enemigos (para Combate) ---

class NodoColaEnemigos {
public:
    Enemigo* enemigo;
    NodoColaEnemigos* siguiente;

    // Constructor
    NodoColaEnemigos(Enemigo* e) {
        enemigo = e;
        siguiente = nullptr;
    }

    // Destructor: Importante para liberar la memoria del enemigo que contiene
    ~NodoColaEnemigos() {
        if (enemigo != nullptr) {
            delete enemigo; // Libera el objeto Enemigo
            enemigo = nullptr;
        }
    }
};

class ColaEnemigos {
private:
    NodoColaEnemigos* frente;
    NodoColaEnemigos* final;
    int tamano;

public:
    // Constructor: Inicializa una cola vacía.
    ColaEnemigos() {
        frente = nullptr;
        final = nullptr;
        tamano = 0;
    }

    // Destructor: Libera toda la memoria ocupada por los nodos de la cola y los enemigos.
    ~ColaEnemigos() {
            while (!isEmpty()) {
                // dequeue ya se encarga de liberar el nodo y el enemigo
                Enemigo* enemigo_desencolado = dequeue();
                if (enemigo_desencolado != nullptr) {
                    delete enemigo_desencolado; // ¡Liberar la memoria del Enemigo* devuelto!
                    enemigo_desencolado = nullptr;
            }
        }
    }

    // Método para añadir un enemigo al final de la cola.
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

    // Método para sacar un enemigo del frente de la cola y devolverlo.
    // Retorna nullptr si la cola está vacía.
    Enemigo* dequeue() {
        if (isEmpty()) {
            return nullptr;
        }

        NodoColaEnemigos* nodo_a_eliminar = frente;
        Enemigo* enemigo_desencolado = nodo_a_eliminar->enemigo; // Obtener el puntero al enemigo

        frente = frente->siguiente; // Mover el frente al siguiente nodo

        if (frente == nullptr) { // Si la cola queda vacía, el final también debe ser nullptr
            final = nullptr;
        }

        nodo_a_eliminar->enemigo = nullptr; // Desvincular el enemigo del nodo antes de eliminar el nodo
                                            // para que el destructor del nodo no intente eliminarlo de nuevo
                                            // ya que lo estamos devolviendo.
        delete nodo_a_eliminar; // Liberar el nodo de la cola

        tamano--;

        return enemigo_desencolado; // Retornar el enemigo para que el juego lo use
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
        return tamano == 0;
    }

    // Método para obtener el tamaño actual de la cola.
    int getTamano() {
        return tamano;
    }
};

// --- Structs para Carga de Datos ---

// Propiedades base de un enemigo (para cargar del .map)
struct PropEnemigo {
    string nombre;
    int vida;
    int ataque;
    double precision;
    double probabilidad_aparicion; // Probabilidad de que aparezca este tipo de enemigo
};

// Mejoras que el jugador puede obtener (cargadas del .map)
struct Mejoras {
    int vida;
    int ataque;
    double precision;
    int recuperacion; // Cantidad de vida de auto-recuperación
};

// Representa una opción dentro de un evento
struct OpcionEvento {
    char id_opcion; // 'A', 'B', etc.
    string enunciado;
    string descripcion;
    // Consecuencia del evento
    int consec_danio;
    int consec_cura;
    int consec_mejora_vida;
    int consec_mejora_ataque;
    double consec_mejora_precision;
    int consec_mejora_recuperacion;
    bool consec_volver_anterior; // Para el requisito especial 3 (Playa)

    // Constructor que inicializa todo a 0/null
    OpcionEvento() {
        id_opcion = ' ';
        enunciado = "";
        descripcion = "";
        consec_danio = 0;
        consec_cura = 0;
        consec_mejora_vida = 0;
        consec_mejora_ataque = 0;
        consec_mejora_precision = 0.0;
        consec_mejora_recuperacion = 0;
        consec_volver_anterior = false;
    }
};

// Representa un evento principal (cargado del .map)
struct EventoPrincipal {
    string nombre;
    double probabilidad;
    string descripcion_general;
    OpcionEvento opcion_A;
    OpcionEvento opcion_B;
    // Podrías añadir más opciones si el formato del .map lo permitiera, pero el PDF muestra solo A y B

    // Constructor
    EventoPrincipal() {
        nombre = "";
        probabilidad = 0.0;
        descripcion_general = "";
    }
};


// --- Tipos de Habitaciones y Nodos del Árbol ---

// Enum para los tipos de habitaciones
enum TipoHabitacion {
    HabErroneo, // Para errores de parsing
    HabInicio,
    HabEvento,
    HabCombate,
    HabFin
};

// Función auxiliar para convertir string a TipoHabitacion
TipoHabitacion stringToTipoHabitacion(const string& s) {
    if (s == "INICIO") return HabInicio;
    if (s == "EVENTO") return HabEvento;
    if (s == "COMBATE") return HabCombate;
    if (s == "FIN") return HabFin;
    return HabErroneo;
}

// Clase Habitacion: Representa la información de una ubicación en el juego
class Habitacion {
public:
    int id;
    string nombre;
    TipoHabitacion tipo_habitacion;
    string descripcion_habitacion;

    // Constructor usando asignación en el cuerpo
    Habitacion(int i, string n, TipoHabitacion th, string dh) {
        id = i;
        nombre = n;
        tipo_habitacion = th;
        descripcion_habitacion = dh;
    }
    // No necesita destructor si no maneja memoria dinámica propia (punteros a otros objetos)
};

// Clase NodoArbol: Representa un nodo en el árbol ternario
class NodoArbol {
public:
    Habitacion* habitacion; // Puntero a la habitación que contiene este nodo
    NodoArbol* hijo_izq;
    NodoArbol* hijo_cen;
    NodoArbol* hijo_der;

    // Constructor
    NodoArbol(Habitacion* h) {
        habitacion = h;
        hijo_izq = nullptr;
        hijo_cen = nullptr;
        hijo_der = nullptr;
    }

    // Destructor: Libera la memoria de la habitación que contiene
    ~NodoArbol() {
        if (habitacion != nullptr) {
            delete habitacion; // Libera el objeto Habitacion
            habitacion = nullptr;
        }
        // No borra los hijos, ya que el destructor del árbol se encarga de recorrer y eliminar
        // para evitar eliminaciones dobles y asegurar que todos los nodos se liberen una vez.
    }
};


// --- Arbol Ternario (Estructura Principal del Mapa) ---

class ArbolTernario {
public:
    NodoArbol* raiz;
    int max_id_habitacion; // Para dimensionar los arreglos de punteros
    
    // Arreglos para acceso rápido por ID (permiten construir el árbol sin buscar nodos)
    Habitacion** habitaciones_cargadas_por_id; // Puntero a un arreglo de punteros a Habitacion
    NodoArbol** nodos_del_arbol_por_id;       // Puntero a un arreglo de punteros a NodoArbol

    // Datos cargados del mapa
    PropEnemigo* enemigos_cargados;
    int num_enemigos_cargados;
    EventoPrincipal* eventos_principales_cargados;
    int num_eventos_principales_cargados;
    Mejoras mejoras_combate_cargadas;

    // Constructor
    ArbolTernario() {
        raiz = nullptr;
        max_id_habitacion = -1; // Se determinará al cargar las habitaciones
        habitaciones_cargadas_por_id = nullptr;
        nodos_del_arbol_por_id = nullptr;
        enemigos_cargados = nullptr;
        num_enemigos_cargados = 0;
        eventos_principales_cargados = nullptr;
        num_eventos_principales_cargados = 0;
        // mejoras_combate_cargadas ya se inicializa con su constructor por defecto si es necesario
    }

    // Destructor: Libera toda la memoria dinámica asignada por el árbol
    ~ArbolTernario() {
        // Liberar enemigos cargados
        if (enemigos_cargados != nullptr) {
            delete[] enemigos_cargados;
            enemigos_cargados = nullptr;
        }
        // Liberar eventos principales cargados
        if (eventos_principales_cargados != nullptr) {
            delete[] eventos_principales_cargados;
            eventos_principales_cargados = nullptr;
        }

        // Liberar los nodos del árbol (y sus habitaciones contenidas) de forma segura
        // Se usa un enfoque iterativo para evitar desbordamiento de pila con árboles grandes
        // (Aunque para un árbol ternario pequeño, un recorrido recursivo también podría funcionar)
        if (raiz != nullptr) {
            // Usamos una "pila" manual para el recorrido post-orden (o similar) para la eliminación.
            // Para simplificar, asumiremos que nodes_del_arbol_por_id contiene todos los nodos
            // y los borraremos directamente desde allí después de desconectarlos lógicamente
            // del árbol si fuera necesario. Sin embargo, un recorrido para asegurar la eliminación
            // es más robusto. Usaremos un vector temporal como "pila" para la limpieza (NO STL, se emulará con un arreglo dinámico).
            // O más sencillo, si `nodos_del_arbol_por_id` contiene todos los punteros válidos,
            // podemos simplemente iterar y borrar.

            // La forma más segura y sin STL para liberar un árbol no binario si no tienes un recorrido explícito de eliminación es:
            // 1. Desconectar los hijos de cada padre para evitar que los destructores intenten borrar lo mismo.
            // 2. Luego, borrar cada nodo_del_arbol_por_id.
            // Para este ejercicio, como los nodos se referencian por ID y son parte del arreglo,
            // simplemente liberaremos el arreglo de NodoArbol. Los destructores de NodoArbol liberarán las Habitaciones.

            // Liberar los nodos y las habitaciones que contienen (ya que el destructor de NodoArbol los libera)
            if (nodos_del_arbol_por_id != nullptr) {
                for (int i = 0; i <= max_id_habitacion; ++i) {
                    if (nodos_del_arbol_por_id[i] != nullptr) {
                        // El destructor de NodoArbol se encargará de 'delete habitacion;'
                        delete nodos_del_arbol_por_id[i];
                        nodos_del_arbol_por_id[i] = nullptr;
                    }
                }
                delete[] nodos_del_arbol_por_id;
                nodos_del_arbol_por_id = nullptr;
            }
        }
        // El arreglo habitaciones_cargadas_por_id NO necesita ser liberado con delete[]
        // porque las Habitaciones son eliminadas por los destructores de NodoArbol.
        // Solo necesitamos liberar el arreglo de punteros, no los objetos a los que apuntan.
        if (habitaciones_cargadas_por_id != nullptr) {
            delete[] habitaciones_cargadas_por_id;
            habitaciones_cargadas_por_id = nullptr;
        }
    }

    // --- Métodos para cargar el mapa desde archivo ---

    // Función auxiliar para parsear consecuencias (modificada para no usar stringstream)
    void LeerConsecuencia(const string& s, OpcionEvento& opcion) {
        opcion.consec_danio = 0;
        opcion.consec_cura = 0;
        opcion.consec_mejora_vida = 0;
        opcion.consec_mejora_ataque = 0;
        opcion.consec_mejora_precision = 0.0;
        opcion.consec_mejora_recuperacion = 0;
        opcion.consec_volver_anterior = false;

        if (s.find("Ninguna") != string::npos) {
            return; // No hay consecuencias
        }

        // Buscar el valor numérico
        size_t pos_valor_inicio = 0;
        while (pos_valor_inicio < s.length() && (s[pos_valor_inicio] == '+' || s[pos_valor_inicio] == '-' || isdigit(s[pos_valor_inicio]) || s[pos_valor_inicio] == '.')) {
            pos_valor_inicio++; // Encontrar el final del valor numérico
        }
        string s_val = s.substr(0, pos_valor_inicio); // Extraer el string del valor

        // Determinar el tipo de consecuencia y asignar el valor
        if (s.find("PRECISION") != string::npos) {
            opcion.consec_mejora_precision = stod(s_val);
        } else if (s.find("Vida") != string::npos) {
            int valor_vida = stoi(s_val);
            if (valor_vida < 0) {
                opcion.consec_danio = -valor_vida; // Daño es positivo
            } else {
                opcion.consec_cura = valor_vida; // Cura es positiva
            }
        } else if (s.find("Ataque") != string::npos) {
            opcion.consec_mejora_ataque = stoi(s_val);
        } else if (s.find("Recuperacion") != string::npos) {
            opcion.consec_mejora_recuperacion = stoi(s_val);
        } else if (s.find("Vuelves al nodo ante-anterior!") != string::npos) {
            opcion.consec_volver_anterior = true;
        }
    }

    // Carga todo el mapa desde el archivo
    bool cargarMapa(const string& nombre_archivo) {
        ifstream archivo(nombre_archivo);
        if (!archivo.is_open()) {
            cout << "Error: No se pudo abrir el archivo " << nombre_archivo << endl;
            return false;
        }

        string line;
        int current_section = 0; // 0: None, 1: HABITACIONES, 2: ARCOS, 3: ENEMIGOS, 4: EVENTOS, 5: MEJORAS_DE_COMBATE

        // --- Fase 1: Determinar el max_id_habitacion para dimensionar arreglos ---
        // Se hace un primer paso por el archivo para encontrar el ID más grande.
        // Esto es necesario porque no sabemos el tamaño del arreglo de antemano sin STL.
        // Luego el archivo se vuelve a abrir (o se reinicia el puntero de lectura).

        string temp_line;
        int temp_max_id = -1;
        bool in_habitaciones_section_temp = false;
        while (getline(archivo, temp_line)) {
            if (temp_line.find("HABITACIONES") != string::npos) {
                in_habitaciones_section_temp = true;
                getline(archivo, temp_line); // Lee el número de habitaciones
                continue;
            }
            if (in_habitaciones_section_temp && !temp_line.empty() && isdigit(temp_line[0])) {
                size_t pos_espacio = temp_line.find(' ');
                if (pos_espacio != string::npos) {
                    int id = stoi(temp_line.substr(0, pos_espacio));
                    if (id > temp_max_id) {
                        temp_max_id = id;
                    }
                }
            }
            if (temp_line.find("ARCOS") != string::npos) {
                in_habitaciones_section_temp = false; // Ya no estamos en la sección de habitaciones
            }
        }
        max_id_habitacion = temp_max_id;
        if (max_id_habitacion == -1) {
            cout << "Error: No se encontraron habitaciones o IDs validos." << endl;
            return false;
        }

        // Dimensionar los arreglos de punteros una vez que se conoce el tamaño máximo de ID
        habitaciones_cargadas_por_id = new Habitacion*[max_id_habitacion + 1];
        nodos_del_arbol_por_id = new NodoArbol*[max_id_habitacion + 1];
        // Inicializar los punteros a nullptr
        for (int i = 0; i <= max_id_habitacion; ++i) {
            habitaciones_cargadas_por_id[i] = nullptr;
            nodos_del_arbol_por_id[i] = nullptr;
        }

        // Reiniciar el archivo para la lectura real
        archivo.close();
        archivo.open(nombre_archivo);
        if (!archivo.is_open()) {
            cout << "Error: No se pudo reabrir el archivo " << nombre_archivo << endl;
            return false;
        }
        
        // --- Fase 2: Carga real de datos ---
        string current_line_buffer = ""; // Buffer para manejar la lectura de líneas

        while (getline(archivo, line)) {
            if (line.empty()) continue; // Ignorar líneas vacías

            if (line.find("HABITACIONES") != string::npos) {
                current_section = 1;
                getline(archivo, line); // Lee el número de habitaciones
                continue;
            }
            if (line.find("ARCOS") != string::npos) {
                current_section = 2;
                getline(archivo, line); // Lee el número de arcos
                continue;
            }
            if (line.find("ENEMIGOS") != string::npos) {
                current_section = 3;
                getline(archivo, line); // Lee el número de enemigos
                num_enemigos_cargados = stoi(line);
                enemigos_cargados = new PropEnemigo[num_enemigos_cargados];
                continue;
            }
            if (line.find("EVENTOS") != string::npos) {
                current_section = 4;
                getline(archivo, line); // Lee el número de eventos
                num_eventos_principales_cargados = stoi(line);
                eventos_principales_cargados = new EventoPrincipal[num_eventos_principales_cargados];
                // Consumir la línea '&' del primer evento
                getline(archivo, line); // Debería leer '&'
                continue;
            }
            if (line.find("MEJORAS_DE_COMBATE") != string::npos) {
                current_section = 5;
                // Las siguientes líneas se leerán directamente en el switch
                continue;
            }
            if (line.find("FIN_DE_ARCHIVO") != string::npos) {
                current_section = 0; // Terminar procesamiento
                break;
            }

            // Procesar la línea actual según la sección
            switch (current_section) {
                case 1: { // HABITACIONES
                    // Formato: ID Nombre (TIPO) \n Descripción
                    size_t pos_primer_espacio = line.find(' ');
                    int id = stoi(line.substr(0, pos_primer_espacio));

                    size_t pos_parentesis_abierto = line.find('(', pos_primer_espacio);
                    size_t pos_parentesis_cerrado = line.find(')', pos_parentesis_abierto);
                    string nombre_habitacion = line.substr(pos_primer_espacio + 1, pos_parentesis_abierto - (pos_primer_espacio + 1) - 1); // -1 para quitar el espacio antes del (

                    string tipo_str = line.substr(pos_parentesis_abierto + 1, pos_parentesis_cerrado - (pos_parentesis_abierto + 1));
                    TipoHabitacion tipo = stringToTipoHabitacion(tipo_str);

                    // Leer la línea de descripción (este es el cambio clave por el formato)
                    string descripcion_habitacion;
                    getline(archivo, descripcion_habitacion);
                    // Si la descripción está vacía, consume otra línea (puede haber líneas en blanco)
                    while (descripcion_habitacion.empty() && archivo.good()) {
                        getline(archivo, descripcion_habitacion);
                    }
                    
                    // Crear la Habitacion y el NodoArbol
                    Habitacion* nueva_habitacion = new Habitacion(id, nombre_habitacion, tipo, descripcion_habitacion);
                    NodoArbol* nuevo_nodo = new NodoArbol(nueva_habitacion);

                    // Almacenar en los arreglos para acceso rápido
                    if (id >= 0 && id <= max_id_habitacion) {
                        habitaciones_cargadas_por_id[id] = nueva_habitacion;
                        nodos_del_arbol_por_id[id] = nuevo_nodo;
                    } else {
                        cout << "Advertencia: ID de habitacion fuera de rango: " << id << endl;
                    }

                    if (id == 0) { // La habitación con ID 0 es la raíz del árbol
                        raiz = nuevo_nodo;
                    }
                    break;
                }
                case 2: { // ARCOS
                    // Formato: ID_ORIGEN -> ID_DESTINO
                    size_t pos_flecha = line.find(" -> ");
                    int id_origen = stoi(line.substr(0, pos_flecha));
                    int id_destino = stoi(line.substr(pos_flecha + 4)); // +4 para saltar " -> "

                    // Asegurarse de que los nodos existen antes de intentar conectarlos
                    if (id_origen >= 0 && id_origen <= max_id_habitacion && nodos_del_arbol_por_id[id_origen] != nullptr &&
                        id_destino >= 0 && id_destino <= max_id_habitacion && nodos_del_arbol_por_id[id_destino] != nullptr) {
                        
                        NodoArbol* nodo_origen = nodos_del_arbol_por_id[id_origen];
                        NodoArbol* nodo_destino = nodos_del_arbol_por_id[id_destino];

                        // Usar un arreglo para contar los hijos y asignarlos secuencialmente (izq, cen, der)
                        // Este arreglo se debe inicializar a 0 al principio de cargarMapa
                        static int* contador_hijos_por_nodo = nullptr;
                        if (contador_hijos_por_nodo == nullptr) {
                            contador_hijos_por_nodo = new int[max_id_habitacion + 1];
                            for (int i = 0; i <= max_id_habitacion; ++i) {
                                contador_hijos_por_nodo[i] = 0;
                            }
                        }

                        if (contador_hijos_por_nodo[id_origen] == 0) {
                            nodo_origen->hijo_izq = nodo_destino;
                        } else if (contador_hijos_por_nodo[id_origen] == 1) {
                            nodo_origen->hijo_cen = nodo_destino;
                        } else if (contador_hijos_por_nodo[id_origen] == 2) {
                            nodo_origen->hijo_der = nodo_destino;
                        }
                        // Solo incrementamos si hay un hijo asignado (para evitar que se incremente si hay más de 3 arcos)
                        if (contador_hijos_por_nodo[id_origen] < 3) {
                             contador_hijos_por_nodo[id_origen]++;
                        }

                    } else {
                        cout << "Advertencia: Arco invalido o nodo no existente: " << id_origen << " -> " << id_destino << endl;
                    }
                    break;
                }
                case 3: { // ENEMIGOS
                    // Formato: Nombre | Vida X | Ataque Y | Precision Z | Probabilidad P
                    static int enemigo_idx = 0; // Para llevar la cuenta del enemigo actual a cargar

                    size_t pos_vida = line.find("Vida ");
                    size_t pos_ataque = line.find("Ataque ");
                    size_t pos_precision = line.find("Precision ");
                    size_t pos_prob = line.find("Probabilidad ");

                    string nombre = line.substr(0, pos_vida - 3); // -3 para quitar " | "

                    int vida = stoi(line.substr(pos_vida + 5, pos_ataque - (pos_vida + 5) - 3)); // +5 para saltar "Vida " y -3 para " | "
                    int ataque = stoi(line.substr(pos_ataque + 7, pos_precision - (pos_ataque + 7) - 3)); // +7 para saltar "Ataque " y -3 para " | "
                    double precision = stod(line.substr(pos_precision + 10, pos_prob - (pos_precision + 10) - 3)); // +10 para saltar "Precision " y -3 para " | "
                    double probabilidad = stod(line.substr(pos_prob + 13)); // +13 para saltar "Probabilidad "

                    if (enemigo_idx < num_enemigos_cargados) {
                        enemigos_cargados[enemigo_idx].nombre = nombre;
                        enemigos_cargados[enemigo_idx].vida = vida;
                        enemigos_cargados[enemigo_idx].ataque = ataque;
                        enemigos_cargados[enemigo_idx].precision = precision;
                        enemigos_cargados[enemigo_idx].probabilidad_aparicion = probabilidad;
                        enemigo_idx++;
                    }
                    break;
                }
                case 4: { // EVENTOS
                    // Formato de evento:
                    // & (ya se consumió antes de entrar al switch si es el primer evento)
                    // Nombre
                    // Probabilidad X
                    // Descripcion
                    // A: Enunciado A
                    // Descripcion A
                    // Consecuencia A
                    // B: Enunciado B
                    // Descripcion B
                    // Consecuencia B

                    static int evento_idx = 0;
                    if (evento_idx >= num_eventos_principales_cargados) continue; // Si ya se cargaron todos los eventos

                    // Lee el Nombre del evento
                    eventos_principales_cargados[evento_idx].nombre = line;

                    // Lee la Probabilidad
                    getline(archivo, line);
                    size_t pos_prob = line.find("Probabilidad ");
                    eventos_principales_cargados[evento_idx].probabilidad = stod(line.substr(pos_prob + 13));

                    // Lee la Descripcion general
                    getline(archivo, line);
                    eventos_principales_cargados[evento_idx].descripcion_general = line;

                    // --- Opcion A ---
                    getline(archivo, line); // Lee "A: Enunciado A"
                    eventos_principales_cargados[evento_idx].opcion_A.id_opcion = 'A';
                    eventos_principales_cargados[evento_idx].opcion_A.enunciado = line.substr(3); // Saltar "A: "

                    getline(archivo, line); // Lee "Descripcion A"
                    eventos_principales_cargados[evento_idx].opcion_A.descripcion = line;

                    getline(archivo, line); // Lee "Consecuencia A"
                    LeerConsecuencia(line, eventos_principales_cargados[evento_idx].opcion_A);

                    // --- Opcion B ---
                    getline(archivo, line); // Lee "B: Enunciado B"
                    eventos_principales_cargados[evento_idx].opcion_B.id_opcion = 'B';
                    eventos_principales_cargados[evento_idx].opcion_B.enunciado = line.substr(3); // Saltar "B: "

                    getline(archivo, line); // Lee "Descripcion B"
                    eventos_principales_cargados[evento_idx].opcion_B.descripcion = line;

                    getline(archivo, line); // Lee "Consecuencia B"
                    LeerConsecuencia(line, eventos_principales_cargados[evento_idx].opcion_B);

                    evento_idx++;

                    // Consumir la siguiente línea '&' si no es el último evento
                    if (evento_idx < num_eventos_principales_cargados) {
                        getline(archivo, line); // Debería leer '&'
                    }
                    break;
                }
                case 5: { // MEJORAS_DE_COMBATE
                    // Formato: +X Vida / +X Precision / +X Ataque / +X Recuperacion
                    // No hay un contador para estas, se leen una por una y luego se sale de la sección

                    if (line.find("Vida") != string::npos) {
                        mejoras_combate_cargadas.vida = stoi(line.substr(0, line.find(" Vida")));
                    } else if (line.find("Precision") != string::npos) {
                        mejoras_combate_cargadas.precision = stod(line.substr(0, line.find(" Precision")));
                    } else if (line.find("Ataque") != string::npos) {
                        mejoras_combate_cargadas.ataque = stoi(line.substr(0, line.find(" Ataque")));
                    } else if (line.find("Recuperacion") != string::npos) {
                        mejoras_combate_cargadas.recuperacion = stoi(line.substr(0, line.find(" Recuperacion")));
                    }
                    break;
                }
            }
        }

        archivo.close();
        return true;
    }

    // Función para obtener un enemigo aleatorio basado en probabilidades
    Enemigo* generarEnemigoAleatorio() {
        if (num_enemigos_cargados == 0) {
            return nullptr; // No hay enemigos para generar
        }

        double total_probabilidad = 0.0;
        for (int i = 0; i < num_enemigos_cargados; ++i) {
            total_probabilidad += enemigos_cargados[i].probabilidad_aparicion;
        }

        uniform_real_distribution<> prob_dist(0.0, total_probabilidad);
        double r = prob_dist(gen); // Número aleatorio en el rango de probabilidades

        double prob_acumulada = 0.0;
        for (int i = 0; i < num_enemigos_cargados; ++i) {
            prob_acumulada += enemigos_cargados[i].probabilidad_aparicion;
            if (r <= prob_acumulada) {
                // Se encontró el enemigo
                PropEnemigo info = enemigos_cargados[i];
                return new Enemigo(info.nombre, info.vida, info.ataque, info.precision);
            }
        }
        return nullptr; // En caso de error (no debería llegar aquí si las probabilidades suman correctamente)
    }

    // Función para obtener un evento aleatorio basado en probabilidades
    EventoPrincipal* generarEventoAleatorio() {
        if (num_eventos_principales_cargados == 0) {
            return nullptr; // No hay eventos para generar
        }

        double total_probabilidad = 0.0;
        for (int i = 0; i < num_eventos_principales_cargados; ++i) {
            total_probabilidad += eventos_principales_cargados[i].probabilidad;
        }

        uniform_real_distribution<> prob_dist(0.0, total_probabilidad);
        double r = prob_dist(gen);

        double prob_acumulada = 0.0;
        for (int i = 0; i < num_eventos_principales_cargados; ++i) {
            prob_acumulada += eventos_principales_cargados[i].probabilidad;
            if (r <= prob_acumulada) {
                return &eventos_principales_cargados[i]; // Retorna un puntero al evento cargado (no se crea copia)
            }
        }
        return nullptr;
    }
};

// --- Funciones de Lógica de Juego ---

// Función para simular un combate
void iniciarCombate(Jugador* elara, ColaEnemigos* enemigos_en_combate, ArbolTernario* mi_arbol) {
    cout << "\n--- ¡COMBATE INICIADO! ---" << endl;

    while (elara->estaVivo() && !enemigos_en_combate->isEmpty()) {
        Enemigo* enemigo_actual = enemigos_en_combate->peek(); // Ver al enemigo del frente
        cout << "\n--------------------------" << endl;
        cout << "Vida de Elara: " << elara->vida << endl;
        cout << "Te enfrentas a: " << enemigo_actual->nombre << " (Vida: " << enemigo_actual->vida << ")" << endl;

        // Turno del jugador
        cout << "Presiona ENTER para atacar!" << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia el buffer antes de cin.get
        cin.get(); // Espera una pulsación de tecla (Enter)

        bool ataque_exitoso = elara->atacar(enemigo_actual);
        if (ataque_exitoso) {
            cout << "¡Has golpeado a " << enemigo_actual->nombre << "!" << endl;
            if (!enemigo_actual->estaVivo()) {
                cout << enemigo_actual->nombre << " ha sido derrotado!" << endl;
                enemigos_en_combate->dequeue(); // Sacar al enemigo derrotado de la cola
                // El destructor de NodoColaEnemigos se encarga de 'delete enemigo_actual;'
            }
        } else {
            cout << "¡Tu ataque falló!" << endl;
        }

        // Si elara sigue viva y el enemigo actual sigue vivo (o era el último y no hay más)
        if (elara->estaVivo() && enemigo_actual->estaVivo()) {
            // Turno del enemigo
            bool enemigo_impacta = enemigo_actual->atacar(elara);
            if (enemigo_impacta) {
                cout << enemigo_actual->nombre << " te ha golpeado! Vida restante: " << elara->vida << endl;
            } else {
                cout << enemigo_actual->nombre << " falló su ataque!" << endl;
            }
        }
    }

    cout << "\n--- FIN DE COMBATE ---" << endl;

    if (!elara->estaVivo()) {
        cout << "Elara ha sido derrotada... ¡GAME OVER!" << endl;
    } else {
        cout << "¡Has ganado el combate!" << endl;
        // Requisito 7: Recuperación de vida automática post-combate
        cout << "Elara se recupera " << elara->recuperacion << " puntos de vida." << endl;
        elara->curarVida(elara->recuperacion);
        cout << "Vida actual de Elara: " << elara->vida << endl;

        // Requisito 5: El jugador escoge UNA mejora
        cout << "\n¡Has obtenido mejoras por tu victoria! Escoge UNA para aumentar:" << endl;
        cout << "1. Vida (+" << mi_arbol->mejoras_combate_cargadas.vida << ")" << endl;
        cout << "2. Ataque (+" << mi_arbol->mejoras_combate_cargadas.ataque << ")" << endl;
        cout << "3. Precision (+" << mi_arbol->mejoras_combate_cargadas.precision << ")" << endl;
        cout << "4. Recuperacion (+" << mi_arbol->mejoras_combate_cargadas.recuperacion << ")" << endl;
        
        int opcion_mejora;
        bool opcion_valida = false;
        while (!opcion_valida) {
            cout << "Elige una opcion (1-4): ";
            cin >> opcion_mejora;
            if (cin.fail() || opcion_mejora < 1 || opcion_mejora > 4) {
                cout << "Opcion invalida. Intenta de nuevo." << endl;
                cin.clear(); // Limpiar el estado de error de cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descartar entrada inválida
            } else {
                opcion_valida = true;
            }
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar buffer después de leer int

        switch (opcion_mejora) {
            case 1:
                elara->curarVida(mi_arbol->mejoras_combate_cargadas.vida); // Aumenta vida actual
                cout << "¡Vida de Elara aumentada! Vida actual: " << elara->vida << endl;
                break;
            case 2:
                elara->aumentarAtaque(mi_arbol->mejoras_combate_cargadas.ataque);
                cout << "¡Ataque de Elara aumentado! Ataque actual: " << elara->ataque << endl;
                break;
            case 3:
                elara->aumentarPrecision(mi_arbol->mejoras_combate_cargadas.precision);
                cout << "¡Precision de Elara aumentada! Precision actual: " << elara->precision << endl;
                break;
            case 4:
                elara->aumentarRecuperacion(mi_arbol->mejoras_combate_cargadas.recuperacion);
                cout << "¡Recuperacion de Elara aumentada! Recuperacion actual: " << elara->recuperacion << endl;
                break;
        }
    }
}

// Función para resolver un evento
void resolverEvento(Jugador* elara, EventoPrincipal* evento, NodoArbol*& current_node, NodoArbol* previous_node) {
    cout << "\n--- EVENTO: " << evento->nombre << " ---" << endl;
    cout << evento->descripcion_general << endl;

    cout << "\nOpciones:" << endl;
    cout << "A: " << evento->opcion_A.enunciado << endl;
    cout << "B: " << evento->opcion_B.enunciado << endl;

    char eleccion;
    bool opcion_valida = false;
    while (!opcion_valida) {
        cout << "Elige una opcion (A/B): ";
        cin >> eleccion;
        eleccion = (char)toupper(eleccion); // Convertir a mayúscula
        if (cin.fail() || (eleccion != 'A' && eleccion != 'B')) {
            cout << "Opcion invalida. Intenta de nuevo." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            opcion_valida = true;
        }
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar buffer

    OpcionEvento* opcion_elegida = nullptr;
    if (eleccion == 'A') {
        opcion_elegida = &evento->opcion_A;
    } else {
        opcion_elegida = &evento->opcion_B;
    }

    cout << "\nRespuesta: " << opcion_elegida->descripcion << endl;

    // Aplicar consecuencias
    if (opcion_elegida->consec_danio > 0) {
        elara->recibirDanio(opcion_elegida->consec_danio);
        cout << "Elara recibe " << opcion_elegida->consec_danio << " de daño. Vida actual: " << elara->vida << endl;
    }
    if (opcion_elegida->consec_cura > 0) {
        elara->curarVida(opcion_elegida->consec_cura);
        cout << "Elara se cura " << opcion_elegida->consec_cura << " puntos de vida. Vida actual: " << elara->vida << endl;
    }
    if (opcion_elegida->consec_mejora_vida > 0) {
        elara->curarVida(opcion_elegida->consec_mejora_vida); // Aumenta vida actual
        cout << "La vida de Elara ha sido mejorada. Vida actual: " << elara->vida << endl;
    }
    if (opcion_elegida->consec_mejora_ataque > 0) {
        elara->aumentarAtaque(opcion_elegida->consec_mejora_ataque);
        cout << "El ataque de Elara ha sido mejorado. Ataque actual: " << elara->ataque << endl;
    }
    if (opcion_elegida->consec_mejora_precision > 0.0) {
        elara->aumentarPrecision(opcion_elegida->consec_mejora_precision);
        cout << "La precision de Elara ha sido mejorada. Precision actual: " << elara->precision << endl;
    }
    if (opcion_elegida->consec_mejora_recuperacion > 0) {
        elara->aumentarRecuperacion(opcion_elegida->consec_mejora_recuperacion);
        cout << "La recuperacion de Elara ha sido mejorada. Recuperacion actual: " << elara->recuperacion << endl;
    }
    if (opcion_elegida->consec_volver_anterior) {
        if (previous_node != nullptr) {
            current_node = previous_node; // Regresa al nodo ante-anterior
            cout << "Has vuelto a la habitacion anterior!" << endl;
        } else {
            cout << "No hay una habitacion anterior a la que volver." << endl;
        }
    }
    
    // Requisito 7: Recuperación de vida automática post-evento (si no hubo cura/daño directo en el evento)
    // Se aplica solo si el evento no tuvo una consecuencia directa de vida (curar/dañar)
    if (opcion_elegida->consec_cura == 0 && opcion_elegida->consec_danio == 0) {
        cout << "Elara se recupera " << elara->recuperacion << " puntos de vida." << endl;
        elara->curarVida(elara->recuperacion);
        cout << "Vida actual de Elara: " << elara->vida << endl;
    }

    cout << "Presiona ENTER para continuar..." << endl;
    cin.get();
}


int main() {
    // Inicializar jugador Elara
    Jugador elara(100, 10, 0.7, 5); // Vida, Ataque, Precision, Recuperacion

    // Crear y cargar el mapa del juego
    ArbolTernario mi_arbol;
    if (!mi_arbol.cargarMapa("ejemplo.map")) {
        cout << "Fallo al cargar el mapa. Saliendo del juego." << endl;
        return 1;
    }

    // Inicializar la posición actual del jugador
    NodoArbol* current_node = mi_arbol.raiz;
    NodoArbol* previous_node = nullptr; // Para el requisito de volver al nodo anterior (Playa)

    cout << "¡Bienvenido a la Aventura de Estructuras de Datos!" << endl;

    while (elara.estaVivo() && current_node != nullptr) {
        cout << "\n----------------------------------------" << endl;
        cout << "Estas en: " << current_node->habitacion->nombre << " (ID: " << current_node->habitacion->id << ")" << endl;
        cout << "Descripcion: " << current_node->habitacion->descripcion_habitacion << endl;
        cout << "Vida de Elara: " << elara.vida << " | Ataque: " << elara.ataque 
             << " | Precision: " << elara.precision << " | Recuperacion: " << elara.recuperacion << endl;
        cout << "----------------------------------------" << endl;

        switch (current_node->habitacion->tipo_habitacion) {
            case HabInicio:
                // Solo un punto de partida, no hay acción especial
                cout << "Es el inicio de tu aventura. Explora los caminos." << endl;
                break;
            case HabEvento: {
                EventoPrincipal* evento_elegido = mi_arbol.generarEventoAleatorio();
                if (evento_elegido != nullptr) {
                    resolverEvento(&elara, evento_elegido, current_node, previous_node);
                    // previous_node podría haber cambiado si el evento era el de "volver atrás"
                } else {
                    cout << "Error: No se pudo generar un evento aleatorio." << endl;
                }
                break;
            }
            case HabCombate: {
                ColaEnemigos cola_combate;
                // Generar 1 a 3 enemigos aleatorios para el combate
                uniform_int_distribution<> num_enemigos_dis(1, 3);
                int cantidad_enemigos = num_enemigos_dis(gen);
                cout << "¡Te enfrentas a " << cantidad_enemigos << " enemigos!" << endl;

                for (int i = 0; i < cantidad_enemigos; ++i) {
                    Enemigo* nuevo_enemigo = mi_arbol.generarEnemigoAleatorio();
                    if (nuevo_enemigo != nullptr) {
                        cola_combate.enqueue(nuevo_enemigo);
                    } else {
                        cout << "Advertencia: No se pudo generar un enemigo." << endl;
                        break;
                    }
                }
                
                iniciarCombate(&elara, &cola_combate, &mi_arbol); // Pasar el arbol para las mejoras
                
                if (!elara.estaVivo()) {
                    break; // Salir del bucle principal si Elara muere
                }
                break;
            }
            case HabFin:
                cout << "¡Has llegado a un final!" << endl;
                // Manejar requisito especial 3: Playa (ID 9)
                if (current_node->habitacion->id == 9) {
                    if (previous_node != nullptr) {
                        current_node = previous_node; // Vuelve al nodo anterior
                        cout << "El tiempo se distorsiona... ¡Vuelves a la habitacion anterior!" << endl;
                        cout << "Presiona ENTER para continuar..." << endl;
                        cin.get();
                        continue; // Vuelve al inicio del bucle para procesar el nodo anterior
                    } else {
                        cout << "Pero no hay un lugar anterior al que volver. Fin del juego." << endl;
                        elara.recibirDanio(elara.vida); // Simula el fin del juego
                    }
                } else {
                    elara.recibirDanio(elara.vida); // Simula el fin del juego para otros finales
                }
                break;
            case HabErroneo:
                cout << "Error: Tipo de habitacion desconocido. Saliendo del juego." << endl;
                elara.recibirDanio(elara.vida); // Terminar el juego
                break;
        }

        if (!elara.estaVivo()) {
            cout << "\nLa aventura de Elara ha terminado." << endl;
            break; // Game Over
        }

        // Mostrar opciones de movimiento
        cout << "\n¿A donde quieres ir?" << endl;
        if (current_node->hijo_izq != nullptr) {
            cout << "1. " << current_node->hijo_izq->habitacion->nombre << endl;
        }
        if (current_node->hijo_cen != nullptr) {
            cout << "2. " << current_node->hijo_cen->habitacion->nombre << endl;
        }
        if (current_node->hijo_der != nullptr) {
            cout << "3. " << current_node->hijo_der->habitacion->nombre << endl;
        }
        cout << "0. Salir del juego" << endl;

        int eleccion_camino;
        bool camino_valido = false;
        while (!camino_valido) {
            cout << "Elige un camino: ";
            cin >> eleccion_camino;
            if (cin.fail()) {
                cout << "Entrada invalida. Intenta de nuevo." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else if (eleccion_camino == 0) {
                camino_valido = true; // Salir
            } else if (eleccion_camino == 1 && current_node->hijo_izq != nullptr) {
                camino_valido = true;
            } else if (eleccion_camino == 2 && current_node->hijo_cen != nullptr) {
                camino_valido = true;
            } else if (eleccion_camino == 3 && current_node->hijo_der != nullptr) {
                camino_valido = true;
            } else {
                cout << "Opcion de camino invalida o no existe. Intenta de nuevo." << endl;
            }
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar buffer

        if (eleccion_camino == 0) {
            cout << "Elara decide terminar su aventura. ¡Hasta pronto!" << endl;
            break; // Salir del juego
        } else {
            previous_node = current_node; // Guardar el nodo actual antes de moverse
            if (eleccion_camino == 1) {
                current_node = current_node->hijo_izq;
            } else if (eleccion_camino == 2) {
                current_node = current_node->hijo_cen;
            } else if (eleccion_camino == 3) {
                current_node = current_node->hijo_der;
            }
        }
    }

    if (!elara.estaVivo()) {
        cout << "\n¡La aventura ha terminado! Elara no ha sobrevivido." << endl;
    } else {
        cout << "\n¡Gracias por jugar!" << endl;
    }

    // El destructor de mi_arbol se encargará de liberar toda la memoria dinámica.
    return 0;
}