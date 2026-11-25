#include "ventanajuego.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <cstdlib>
#include <ctime>

VentanaJuego::VentanaJuego(QWidget* parent)
    : QMainWindow(parent),
    jugador(new Jugador("Lider")),
    tableroEnemigo(nullptr),
    enemigoIA(new Enemigo(this)),
    faseCombate(false),
    turnoJugador(false),
    timerTurno(new QTimer(this)),
    segundosRestantes(15),
    escenaJugador(nullptr),
    escenaEnemigo(nullptr){

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    setWindowTitle("Batalla Naval - Combate");
    setStyleSheet("background-color: #0b192c; color: white;");

    QWidget* central=new QWidget(this);
    QVBoxLayout* layoutPrincipal=new QVBoxLayout(central);

    QHBoxLayout* barraSuperior=new QHBoxLayout();
    lblEstado=new QLabel("Despliegue: Coloca tus barcos (Clic Der. para rotar)",this);
    lblTiempo=new QLabel("Tiempo: 15s",this);
    lblEstadisticas=new QLabel("Disparos: 0 | Aciertos: 0",this);
    btnRotar=new QPushButton("Rotar: Horizontal",this);
    btnAleatorio=new QPushButton("Colocar Aleatorio",this);

    barraSuperior->addWidget(lblEstado);
    barraSuperior->addWidget(lblTiempo);
    barraSuperior->addWidget(lblEstadisticas);
    barraSuperior->addWidget(btnRotar);
    barraSuperior->addWidget(btnAleatorio);
    layoutPrincipal->addLayout(barraSuperior);

    QHBoxLayout* zonaTableros=new QHBoxLayout();
    vistaJugador=new QGraphicsView(this);
    vistaEnemigo=new QGraphicsView(this);
    vistaJugador->setFixedSize(506,506);
    vistaEnemigo->setFixedSize(506,506);

    zonaTableros->addWidget(vistaJugador);
    zonaTableros->addWidget(vistaEnemigo);
    layoutPrincipal->addLayout(zonaTableros);

    setCentralWidget(central);

    connect(btnRotar,&QPushButton::clicked,this,&VentanaJuego::rotarBarco);
    connect(btnAleatorio,&QPushButton::clicked,this,&VentanaJuego::despliegueAleatorio);
    connect(timerTurno,&QTimer::timeout,this,&VentanaJuego::tickTimer);
    connect(enemigoIA,&Enemigo::disparoCalculado,this,&VentanaJuego::procesarDisparoIA);

    iniciarNivel(2);
}

VentanaJuego::~VentanaJuego(){
    timerTurno->stop();
    if(enemigoIA!=nullptr){
        enemigoIA->cancelarTurno();
    }
    delete jugador;
    delete tableroEnemigo;
}

void VentanaJuego::iniciarNivel(int nivel){
    timerTurno->stop();
    enemigoIA->cancelarTurno();

    faseCombate=false;
    turnoJugador=false;
    segundosRestantes=15;

    jugador->reiniciarStats();
    jugador->configurarNivel(nivel);

    delete tableroEnemigo;
    tableroEnemigo=new Tablero(Tablero::tamanoPorNivel(nivel),Tablero::maxNavesPorNivel(nivel));
    tableroEnemigo->cargarFlotaNivel(nivel);
    tableroEnemigo->colocarFlotaAleatoria();
    if(nivel>=2){
        tableroEnemigo->colocarMinas(3);
    }

    enemigoIA->configurarNivel(nivel,jugador->getTablero());

    delete escenaJugador;
    delete escenaEnemigo;
    escenaJugador=new EscenaTablero(jugador->getTablero(),false,this);
    escenaEnemigo=new EscenaTablero(tableroEnemigo,true,this);

    vistaJugador->setScene(escenaJugador);
    vistaEnemigo->setScene(escenaEnemigo);

    connect(escenaJugador,&EscenaTablero::celdaClickeada,this,&VentanaJuego::manejarClicJugador);
    connect(escenaJugador,&EscenaTablero::clicDerechoRotar,this,&VentanaJuego::rotarBarco);
    connect(escenaEnemigo,&EscenaTablero::celdaClickeada,this,&VentanaJuego::manejarClicEnemigo);

    btnRotar->setEnabled(true);
    btnAleatorio->setEnabled(true);
    btnRotar->setText("Rotar: Horizontal");

    escenaJugador->actualizarVista();
    escenaEnemigo->actualizarVista();
    actualizarEtiquetas();
}

