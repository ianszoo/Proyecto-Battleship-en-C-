#include "escenatablero.h"
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>

EscenaTablero::EscenaTablero(Tablero* tablero, bool esRadarEnemigo, QObject* parent)
    : QGraphicsScene(parent),
    tablero(tablero),
    esRadarEnemigo(esRadarEnemigo),
    offsetX(49.0),
    offsetY(65.0),
    anchoCelda(41.5),
    altoCelda(42.1),
    navesVisuales(nullptr),
    cantNavesVisuales(0) {

    setSceneRect(0, 0, 500, 500);

    imgTablero = QPixmap(":/new/prefix1/imagenes/Empty Nautical Battleship Grid.png").scaled(500, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    imgBarco1  = QPixmap(":/new/prefix1/imagenes/bote2.png");
    imgBarco2  = QPixmap(":/new/prefix1/imagenes/bote3.png");

    actualizarVista();
}

void EscenaTablero::setNavesVisuales(Naves** naves, int cantidad) {
    navesVisuales = naves;
    cantNavesVisuales = cantidad;
}

void EscenaTablero::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (tablero == nullptr) {
        return;
    }

    if (event->button() == Qt::RightButton && !esRadarEnemigo) {
        emit clicDerechoRotar();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        QPointF pos = event->scenePos();
        int n = tablero->getN();

        double wCelda = (n == 10) ? anchoCelda : (415.0 / n);
        double hCelda = (n == 10) ? altoCelda  : (421.0 / n);

        int col  = static_cast<int>((pos.x() - offsetX) / wCelda);
        int fila = static_cast<int>((pos.y() - offsetY) / hCelda);

        if (tablero->coordenadaValida(fila, col)) {
            emit celdaClickeada(fila, col);
        }
    }

    QGraphicsScene::mousePressEvent(event);
}

void EscenaTablero::actualizarVista() {
    clear();

    // 1. Fondo del tablero
    if (!imgTablero.isNull()) {
        addPixmap(imgTablero);
    }

    if (tablero == nullptr) {
        return;
    }

    int n = tablero->getN();
    double wCelda = (n == 10) ? anchoCelda : (415.0 / n);
    double hCelda = (n == 10) ? altoCelda  : (421.0 / n);

    // 2. Dibujar los barcos propios usando tus imágenes
    if (!esRadarEnemigo && navesVisuales != nullptr) {
        for (int i = 0; i < cantNavesVisuales; ++i) {
            Naves* nave = *(navesVisuales + i);
            if (nave == nullptr || nave->getFila() < 0) {
                continue;
            }

            QPixmap spriteBase = (nave->getTamano() >= 3) ? imgBarco2 : imgBarco1;
            if (spriteBase.isNull()) {
                continue;
            }

            int tam = nave->getTamano();
            QPixmap spriteEscalado = spriteBase.scaled(
                static_cast<int>(tam * wCelda),
                static_cast<int>(hCelda),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );

            QGraphicsPixmapItem* item = addPixmap(spriteEscalado);
            double x = offsetX + nave->getCol() * wCelda;
            double y = offsetY + nave->getFila() * hCelda;

            if (nave->esHorizontal()) {
                item->setPos(x, y);
            } else {
                // Al rotar 90 grados en sentido horario, desplazamos +hCelda en X para alinear
                item->setPos(x + wCelda, y);
                item->setRotation(90);
            }
        }
    }

    if (n != 10 && !imgTablero.isNull()) {
        addRect(offsetX, offsetY, 415.0, 421.0,
                QPen(Qt::NoPen), QBrush(QColor(8, 38, 78)));
    }

    // 3. Dibujar cuadrícula (solo si n != 10) y marcadores de estado ('X' impacto, 'O' agua)
    for (int f = 0; f < n; ++f) {
        for (int c = 0; c < n; ++c) {
            double x = offsetX + c * wCelda;
            double y = offsetY + f * hCelda;
            char est = tablero->getEstado(f, c);

            if (n != 10 || imgTablero.isNull()) {
                addRect(x, y, wCelda, hCelda,
                        QPen(QColor(80, 170, 255, 80)),
                        QBrush(Qt::NoBrush));
            }

            if (!esRadarEnemigo && est == 'B' && imgBarco1.isNull()) {
                addRect(x + 2, y + 2, wCelda - 4, hCelda - 4,
                        QPen(Qt::gray), QBrush(QColor(130, 140, 150, 180)));
            }

            // Impacto ('X') -> Rojo
            if (est == 'X') {
                addEllipse(x + 6, y + 6, wCelda - 12, hCelda - 12,
                           QPen(Qt::yellow, 2), QBrush(QColor(220, 30, 30, 200)));
            }
            // Agua disparada ('O') -> Blanco/Celeste
            else if (est == 'O') {
                addEllipse(x + 10, y + 10, wCelda - 20, hCelda - 20,
                           QPen(Qt::white, 1), QBrush(QColor(200, 235, 255, 160)));
            }
        }
    }
}