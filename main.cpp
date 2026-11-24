#include <QCoreApplication>
#include <QEventLoop>
#include <QObject>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include "jugador.h"
#include "enemigo.h"
#include "tablero.h"

using namespace std;

void limpiarEntrada() {
    cin.clear();
    cin.ignore(10000, '\n');
}

int leerEntero(const string& mensaje, int minVal, int maxVal) {
    int valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor && valor >= minVal && valor <= maxVal) {
            return valor;
        }
        cout << "Entrada invalida. Ingresa un numero entre "
             << minVal << " y " << maxVal << ".\n";
        limpiarEntrada();
    }
}

void imprimirLeyenda() {
    cout << "Leyenda: [~] Agua | [B] Barco | [X] Impacto | [O] Disparo al agua\n";
}

void imprimirTableroJugador(const Tablero* t) {
    int n = t->getN();
    cout << "\n=== MI FLOTA ===\n   ";
    for (int c = 0; c < n; ++c) {
        cout << (c < 10 ? " " : "") << c << " ";
    }
    cout << "\n";
    for (int f = 0; f < n; ++f) {
        cout << (f < 10 ? " " : "") << f << " ";
        for (int c = 0; c < n; ++c) {
            cout << " " << t->getEstado(f, c) << " ";
        }
        cout << "\n";
    }
}

void imprimirRadarEnemigo(const Tablero* t, int revF, int revC) {
    int n = t->getN();
    cout << "\n=== RADAR ENEMIGO ===\n   ";
    for (int c = 0; c < n; ++c) {
        cout << (c < 10 ? " " : "") << c << " ";
    }
    cout << "\n";
    for (int f = 0; f < n; ++f) {
        cout << (f < 10 ? " " : "") << f << " ";
        for (int c = 0; c < n; ++c) {
            char estado = t->getEstado(f, c);
            bool enZonaRevelada = (revF != -1 && std::abs(f - revF) <= 1 && std::abs(c - revC) <= 1);

            // Ocultar los barcos enemigos no impactados salvo en zona revelada (Mina/Sonar)
            if (estado == 'B' && !enZonaRevelada) {
                estado = '~';
            }
            cout << " " << estado << " ";
        }
        cout << "\n";
    }
}

Tablero* crearTableroEnemigo(int nivel) {
    Tablero* t = nullptr;
    if (nivel == 1) {
        t = new Tablero(8, 3);
        t->agregarNave(new Naves("Cruiser", 3));
        t->agregarNave(new Naves("Submarino", 3));
        t->agregarNave(new Naves("Destroyer", 2));
        t->colocarFlotaAleatoria();
    } else if (nivel == 2) {
        t = new Tablero(10, 5);
        t->agregarNave(new Naves("Carrier", 5));
        t->agregarNave(new Naves("Battleship", 4));
        t->agregarNave(new Naves("Cruiser", 3));
        t->agregarNave(new Naves("Submarino", 3));
        t->agregarNave(new Naves("Destroyer", 2));
        t->colocarFlotaAleatoria();
        t->colocarMinas(4);
    } else {
        t = new Tablero(14, 7);
        t->agregarNave(new Naves("Carrier", 5));
        t->agregarNave(new Naves("Battleship", 4));
        t->agregarNave(new Naves("Cruiser 1", 3));
        t->agregarNave(new Naves("Cruiser 2", 3));
        t->agregarNave(new Naves("Submarino", 3));
        t->agregarNave(new Naves("Destroyer 1", 2));
        t->agregarNave(new Naves("Destroyer 2", 2));
        t->colocarFlotaAleatoria();
    }
    return t;
}

