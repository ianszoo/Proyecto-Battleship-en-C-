#ifndef NAVES_H
#define NAVES_H

class Naves
{
private:
    char nombre[20];
    int tamano;
    int impactos;
    bool horizontal;
    int fila, col;
public:
    Naves();
    Naves(const char* nombre, int tamano);
    ~Naves();

    void colocar(int fila, int col, bool horizontal);
    void rotar();
    void recibirImpacto();
    void estaHundida() const;
    void estaColocada() const;
    void reiniciar();
    int getTamano() const;
    int getImpactos() const;
    int getFila() const;
    int getCol() const;
    bool esHorizontal() const;
    const char* getNombre() const;
};

#endif // NAVES_H
