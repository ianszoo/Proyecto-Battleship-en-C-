#ifndef ESCENATABLERO_H
#define ESCENATABLERO_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include "tablero.h"

class EscenaTablero : public QGraphicsScene{
    Q_OBJECT

private:
    Tablero* tablero;
    bool esRadarEnemigo;

    double offsetX;
    double offsetY;
    double anchoCelda;
    double altoCelda;

    QPixmap imgTablero;
    QPixmap imgBarco1;
    QPixmap imgBarco2;

    void obtenerDimensionesCelda(double& wCelda, double& hCelda) const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

public:
    explicit EscenaTablero(Tablero* tablero, bool esRadarEnemigo, QObject* parent=nullptr);
    void actualizarVista();

signals:
    void celdaClickeada(int fila, int col);
    void clicDerechoRotar();
};

#endif