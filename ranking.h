#ifndef RANKING_H
#define RANKING_H
#include <string>
#include "Jugador.h"

struct RegistroRanking{
    string nombre;
    int puntaje;
    double punteria;
    int barcosHundidos;
    int nivel;
};

class Ranking
{
private:
    RegistroRanking* registros;
    int cantidad;
    int capacidadMax;
    std::string rutaArchivo;
public:
    Ranking(int capacidadMax=10, string rutaArchivo="ranking.txt");
    ~Ranking();

    //por burbuja
    void ordenar();
    bool entrarAlRanking(int puntaje);
    bool agregarRegistro(const Jugador &Jugador);
    bool agregarRegistro(string nombre, int puntaje, double punteria, int barcosHundidos, int nivel);
    void limpiar();

    //parte de archivos
    bool guardar() const;
    bool cargar();

    //getters
    int getCantidad() const;
    RegistroRanking getRegistro(int indice) const;
    int getMejorPuntaje() const;
    int buscarPosicion(const string &nombre) const;
};

#endif // RANKING_H