string nombreBarcoPorIndice(int nivel, int idx) {
    if (nivel == 1) {
        const string n1[3] = {"Crucero (3)", "Submarino (3)", "Destructor (2)"};
        return n1[idx];
    } else if (nivel == 2) {
        const string n2[5] = {"Portaaviones (5)", "Acorazado (4)", "Crucero (3)", "Submarino (3)", "Destructor (2)"};
        return n2[idx];
    } else {
        const string n3[7] = {"Portaaviones (5)", "Acorazado (4)", "Crucero 1 (3)", "Crucero 2 (3)", "Submarino (3)", "Destructor 1 (2)", "Destructor 2 (2)"};
        return n3[idx];
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    cout << "========================================\n";
    cout << "             BATALLA NAVAL              \n";
    cout << "========================================\n";

    string nombre;
    cout << "Nombre del comandante: ";
    getline(cin, nombre);
    if (nombre.empty()) {
        nombre = "Comandante";
    }

    cout << "\nSelecciona el nivel:\n";
    cout << "  1. Patrulla Costera (8x8 - 3 barcos)\n";
    cout << "  2. Escuadra Tactica (10x10 - 5 barcos + minas)\n";
    cout << "  3. Guerra Total     (14x14 - 7 barcos + ataque especial)\n";
    int nivel = leerEntero("Nivel (1-3): ", 1, 3);

    Jugador jugador(nombre);
    jugador.configurarNivel(nivel);

    int modoDespliegue = leerEntero("\nDespliegue de flota (1 = Manual, 2 = Aleatorio): ", 1, 2);

    if (modoDespliegue == 1) {
        int n = jugador.getTablero()->getN();
        while (!jugador.flotaCompletada()) {
            imprimirTableroJugador(jugador.getTablero());
            imprimirLeyenda();

            int idx = jugador.getNavesColocadas();
            cout << "\nColocando: " << nombreBarcoPorIndice(nivel, idx)
                 << " | Orientacion actual: "
                 << (jugador.siesHorizontal() ? "Horizontal" : "Vertical") << "\n";

            int opc = leerEntero("1 = Colocar aqui | 2 = Rotar orientacion: ", 1, 2);
            if (opc == 2) {
                jugador.rotarOrientacion();
                continue;
            }

            int f = leerEntero("Fila (0-" + to_string(n - 1) + "): ", 0, n - 1);
            int c = leerEntero("Columna (0-" + to_string(n - 1) + "): ", 0, n - 1);

            if (!jugador.colocarNave(f, c)) {
                cout << "Posicion invalida (se sale del tablero o choca con otro barco).\n";
            }
        }
    } else {
        jugador.colocarFlotaRandom();
    }

    Tablero* tableroEnemigo = crearTableroEnemigo(nivel);
    Enemigo enemigo;
    enemigo.configurarNivel(nivel, jugador.getTablero());

    // Conexión del hilo secundario de la IA al bucle de eventos de turno
    int iaFila = -1, iaCol = -1;
    QEventLoop loopTurnoIA;
    QObject::connect(&enemigo, &Enemigo::disparoCalculado,
                     &app, [&](int f, int c) {
                         iaFila = f;
                         iaCol = c;
                         loopTurnoIA.quit();
                     });

    int tam = jugador.getTablero()->getN();
    int revF = -1, revC = -1; // Zona temporal 3x3 revelada por Mina o Sonar

    // Bucle principal del juego
    while (!jugador.perdio() && !tableroEnemigo->flotaDestruida()) {
        imprimirTableroJugador(jugador.getTablero());
        imprimirRadarEnemigo(tableroEnemigo, revF, revC);
        imprimirLeyenda();

        cout << "\n--- TU TURNO (" << jugador.getNombre() << ") ---\n";
        cout << "Tus barcos activos: " << jugador.getNavesActivas()
             << " | Enemigos activos: " << tableroEnemigo->navesActivas()
             << " | Puntaje: " << jugador.getPuntaje() << "\n";

        // Limpiar revelación temporal anterior después de mostrarla un turno
        revF = -1;
        revC = -1;

        int tipoAtaque = 1;
        if (jugador.tieneAtaqueEspecial()) {
            cout << "\n[ATAQUE ESPECIAL DISPONIBLE - Cargas: " << jugador.getCargasEspeciales() << "]\n";
            cout << "1. Disparo normal | 2. Ataque en Cruz | 3. Sonar (3x3)\n";
            tipoAtaque = leerEntero("Elige accion (1-3): ", 1, 3);
        }

        // Pedir coordenadas válidas (o -1 para salir)
        bool tiroValido = false;
        while (!tiroValido) {
            int f = leerEntero("Disparar a Fila (0-" + to_string(tam - 1) + ", o -1 para salir): ", -1, tam - 1);
            if (f == -1) {
                cout << "Partida finalizada por el jugador.\n";
                delete tableroEnemigo;
                return 0;
            }
            int c = leerEntero("Disparar a Columna (0-" + to_string(tam - 1) + "): ", 0, tam - 1);

            if (tipoAtaque == 2) {
                int impactos = jugador.usarAtaqueCruz(tableroEnemigo, f, c);
                cout << ">> Ataque en Cruz ejecutado. Impactos logrados: " << impactos << "\n";
                tiroValido = true;
            } else if (tipoAtaque == 3) {
                int detectados = jugador.usarAtaqueSonar(tableroEnemigo, f, c);
                revF = f;
                revC = c;
                cout << ">> Sonar activado en (" << f << ", " << c << "). Barcos detectados en 3x3: "
                     << detectados << "\n";
                tiroValido = true;
            } else {
                int res = jugador.realizarDisparo(tableroEnemigo, f, c);
                if (res == -1) {
                    cout << "Ya disparaste a esa casilla o es invalida. Elige otra.\n";
                } else {
                    tiroValido = true;
                    if (res == 0) {
                        cout << ">> Agua!\n";
                    } else if (res == 1) {
                        cout << ">> IMPACTO en nave enemiga!\n";
                    } else if (res == 2) {
                        cout << ">> BARCO ENEMIGO HUNDIDO!\n";
                    } else if (res == 3) {
                        revF = f;
                        revC = c;
                        cout << ">> MINA DETONADA! Zona 3x3 revelada temporalmente.\n";
                    }
                }
            }
        }

        if (tableroEnemigo->flotaDestruida()) {
            break;
        }

        // Turno de la IA en QThread
        cout << "\nTurno del enemigo...\n";
        iaFila = -1;
        iaCol = -1;
        enemigo.start();
        if (iaFila == -1) {
            loopTurnoIA.exec();
        }
        enemigo.wait();

        int resIA = jugador.getTablero()->disparar(iaFila, iaCol);
        enemigo.registrarResultadoDisparo(iaFila, iaCol, resIA);

        cout << ">> El enemigo disparo a (" << iaFila << ", " << iaCol << "): ";
        if (resIA == 0) {
            cout << "Agua!\n";
        } else if (resIA == 1) {
            cout << "IMPACTO en tu flota!\n";
        } else if (resIA == 2) {
            cout << "TE HUNDIERON UN BARCO!\n";
        }
    }

    cout << "\n========================================\n";
    if (tableroEnemigo->flotaDestruida()) {
        cout << "   VICTORIA! Hundiste toda la flota enemiga\n";
    } else {
        cout << "   DERROTA! Tu flota ha sido destruida\n";
    }
    cout << "   Disparos: " << jugador.getDisparosRealizados()
         << " | Punteria: " << jugador.getPorcentajePunteria() << "%"
         << " | Puntaje: " << jugador.getPuntaje() << "\n";
    cout << "========================================\n";

    delete tableroEnemigo;
    tableroEnemigo = nullptr;
    return 0;
}