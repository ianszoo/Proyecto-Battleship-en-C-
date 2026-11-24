#include "enemigo.h"
#include <QRandomGenerator>

// =========================================================================
// IMPLEMENTACIÓN DE PILA DINÁMICA PROPIA
// =========================================================================
PilaCoordenadas::PilaCoordenadas() : tope(nullptr) {}

PilaCoordenadas::~PilaCoordenadas() {
    limpiar();
}

void PilaCoordenadas::push(int fila, int col) {
    NodoCoord* nuevo = new NodoCoord(fila, col);
    nuevo->siguiente = tope;
    tope = nuevo;
}

bool PilaCoordenadas::pop(Coordenada& destino) {
    if (estaVacia()) {
        return false;
    }
    NodoCoord* temp = tope;
    destino = temp->dato;
    tope = tope->siguiente;
    delete temp;
    return true;
}

bool PilaCoordenadas::estaVacia() const {
    return tope == nullptr;
}

void PilaCoordenadas::limpiar() {
    Coordenada aux;
    while (pop(aux)) {}
}

// =========================================================================
// IMPLEMENTACIÓN DE ENEMIGO (QThread + IA DE 3 NIVELES)
// =========================================================================
Enemigo::Enemigo(QObject* parent)
    : QThread(parent),
    nivel(1),
    n(8),
    estadoActual(EstadoIA::CAZA),
    tableroObjetivo(nullptr),
    registroRadar(nullptr),
    mapaProbabilidad(nullptr),
    tamaniosBarcos(nullptr),
    barcosVivos(nullptr),
    cantidadBarcosNivel(0) {
    configurarNivel(1, nullptr);
}

Enemigo::~Enemigo() {
    cancelarTurno();
    liberarMemoria();
    tableroObjetivo = nullptr;
}

void Enemigo::reservarMemoria(int tam, int cantBarcos) {
    n = tam;
    cantidadBarcosNivel = cantBarcos;

    // 1. Matrices dinámicas con punteros dobles
    registroRadar = new int*[n];
    mapaProbabilidad = new int*[n];
    for (int i = 0; i < n; ++i) {
        *(registroRadar + i) = new int[n];
        *(mapaProbabilidad + i) = new int[n];
        for (int j = 0; j < n; ++j) {
            *(*(registroRadar + i) + j) = 0;
            *(*(mapaProbabilidad + i) + j) = 0;
        }
    }

    // 2. Arreglos dinámicos para rastrear tamaños de barcos vivos
    tamaniosBarcos = new int[cantidadBarcosNivel];
    barcosVivos = new bool[cantidadBarcosNivel];
    for (int i = 0; i < cantidadBarcosNivel; ++i) {
        *(barcosVivos + i) = true;
    }
}

void Enemigo::liberarMemoria() {
    if (registroRadar != nullptr) {
        for (int i = 0; i < n; ++i) {
            delete[] *(registroRadar + i);
            *(registroRadar + i) = nullptr;
        }
        delete[] registroRadar;
        registroRadar = nullptr;
    }

    if (mapaProbabilidad != nullptr) {
        for (int i = 0; i < n; ++i) {
            delete[] *(mapaProbabilidad + i);
            *(mapaProbabilidad + i) = nullptr;
        }
        delete[] mapaProbabilidad;
        mapaProbabilidad = nullptr;
    }

    if (tamaniosBarcos != nullptr) {
        delete[] tamaniosBarcos;
        tamaniosBarcos = nullptr;
    }

    if (barcosVivos != nullptr) {
        delete[] barcosVivos;
        barcosVivos = nullptr;
    }

    n = 0;
    cantidadBarcosNivel = 0;
}

void Enemigo::cancelarTurno() {
    if (isRunning()) {
        requestInterruption();
        wait();
    }
}

