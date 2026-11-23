#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
#include "tablero.h"
#include "Naves.h"

using namespace std;

class Jugador
{
private:
    string nombre;
    Tablero* tablero;
    int nivelActual;
    int maxNaves;
    int navesColocadas;
    bool esHorizontal;

    int disparosRealizados;
    int disparosHit;
    int barcosHundidos;
    int ptj;
    int rachaAciertos;
    int cargasEspeciales;

    void liberarTablero();
    void crearNavesNivel();
public:
    Jugador(string nombre="Lider");
    ~Jugador();

    void configurarNivel(int nivel);
    void reiniciarStats();
    bool agregarNave(Naves *nave);
    bool colocarNave(int fila, int col);
    void colocarFlotaRandom();
    void rotarOrientacion();
    void reiniciarTablero();
    bool flotaCompletada() const;
    int realizarDisparo(Tablero* tableroEnemigo, int fila, int col);
    int usarAtaqueCruz(Tablero* tableroEnemigo, int fila, int col);
    int usarAtaqueSonar(Tablero* tableroEnemigo, int fila, int col);
    bool perdio() const;

    int getNavesActivas() const;
    void setNombre(const string &nombre);
    string getNombre() const;
    Tablero* getTablero() const;
    int getNivelActual() const;
    int getMaxNaves() const;
    int getNavesColocadas() const;
    bool siesHorizontal() const;
    int getDisparosRealizados() const;
    int getDisparosAcertados() const;
    int getBarcosHundidos() const;
    int getPuntaje() const;
    double getPorcentajePunteria() const;
    int getRachaHits() const;
    int getCargasEspeciales() const;
    bool tieneAtaqueEspecial() const;

};

#endif // JUGADOR_H
