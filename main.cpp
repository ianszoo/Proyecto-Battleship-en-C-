
//IAN ANTONIO SUAZO PALAO
//2611345
//SEMANA #6-10
//PROYECTO BATTLESHIP POR IAN Y DAVID SUAZO

#include <QApplication>
#include "ventanajuego.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    VentanaJuego ventana;
    ventana.show();

    return app.exec();
}