void Enemigo::configurarNivel(int nuevoNivel, Tablero* tableroJugador) {
    cancelarTurno();
    liberarMemoria();

    nivel = nuevoNivel;
    tableroObjetivo = tableroJugador;
    estadoActual = EstadoIA::CAZA;
    pilaObjetivos.limpiar();

    int tam = 8;
    int cant = 3;

    if (nivel == 1) {
        // Nivel 1: Patrulla Costera (8x8, 3 barcos: 3, 3, 2)
        tam = (tableroJugador != nullptr) ? tableroJugador->getN() : 8;
        cant = 3;
        reservarMemoria(tam, cant);
        *(tamaniosBarcos + 0) = 3;
        *(tamaniosBarcos + 1) = 3;
        *(tamaniosBarcos + 2) = 2;
    } else if (nivel == 2) {
        // Nivel 2: Escuadra Táctica (10x10, 5 barcos: 5, 4, 3, 3, 2)
        tam = (tableroJugador != nullptr) ? tableroJugador->getN() : 10;
        cant = 5;
        reservarMemoria(tam, cant);
        *(tamaniosBarcos + 0) = 5;
        *(tamaniosBarcos + 1) = 4;
        *(tamaniosBarcos + 2) = 3;
        *(tamaniosBarcos + 3) = 3;
        *(tamaniosBarcos + 4) = 2;
    } else {
        // Nivel 3: Guerra Total (14x14, 7 barcos: 5, 4, 3, 3, 3, 2, 2)
        tam = (tableroJugador != nullptr) ? tableroJugador->getN() : 14;
        cant = 7;
        reservarMemoria(tam, cant);
        *(tamaniosBarcos + 0) = 5;
        *(tamaniosBarcos + 1) = 4;
        *(tamaniosBarcos + 2) = 3;
        *(tamaniosBarcos + 3) = 3;
        *(tamaniosBarcos + 4) = 3;
        *(tamaniosBarcos + 5) = 2;
        *(tamaniosBarcos + 6) = 2;
    }
}

void Enemigo::reiniciar() {
    cancelarTurno();
    estadoActual = EstadoIA::CAZA;
    pilaObjetivos.limpiar();

    if (registroRadar != nullptr && mapaProbabilidad != nullptr) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                *(*(registroRadar + i) + j) = 0;
                *(*(mapaProbabilidad + i) + j) = 0;
            }
        }
    }

    if (barcosVivos != nullptr) {
        for (int i = 0; i < cantidadBarcosNivel; ++i) {
            *(barcosVivos + i) = true;
        }
    }
}

bool Enemigo::coordenadaValida(int fila, int col) const {
    return registroRadar != nullptr &&
           fila >= 0 && fila < n &&
           col >= 0 && col < n;
}

void Enemigo::apilarVecinosCardinales(int fila, int col) {
    // 4 ejes cardinales: Arriba, Abajo, Izquierda, Derecha
    const int df[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};

    for (int i = 0; i < 4; ++i) {
        int nf = fila + *(df + i);
        int nc = col + *(dc + i);

        if (coordenadaValida(nf, nc) && *(*(registroRadar + nf) + nc) == 0) {
            pilaObjetivos.push(nf, nc);
        }
    }
}

bool Enemigo::hayImpactosPendientes() const {
    if (registroRadar == nullptr) {
        return false;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (*(*(registroRadar + i) + j) == 2) {
                return true;
            }
        }
    }
    return false;
}

void Enemigo::reconstruirPilaDesdeImpactos() {
    pilaObjetivos.limpiar();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (*(*(registroRadar + i) + j) == 2) {
                apilarVecinosCardinales(i, j);
            }
        }
    }
}

int Enemigo::contarConectadosRecursivo(int fila, int col, bool** visitados) {
    if (!coordenadaValida(fila, col) || *(*(visitados + fila) + col)) {
        return 0;
    }
    if (*(*(registroRadar + fila) + col) != 2) {
        return 0;
    }

    *(*(visitados + fila) + col) = true;
    *(*(registroRadar + fila) + col) = 3; // Marca como hundido

    return 1 + contarConectadosRecursivo(fila - 1, col, visitados)
           + contarConectadosRecursivo(fila + 1, col, visitados)
           + contarConectadosRecursivo(fila, col - 1, visitados)
           + contarConectadosRecursivo(fila, col + 1, visitados);
}

