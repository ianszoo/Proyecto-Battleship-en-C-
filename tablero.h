#ifndef TABLERO_H
#define TABLERO_H
#include "naves.h"

struct Casilla {
    //estado determina con ~ si es agua, B para un barco, X si hay un impacto con una nave, O si la agua es disparada
    char estado;
    //indice para los diferentes barcos dentro de la flota, es -1 si no hay
    int idBarco;
    //en caso de que en el nivel 2 haya mina
    bool hayMina;
};

class Tablero {

    //variables privadas
private:
    Casilla** celdas;
    int n;
    Naves** flota;
    int cantidadNaves;
    int maxNaves;

    //variables publicas

public:

    //constructor
    Tablero(int n, int maxNaves);
    //destructor
    ~Tablero();

    //ya hechas.
    void inicializar();
    //const al final para que no cambie ningun puntero this
    bool coordenadaValida(int fila, int col) const;
    int getN() const;
    char getEstado(int fila, int col) const;
    bool casillaHundida(int fila, int col) const;
    bool agregarNave(Naves* nave);
    bool validarNave(int fila, int col, int tam, bool horizontal) const;
    bool colocarNave(int idNave, int fila, int col, bool horizontal);
    void colocarFlotaAleatoria();
    void colocarMinas(int cantidad);
    //(acordar que para este metodo vamos a usar 0 para la agua, 1 al impacto, 2 si el se hundio o -1 si es invalido.
    int  disparar(int fila, int col);
    bool flotaDestruida() const;
    int  navesActivas() const;
    void revelarZona3x3(int fila, int col);
};

#endif
