#ifndef TABLERO_H
#define TABLERO_H
#include "naves.h"

struct Casilla{
    char estado;
    int idBarco;
    bool hayMina;
    bool revelada;
};

class Tablero{
private:
    Casilla** celdas;
    int n;
    Naves** flota;
    int cantidadNaves;
    int maxNaves;

public:
    Tablero(int n, int maxNaves);
    ~Tablero();

    static int tamanoPorNivel(int nivel);
    static int maxNavesPorNivel(int nivel);

    void inicializar();
    void cargarFlotaNivel(int nivel);
    bool coordenadaValida(int fila, int col) const;
    int getN() const;
    int getCantidadNaves() const;
    Naves* getNave(int indice) const;
    char getEstado(int fila, int col) const;
    bool casillaHundida(int fila, int col) const;
    bool estaRevelada(int fila, int col) const;
    bool agregarNave(Naves* nave);
    bool validarNave(int fila, int col, int tam, bool horizontal) const;
    bool colocarNave(int idNave, int fila, int col, bool horizontal);
    void colocarFlotaAleatoria();
    void colocarMinas(int cantidad);
    int disparar(int fila, int col);
    bool flotaDestruida() const;
    int navesActivas() const;
    void revelarZona3x3(int fila, int col);
};

#endif