void Enemigo::marcarBarcoHundidoEnRadar(int fila, int col) {
    int tamHundido = 0;

    // 1. Aprovechamos Tablero::casillaHundida(i, j) si está conectado
    if (tableroObjetivo != nullptr) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (*(*(registroRadar + i) + j) == 2 && tableroObjetivo->casillaHundida(i, j)) {
                    *(*(registroRadar + i) + j) = 3;
                    tamHundido++;
                }
            }
        }
    }

    // 2. Respaldo recursivo por si tableroObjetivo es nullptr
    if (tamHundido == 0) {
        bool** visitados = new bool*[n];
        for (int i = 0; i < n; ++i) {
            *(visitados + i) = new bool[n];
            for (int j = 0; j < n; ++j) {
                *(*(visitados + i) + j) = false;
            }
        }

        tamHundido = contarConectadosRecursivo(fila, col, visitados);

        for (int i = 0; i < n; ++i) {
            delete[] *(visitados + i);
            *(visitados + i) = nullptr;
        }
        delete[] visitados;
        visitados = nullptr;
    }

    // 3. Dar de baja ese barco en la lista de barcos vivos (para Nivel 3)
    for (int i = 0; i < cantidadBarcosNivel; ++i) {
        if (*(barcosVivos + i) && *(tamaniosBarcos + i) == tamHundido) {
            *(barcosVivos + i) = false;
            break;
        }
    }
}

int Enemigo::obtenerMinimoBarcoVivo() const {
    int minTam = 99;
    for (int i = 0; i < cantidadBarcosNivel; ++i) {
        if (*(barcosVivos + i) && *(tamaniosBarcos + i) < minTam) {
            minTam = *(tamaniosBarcos + i);
        }
    }
    return (minTam == 99) ? 2 : minTam;
}

void Enemigo::registrarResultadoDisparo(int fila, int col, int resultado) {
    if (!coordenadaValida(fila, col)) {
        return;
    }

    if (resultado == 0) {
        // 0 = Agua
        *(*(registroRadar + fila) + col) = 1;
    }
    else if (resultado == 1) {
        // 1 = Impacto
        *(*(registroRadar + fila) + col) = 2;
        if (nivel >= 2) {
            estadoActual = EstadoIA::DESTRUCCION;
            apilarVecinosCardinales(fila, col);
        }
    }
    else if (resultado == 2) {
        // 2 = Hundido
        *(*(registroRadar + fila) + col) = 2;
        marcarBarcoHundidoEnRadar(fila, col);

        if (nivel >= 2) {
            if (hayImpactosPendientes()) {
                estadoActual = EstadoIA::DESTRUCCION;
                reconstruirPilaDesdeImpactos();
            } else {
                pilaObjetivos.limpiar();
                estadoActual = EstadoIA::CAZA;
            }
        }
    }
}

Coordenada Enemigo::calcularDisparoNivel1() {
    Coordenada* disponibles = new Coordenada[n * n];
    int cantidad = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (*(*(registroRadar + i) + j) == 0) {
                *(disponibles + cantidad) = {i, j};
                cantidad++;
            }
        }
    }

    Coordenada elegida = {-1, -1};
    if (cantidad > 0) {
        int idx = QRandomGenerator::global()->bounded(cantidad);
        elegida = *(disponibles + idx);
    }

    delete[] disponibles;
    disponibles = nullptr;
    return elegida;
}

// =========================================================================
// NIVEL 2: Escuadra Táctica (10x10) -> Caza + Destrucción (4 ejes cardinales)
// =========================================================================
Coordenada Enemigo::calcularDisparoNivel2() {
    if (estadoActual == EstadoIA::DESTRUCCION) {
        Coordenada objetivo;
        while (pilaObjetivos.pop(objetivo)) {
            if (coordenadaValida(objetivo.fila, objetivo.col) &&
                *(*(registroRadar + objetivo.fila) + objetivo.col) == 0) {
                return objetivo;
            }
        }
        estadoActual = EstadoIA::CAZA;
    }

    return calcularDisparoNivel1();
}

