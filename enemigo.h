#ifndef ENEMIGO_H
#define ENEMIGO_H

#include <QThread>
#include "tablero.h"

struct Coordenada{
    int fila;
    int col;
};

struct NodoCoord{
    Coordenada dato;
    NodoCoord* siguiente;

    NodoCoord(int f, int c) : dato{f,c}, siguiente(nullptr){}
};

class PilaCoordenadas{
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

enum class EstadoIA{
    CAZA,
    DESTRUCCION
};

class Enemigo : public QThread{
    Q_OBJECT

private:
    int nivel;
    int n;
    EstadoIA estadoActual;

    Tablero* tableroObjetivo;

    int** registroRadar;
    int** mapaProbabilidad;

    int* tamaniosBarcos;
    bool* barcosVivos;
    int cantidadBarcosNivel;

    PilaCoordenadas pilaObjetivos;

    void reservarMemoria(int tam, int cantBarcos);
    void liberarMemoria();
    bool coordenadaValida(int fila, int col) const;

    void apilarVecinosCardinales(int fila, int col);
    bool hayImpactosPendientes() const;
    void reconstruirPilaDesdeImpactos();
    void marcarBarcoHundidoEnRadar(int fila, int col);
    int contarConectadosRecursivo(int fila, int col, bool** visitados);
    int obtenerMinimoBarcoVivo() const;
    void evaluarColocaciones(int tamBarco, bool horizontal, bool modoImpactoActivo);

    Coordenada calcularDisparoNivel1();
    Coordenada calcularDisparoNivel2();
    Coordenada calcularDisparoNivel3();

protected:
    void run() override;

public:
    explicit Enemigo(QObject* parent=nullptr);
    ~Enemigo() override;

    void configurarNivel(int nuevoNivel, Tablero* tableroJugador);
    void reiniciar();
    void cancelarTurno();

    void registrarResultadoDisparo(int fila, int col, int resultado);

    int getNivel() const;
    EstadoIA getEstadoActual() const;

signals:
    void disparoCalculado(int fila, int col);
};

#endif