void VentanaJuego::rotarBarco(){
    if(faseCombate){
        return;
    }
    jugador->rotarOrientacion();
    btnRotar->setText(jugador->siesHorizontal() ? "Rotar: Horizontal":"Rotar: Vertical");
}

void VentanaJuego::despliegueAleatorio(){
    if(faseCombate){
        return;
    }
    jugador->colocarFlotaRandom();
    escenaJugador->actualizarVista();
    comenzarCombate();
}

void VentanaJuego::manejarClicJugador(int fila, int col){
    if(faseCombate || jugador->flotaCompletada()){
        return;
    }

    if(jugador->colocarNave(fila,col)){
        escenaJugador->actualizarVista();

        if(jugador->flotaCompletada()){
            comenzarCombate();
        }else{
            actualizarEtiquetas();
        }
    }
}

void VentanaJuego::comenzarCombate(){
    faseCombate=true;
    turnoJugador=true;
    btnRotar->setEnabled(false);
    btnAleatorio->setEnabled(false);

    segundosRestantes=15;
    timerTurno->start(1000);
    actualizarEtiquetas();
}

void VentanaJuego::iniciarTurnoEnemigo(const QString& mensaje){
    turnoJugador=false;
    timerTurno->stop();
    lblEstado->setText(mensaje);

    if(enemigoIA->isRunning()){
        enemigoIA->wait();
    }
    enemigoIA->start();
}

void VentanaJuego::manejarClicEnemigo(int fila, int col){
    if(!faseCombate || !turnoJugador){
        return;
    }

    int res=jugador->realizarDisparo(tableroEnemigo,fila,col);
    if(res==-1){
        return;
    }

    escenaEnemigo->actualizarVista();
    actualizarEtiquetas();

    if(tableroEnemigo->flotaDestruida()){
        faseCombate=false;
        turnoJugador=false;
        timerTurno->stop();
        lblEstado->setText("¡Victoria!");
        QMessageBox::information(this,"¡Victoria!","Has hundido toda la flota enemiga.");
        return;
    }

    iniciarTurnoEnemigo("Turno del Enemigo...");
}

void VentanaJuego::tickTimer(){
    if(!faseCombate || !turnoJugador){
        return;
    }

    segundosRestantes--;
    lblTiempo->setText(QString("Tiempo: %1s").arg(segundosRestantes));

    if(segundosRestantes<=0){
        iniciarTurnoEnemigo("¡Tiempo agotado! Turno del Enemigo...");
    }
}

void VentanaJuego::procesarDisparoIA(int fila, int col){
    if(!faseCombate){
        return;
    }

    int res=jugador->getTablero()->disparar(fila,col);
    enemigoIA->registrarResultadoDisparo(fila,col,res);

    escenaJugador->actualizarVista();

    if(jugador->perdio()){
        faseCombate=false;
        turnoJugador=false;
        timerTurno->stop();
        lblEstado->setText("Derrota.");
        QMessageBox::critical(this,"Derrota","Tu flota ha sido destruida.");
        return;
    }

    turnoJugador=true;
    segundosRestantes=15;
    timerTurno->start(1000);
    actualizarEtiquetas();
}

void VentanaJuego::actualizarEtiquetas(){
    Naves* naveActual=jugador->getNavePorColocar();
    if(!faseCombate && naveActual!=nullptr){
        lblEstado->setText(QString("Coloca tu %1 (%2 celdas)")
                               .arg(naveActual->getNombre())
                               .arg(naveActual->getTamano()));
    }else if(turnoJugador){
        lblEstado->setText("¡Tu turno! Dispara en el radar enemigo.");
    }

    lblTiempo->setText(QString("Tiempo: %1s").arg(segundosRestantes));
    lblEstadisticas->setText(QString("Disparos: %1 | Aciertos: %2")
                                 .arg(jugador->getDisparosRealizados())
                                 .arg(jugador->getDisparosAcertados()));
}