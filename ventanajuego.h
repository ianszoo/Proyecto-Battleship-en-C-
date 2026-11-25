#ifndef VENTANAJUEGO_H
#define VENTANAJUEGO_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include "jugador.h"
#include "enemigo.h"
#include "escenatablero.h"

class VentanaJuego : public QMainWindow{
    Q_OBJECT

private:
    Jugador* jugador;
    Tablero* tableroEnemigo;
    Enemigo* enemigoIA;

    bool faseCombate;
    bool turnoJugador;

    QTimer* timerTurno;
    int segundosRestantes;

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
    void comenzarCombate();
    void iniciarTurnoEnemigo(const QString& mensaje);
    void actualizarEtiquetas();

private slots:
    void manejarClicJugador(int fila, int col);
    void manejarClicEnemigo(int fila, int col);
    void rotarBarco();
    void despliegueAleatorio();
    void tickTimer();
    void procesarDisparoIA(int fila, int col);

public:
    explicit VentanaJuego(QWidget* parent=nullptr);
    ~VentanaJuego() override;
};

#endif