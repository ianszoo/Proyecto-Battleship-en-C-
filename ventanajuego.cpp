#include "ventanajuego.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

VentanaJuego::VentanaJuego(QWidget* parent)
    : QMainWindow(parent),
    tableroJugador(nullptr),
    tableroEnemigo(nullptr),
    enemigoIA(new Enemigo(this)),
    navesJugador(nullptr),
    totalNaves(0),
    indiceNaveActual(0),
    orientacionHorizontal(true),
    faseCombate(false),
    turnoJugador(false),
    timerTurno(new QTimer(this)),
    segundosRestantes(15),
    disparosRealizados(0),
    disparosAcertados(0),
    escenaJugador(nullptr),
    escenaEnemigo(nullptr) {

    setWindowTitle("Batalla Naval - Combate");
    setStyleSheet("background-color: #0b192c; color: white;");

    QWidget* central = new QWidget(this);
    QVBoxLayout* layoutPrincipal = new QVBoxLayout(central);

    // Barra superior de información
    QHBoxLayout* barraSuperior = new QHBoxLayout();
    lblEstado = new QLabel("Despliegue: Coloca tus barcos (Clic Der. para rotar)", this);
    lblTiempo = new QLabel("Tiempo: 15s", this);
    lblEstadisticas = new QLabel("Disparos: 0 | Aciertos: 0", this);
    btnRotar = new QPushButton("Rotar: Horizontal", this);
    btnAleatorio = new QPushButton("Colocar Aleatorio", this);

    barraSuperior->addWidget(lblEstado);
    barraSuperior->addWidget(lblTiempo);
    barraSuperior->addWidget(lblEstadisticas);
    barraSuperior->addWidget(btnRotar);
    barraSuperior->addWidget(btnAleatorio);
    layoutPrincipal->addLayout(barraSuperior);

    // Dos tableros lado a lado
    QHBoxLayout* zonaTableros = new QHBoxLayout();
    vistaJugador = new QGraphicsView(this);
    vistaEnemigo = new QGraphicsView(this);
    vistaJugador->setFixedSize(506, 506);
    vistaEnemigo->setFixedSize(506, 506);

    zonaTableros->addWidget(vistaJugador);
    zonaTableros->addWidget(vistaEnemigo);
    layoutPrincipal->addLayout(zonaTableros);

    setCentralWidget(central);

    connect(btnRotar, &QPushButton::clicked, this, &VentanaJuego::rotarBarco);
    connect(btnAleatorio, &QPushButton::clicked, this, &VentanaJuego::despliegueAleatorio);
    connect(timerTurno, &QTimer::timeout, this, &VentanaJuego::tickTimer);
    connect(enemigoIA, &Enemigo::disparoCalculado, this, &VentanaJuego::procesarDisparoIA);
    iniciarNivel(2);
}

VentanaJuego::~VentanaJuego() {
    timerTurno->stop();
    if (enemigoIA != nullptr) {
        enemigoIA->cancelarTurno();
    }
    delete tableroJugador;
    delete tableroEnemigo;
    liberarNaves();
}

void VentanaJuego::liberarNaves() {
    if (navesJugador != nullptr) {
        delete[] navesJugador;
        navesJugador = nullptr;
    }
}

void VentanaJuego::iniciarNivel(int nivel) {
    timerTurno->stop();
    faseCombate = false;
    turnoJugador = false;
    indiceNaveActual = 0;
    disparosRealizados = 0;
    disparosAcertados = 0;

    int tam = (nivel == 1) ? 8 : (nivel == 2 ? 10 : 14);
    int cant = (nivel == 1) ? 3 : (nivel == 2 ? 5 : 7);

    delete tableroJugador;
    delete tableroEnemigo;
    liberarNaves();

    tableroJugador = new Tablero(tam, cant);
    tableroEnemigo = new Tablero(tam, cant);
    totalNaves = cant;
    navesJugador = new Naves*[totalNaves];
    if (nivel == 1) {
        navesJugador[0] = new Naves("Crucero", 3);
        navesJugador[1] = new Naves("Submarino", 3);
        navesJugador[2] = new Naves("Destructor", 2);
    } else if (nivel == 2) {
        navesJugador[0] = new Naves("Portaaviones", 5);
        navesJugador[1] = new Naves("Acorazado", 4);
        navesJugador[2] = new Naves("Crucero", 3);
        navesJugador[3] = new Naves("Submarino", 3);
        navesJugador[4] = new Naves("Destructor", 2);
    } else {
        navesJugador[0] = new Naves("Portaaviones", 5);
        navesJugador[1] = new Naves("Acorazado", 4);
        navesJugador[2] = new Naves("Crucero 1", 3);
        navesJugador[3] = new Naves("Crucero 2", 3);
        navesJugador[4] = new Naves("Submarino", 3);
        navesJugador[5] = new Naves("Destructor 1", 2);
        navesJugador[6] = new Naves("Destructor 2", 2);
    }

    for (int i = 0; i < totalNaves; ++i) {
        tableroJugador->agregarNave(navesJugador[i]);
    }

    // Crear y colocar flota enemiga aleatoria
    if (nivel == 2) {
        tableroEnemigo->agregarNave(new Naves("Portaaviones", 5));
        tableroEnemigo->agregarNave(new Naves("Acorazado", 4));
        tableroEnemigo->agregarNave(new Naves("Crucero", 3));
        tableroEnemigo->agregarNave(new Naves("Submarino", 3));
        tableroEnemigo->agregarNave(new Naves("Destructor", 2));
        tableroEnemigo->colocarFlotaAleatoria();
        tableroEnemigo->colocarMinas(3);
    }

    enemigoIA->configurarNivel(nivel, tableroJugador);

    delete escenaJugador;
    delete escenaEnemigo;
    escenaJugador = new EscenaTablero(tableroJugador, false, this);
    escenaEnemigo = new EscenaTablero(tableroEnemigo, true, this);
    escenaJugador->setNavesVisuales(navesJugador, totalNaves);

    vistaJugador->setScene(escenaJugador);
    vistaEnemigo->setScene(escenaEnemigo);

    connect(escenaJugador, &EscenaTablero::celdaClickeada, this, &VentanaJuego::manejarClicJugador);
    connect(escenaJugador, &EscenaTablero::clicDerechoRotar, this, &VentanaJuego::rotarBarco);
    connect(escenaEnemigo, &EscenaTablero::celdaClickeada, this, &VentanaJuego::manejarClicEnemigo);

    escenaJugador->actualizarVista();
    escenaEnemigo->actualizarVista();
    actualizarEtiquetas();
}

