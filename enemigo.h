#ifndef ENEMIGO_H
#define ENEMIGO_H

#include <QThread>
#include "tablero.h"

struct Coordenada {
    int fila;
    int col;
};

struct NodoCoord {
    Coordenada dato;
    NodoCoord* siguiente;

    NodoCoord(int f, int c) : dato{f, c}, siguiente(nullptr) {}
};

// Pila dinámica propia (sin vector/QList) para el rastreo cardinal de la IA
class PilaCoordenadas {
private:
    NodoCoord* tope;

public:
    PilaCoordenadas();
    ~PilaCoordenadas();

    void push(int fila, int col);
    bool pop(Coordenada& destino);
    bool estaVacia() const;
    void limpiar();
};

enum class EstadoIA {
    CAZA,        // Buscando un barco nuevo
    DESTRUCCION  // Ya impactó; rastrea arriba, abajo, izquierda y derecha
};

class Enemigo : public QThread {
    Q_OBJECT

private:
    int nivel;
    int n; // Tamaño del tablero (8, 10 o 14)
    EstadoIA estadoActual;

    // Puntero de referencia al tablero del jugador (no es dueño de su memoria)
    Tablero* tableroObjetivo;

    // Matrices dinámicas con punteros dobles
    // registroRadar: 0 = Sin disparar, 1 = Agua, 2 = Impacto vivo, 3 = Hundido
    int** registroRadar;
    int** mapaProbabilidad;

    // Control de barcos vivos del jugador para el mapa de probabilidad (Nivel 3)
    int* tamaniosBarcos;
    bool* barcosVivos;
    int cantidadBarcosNivel;

    // Pila propia para búsqueda adyacente (Nivel 2 y respaldo Nivel 3)
    PilaCoordenadas pilaObjetivos;

    // Gestión de memoria dinámica
    void reservarMemoria(int tam, int cantBarcos);
    void liberarMemoria();
    bool coordenadaValida(int fila, int col) const;

    // Auxiliares de IA
    void apilarVecinosCardinales(int fila, int col);
    bool hayImpactosPendientes() const;
    void reconstruirPilaDesdeImpactos();
    void marcarBarcoHundidoEnRadar(int fila, int col);
    int contarConectadosRecursivo(int fila, int col, bool** visitados);
    int obtenerMinimoBarcoVivo() const;

    // Algoritmos para los 3 niveles
    Coordenada calcularDisparoNivel1();
    Coordenada calcularDisparoNivel2();
    Coordenada calcularDisparoNivel3();

protected:
    // Se ejecuta en hilo secundario para no congelar la interfaz gráfica
    void run() override;

public:
    explicit Enemigo(QObject* parent = nullptr);
    ~Enemigo() override;

    // Configura la IA según el nivel (1, 2 o 3) y el tablero del jugador
    void configurarNivel(int nuevoNivel, Tablero* tableroJugador);
    void reiniciar();
    void cancelarTurno();

    // Actualiza la IA con el resultado de Tablero::disparar(fila, col):
    // 0 = agua, 1 = impacto, 2 = hundido, -1 = inválido
    void registrarResultadoDisparo(int fila, int col, int resultado);

    int getNivel() const;
    EstadoIA getEstadoActual() const;

signals:
    // Se emite al hilo principal cuando la IA termina de elegir su disparo
    void disparoCalculado(int fila, int col);
};

#endif // ENEMIGO_H