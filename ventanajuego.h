#ifndef VENTANAJUEGO_H
#define VENTANAJUEGO_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include "tablero.h"
#include "enemigo.h"
#include "escenatablero.h"

class VentanaJuego : public QMainWindow {
    Q_OBJECT

private:
    Tablero* tableroJugador;
    Tablero* tableroEnemigo;
    Enemigo* enemigoIA;

    // Arreglo dinámico propio de naves del jugador (sin QList/vector)
    Naves** navesJugador;
    int totalNaves;
    int indiceNaveActual;
    bool orientacionHorizontal;
    bool faseCombate;
    bool turnoJugador;

    // Temporizador por turno
    QTimer* timerTurno;
    int segundosRestantes;
    int disparosRealizados;
    int disparosAcertados;

    // Componentes visuales
    EscenaTablero* escenaJugador;
    EscenaTablero* escenaEnemigo;
    QGraphicsView* vistaJugador;
    QGraphicsView* vistaEnemigo;

    QLabel* lblEstado;
    QLabel* lblTiempo;
    QLabel* lblEstadisticas;
    QPushButton* btnRotar;
    QPushButton* btnAleatorio;

    void iniciarNivel(int nivel);
    void liberarNaves();
    void comenzarCombate();
    void actualizarEtiquetas();

private slots:
    void manejarClicJugador(int fila, int col);
    void manejarClicEnemigo(int fila, int col);
    void rotarBarco();
    void despliegueAleatorio();
    void tickTimer();
    void procesarDisparoIA(int fila, int col);

public:
    explicit VentanaJuego(QWidget* parent = nullptr);
    ~VentanaJuego() override;
};

#endif // VENTANAJUEGO_H