void VentanaJuego::rotarBarco() {
    if (faseCombate) {
        return;
    }
    orientacionHorizontal = !orientacionHorizontal;
    btnRotar->setText(orientacionHorizontal ? "Rotar: Horizontal" : "Rotar: Vertical");
}

void VentanaJuego::despliegueAleatorio() {
    if (faseCombate) {
        return;
    }
    tableroJugador->colocarFlotaAleatoria();
    escenaJugador->actualizarVista();
    comenzarCombate();
}

void VentanaJuego::manejarClicJugador(int fila, int col) {
    if (faseCombate || indiceNaveActual >= totalNaves) {
        return;
    }

    if (tableroJugador->colocarNave(indiceNaveActual, fila, col, orientacionHorizontal)) {
        indiceNaveActual++;
        escenaJugador->actualizarVista();

        if (indiceNaveActual >= totalNaves) {
            comenzarCombate();
        } else {
            actualizarEtiquetas();
        }
    }
}

void VentanaJuego::comenzarCombate() {
    faseCombate = true;
    turnoJugador = true;
    btnRotar->setEnabled(false);
    btnAleatorio->setEnabled(false);

    segundosRestantes = 15;
    timerTurno->start(1000);
    actualizarEtiquetas();
}

void VentanaJuego::manejarClicEnemigo(int fila, int col) {
    if (!faseCombate || !turnoJugador) {
        return;
    }

    int res = tableroEnemigo->disparar(fila, col);
    if (res == -1) {
        return; // Coordenada inválida o repetida
    }

    disparosRealizados++;
    if (res == 1 || res == 2) {
        disparosAcertados++;
    }

    escenaEnemigo->actualizarVista();
    actualizarEtiquetas();

    if (tableroEnemigo->flotaDestruida()) {
        timerTurno->stop();
        QMessageBox::information(this, "¡Victoria!", "Has hundido toda la flota enemiga.");
        return;
    }

    // Pasa turno a la IA en segundo plano (QThread)
    turnoJugador = false;
    timerTurno->stop();
    lblEstado->setText("Turno del Enemigo...");
    enemigoIA->start();
}

void VentanaJuego::tickTimer() {
    if (!faseCombate || !turnoJugador) {
        return;
    }

    segundosRestantes--;
    lblTiempo->setText(QString("Tiempo: %1s").arg(segundosRestantes));

    if (segundosRestantes <= 0) {
        // Pierde el turno por agotarse el tiempo
        turnoJugador = false;
        timerTurno->stop();
        lblEstado->setText("¡Tiempo agotado! Turno del Enemigo...");
        enemigoIA->start();
    }
}

void VentanaJuego::procesarDisparoIA(int fila, int col) {
    int res = tableroJugador->disparar(fila, col);
    enemigoIA->registrarResultadoDisparo(fila, col, res);

    escenaJugador->actualizarVista();

    if (tableroJugador->flotaDestruida()) {
        QMessageBox::critical(this, "Derrota", "Tu flota ha sido destruida.");
        return;
    }

    // Regresa el turno al jugador
    turnoJugador = true;
    segundosRestantes = 15;
    timerTurno->start(1000);
    actualizarEtiquetas();
}

void VentanaJuego::actualizarEtiquetas() {
    if (!faseCombate && indiceNaveActual < totalNaves) {
        lblEstado->setText(QString("Coloca tu %1 (%2 celdas)")
                               .arg(navesJugador[indiceNaveActual]->getNombre())
                               .arg(navesJugador[indiceNaveActual]->getTamano()));
    } else if (turnoJugador) {
        lblEstado->setText("¡Tu turno! Dispara en el radar enemigo.");
    }

    lblTiempo->setText(QString("Tiempo: %1s").arg(segundosRestantes));
    lblEstadisticas->setText(QString("Disparos: %1 | Aciertos: %2")
                                 .arg(disparosRealizados)
                                 .arg(disparosAcertados));
}