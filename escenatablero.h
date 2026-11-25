#ifndef ESCENATABLERO_H
#define ESCENATABLERO_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include "tablero.h"

class EscenaTablero : public QGraphicsScene {
    Q_OBJECT

private:
    Tablero* tablero;
    bool esRadarEnemigo;

    // Coordenadas para alinear la cuadrícula sobre tu imagen tablero.png (500x500)
    double offsetX;
    double offsetY;
    double anchoCelda;
    double altoCelda;

    QPixmap imgTablero;
    QPixmap imgBarco1;
    QPixmap imgBarco2;

    // Arreglo simple de punteros a las naves del jugador para poder dibujar su sprite completo
    Naves** navesVisuales;
    int cantNavesVisuales;

protected:
    // Captura explícita de ratón requerida en el PDF
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

public:
    explicit EscenaTablero(Tablero* tablero, bool esRadarEnemigo, QObject* parent = nullptr);

    void setNavesVisuales(Naves** naves, int cantidad);
    void actualizarVista();

signals:
    void celdaClickeada(int fila, int col);
    void clicDerechoRotar();
};

#endif // ESCENATABLERO_H