// =========================================================================
// NIVEL 3: Guerra Total (14x14) -> Mapa de Probabilidad + Paridad
// =========================================================================
Coordenada Enemigo::calcularDisparoNivel3() {
    if (mapaProbabilidad == nullptr || registroRadar == nullptr) {
        return {-1, -1};
    }

    // 1. Limpiar mapa de probabilidad
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            *(*(mapaProbabilidad + i) + j) = 0;
        }
    }

    bool modoImpactoActivo = hayImpactosPendientes();

    // 2. Simular todas las posiciones válidas para cada barco vivo del jugador
    for (int b = 0; b < cantidadBarcosNivel; ++b) {
        if (!*(barcosVivos + b)) {
            continue;
        }
        int tamBarco = *(tamaniosBarcos + b);

        // Barrido HORIZONTAL
        for (int f = 0; f < n; ++f) {
            for (int c = 0; c <= n - tamBarco; ++c) {
                bool valido = true;
                int impactosCubiertos = 0;

                for (int k = 0; k < tamBarco; ++k) {
                    int val = *(*(registroRadar + f) + (c + k));
                    if (val == 1 || val == 3) {
                        valido = false;
                        break;
                    }
                    if (val == 2) {
                        impactosCubiertos++;
                    }
                }

                if (valido) {
                    if (modoImpactoActivo && impactosCubiertos == 0) {
                        continue;
                    }
                    int peso = 1 + (impactosCubiertos * 50);
                    for (int k = 0; k < tamBarco; ++k) {
                        if (*(*(registroRadar + f) + (c + k)) == 0) {
                            *(*(mapaProbabilidad + f) + (c + k)) += peso;
                        }
                    }
                }
            }
        }

        // Barrido VERTICAL
        for (int f = 0; f <= n - tamBarco; ++f) {
            for (int c = 0; c < n; ++c) {
                bool valido = true;
                int impactosCubiertos = 0;

                for (int k = 0; k < tamBarco; ++k) {
                    int val = *(*(registroRadar + (f + k)) + c);
                    if (val == 1 || val == 3) {
                        valido = false;
                        break;
                    }
                    if (val == 2) {
                        impactosCubiertos++;
                    }
                }

                if (valido) {
                    if (modoImpactoActivo && impactosCubiertos == 0) {
                        continue;
                    }
                    int peso = 1 + (impactosCubiertos * 50);
                    for (int k = 0; k < tamBarco; ++k) {
                        if (*(*(registroRadar + (f + k)) + c) == 0) {
                            *(*(mapaProbabilidad + (f + k)) + c) += peso;
                        }
                    }
                }
            }
        }
    }

    int minBarco = obtenerMinimoBarcoVivo();
    int maxProb = -1;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (*(*(registroRadar + i) + j) != 0) {
                continue;
            }
            if (!modoImpactoActivo && ((i + j) % minBarco != 0)) {
                continue;
            }
            if (*(*(mapaProbabilidad + i) + j) > maxProb) {
                maxProb = *(*(mapaProbabilidad + i) + j);
            }
        }
    }

    bool usarParidad = (!modoImpactoActivo && maxProb > 0);
    if (!usarParidad) {
        maxProb = -1;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (*(*(registroRadar + i) + j) == 0 &&
                    *(*(mapaProbabilidad + i) + j) > maxProb) {
                    maxProb = *(*(mapaProbabilidad + i) + j);
                }
            }
        }
    }

    // 4. Desempate aleatorio entre las celdas con mayor probabilidad
    if (maxProb > 0) {
        Coordenada* mejores = new Coordenada[n * n];
        int cantMejores = 0;

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (*(*(registroRadar + i) + j) != 0) {
                    continue;
                }
                if (usarParidad && ((i + j) % minBarco != 0)) {
                    continue;
                }
                if (*(*(mapaProbabilidad + i) + j) == maxProb) {
                    *(mejores + cantMejores) = {i, j};
                    cantMejores++;
                }
            }
        }

        Coordenada elegida = {-1, -1};
        if (cantMejores > 0) {
            int idx = QRandomGenerator::global()->bounded(cantMejores);
            elegida = *(mejores + idx);
        }

        delete[] mejores;
        mejores = nullptr;

        if (elegida.fila != -1) {
            return elegida;
        }
    }

    // Respaldo seguro
    return calcularDisparoNivel2();
}


void Enemigo::run() {
    for (int i = 0; i < 6; ++i) {
        if (isInterruptionRequested()) {
            return;
        }
        msleep(100);
    }

    if (isInterruptionRequested()) {
        return;
    }

    Coordenada tiro = {-1, -1};
    if (nivel == 1) {
        tiro = calcularDisparoNivel1();
    } else if (nivel == 2) {
        tiro = calcularDisparoNivel2();
    } else {
        tiro = calcularDisparoNivel3();
    }

    if (!isInterruptionRequested() && tiro.fila != -1 && tiro.col != -1) {
        emit disparoCalculado(tiro.fila, tiro.col);
    }
}

int Enemigo::getNivel() const {
    return nivel;
}

EstadoIA Enemigo::getEstadoActual() const {
    return estadoActual;
}