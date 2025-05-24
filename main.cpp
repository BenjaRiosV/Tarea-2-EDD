// Main de la Tarea 2 EDD

#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <limits>

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
};

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

    // Destructor: Libera toda la memoria ocupada por los nodos de la cola.
    ~ColaEnemigos() {
        while (!isEmpty()) {
            Enemigo* enemigo_desencolado = dequeue();
            if (enemigo_desencolado != nullptr) {
            }
        }
    }

    // Método para añadir un enemigo al final de la cola.
    void enqueue(Enemigo* e) {
        NodoColaEnemigos* nuevoNodo = new NodoColaEnemigos(e);

        if (isEmpty()) {
            frente = nuevoNodo; // Si la cola está vacía, el nuevo nodo es el primero y el último
            final = nuevoNodo;
        } else {
            final->siguiente = nuevoNodo; // El último nodo actual apunta al nuevo nodo
            final = nuevoNodo;            // El nuevo nodo se convierte en el nuevo último
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

        delete nodo_a_eliminar;

        tamano--;

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
        return tamano == 0;
    }

    // Método para obtener el tamaño actual de la cola.
    int getTamano() {
        return tamano;
    }
};





int main() {
    
    return 0;
}