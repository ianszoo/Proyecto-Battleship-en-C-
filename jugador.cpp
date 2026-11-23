#include "jugador.h"

Jugador::Jugador(string nombre) {
    this->nombre=nombre.empty() ? "Lider":nombre;
    this->tablero=nullptr;
    this->nivelActual=1;
    this->maxNaves=0;
    this->navesColocadas=0;
    this->esHorizontal=true;

    reiniciarStats();
}
Jugador::~Jugador(){
    liberarTablero();
}

void Jugador::liberarTablero(){
    if(tablero!=nullptr){
        delete tablero;
        tablero=nullptr;
    }
}

void Jugador::reiniciarStats(){
    disparosRealizados=0;
    disparosHit=0;
    barcosHundidos=0;
    ptj=0;
    rachaAciertos=0;
    cargasEspeciales=0;
}

void Jugador::configurarNivel(int nivel){
    liberarTablero();

    if(nivel<1 || nivel>3){
        nivel=1;
    }

    nivelActual=nivel;
    navesColocadas=0;
    esHorizontal=true;
    rachaAciertos=0;
    cargasEspeciales=0;

    int tamTablero=0;
    if(nivelActual==1){
        tamTablero=8;
        maxNaves=3;
    }else if(nivelActual==2){
        tamTablero=10;
        maxNaves=5;
    }else if(nivelActual==3){
        tamTablero=14;
        maxNaves=7;
    }

    tablero=new Tablero(tamTablero, maxNaves);
    crearNavesNivel();
}

void Jugador::crearNavesNivel(){
    if(tablero==nullptr){
        return;
    }

    if (nivelActual==1){
        tablero->agregarNave(new Naves("Cruiser", 3));
        tablero->agregarNave(new Naves("Submarino", 3));
        tablero->agregarNave(new Naves("Destroyer", 2));
    }
    else if (nivelActual==2){
        tablero->agregarNave(new Naves("Carrier", 5));
        tablero->agregarNave(new Naves("Battleship", 4));
        tablero->agregarNave(new Naves("Cruiser", 3));
        tablero->agregarNave(new Naves("Submarino", 3));
        tablero->agregarNave(new Naves("Destroyer", 2));
    }
    else if (nivelActual==3){
        tablero->agregarNave(new Naves("Carrier", 5));
        tablero->agregarNave(new Naves("Battleship", 4));
        tablero->agregarNave(new Naves("Cruiser 1", 3));
        tablero->agregarNave(new Naves("Cruiser 2", 3));
        tablero->agregarNave(new Naves("Submarino", 3));
        tablero->agregarNave(new Naves("Destroyer 1", 2));
        tablero->agregarNave(new Naves("Destroyer 2", 2));
    }

}

bool Jugador::agregarNave(Naves* nave){
    if (tablero==nullptr || nave==nullptr){
        return false;
    }

    return tablero->agregarNave(nave);
}

bool Jugador::colocarNave(int fila, int col){
    if(tablero==nullptr || flotaCompletada()){
        return false;
    }

    if(tablero->colocarNave(navesColocadas, fila, col, esHorizontal)){
        navesColocadas++;
        return true;
    }
    return false;
}

void Jugador::colocarFlotaRandom(){
    if(tablero==nullptr){
        return;
    }
    tablero->colocarFlotaAleatoria();
    navesColocadas=maxNaves;
}

void Jugador::rotarOrientacion(){
    esHorizontal=!esHorizontal;
}

void Jugador::reiniciarTablero(){
    if(tablero!=nullptr){
        tablero->inicializar();
    }

    navesColocadas=0;
    esHorizontal=true;
}

bool Jugador::flotaCompletada() const{
    return (navesColocadas>=maxNaves && maxNaves>0);
}

int Jugador::realizarDisparo(Tablero* tableroEnemigo, int fila, int col){
    if(tableroEnemigo==nullptr){
        return -1;
    }

    int resultado=tableroEnemigo->disparar(fila,col);

    if(resultado==-1){
        return -1;
    }

    disparosRealizados++;

    if(resultado==0 || resultado==3){
        rachaAciertos=0;
        if(resultado==3){
            ptj+=25;
        }
    }else if(resultado==1 || resultado==2){
        disparosHit++;
        rachaAciertos++;
        ptj+=100;

        if(resultado==2){
            barcosHundidos++;
            ptj+=250;
        }

        if(nivelActual==3 && rachaAciertos>=3){
            cargasEspeciales++;
            rachaAciertos=0;
        }
    }
    return resultado;
}

int Jugador::usarAtaqueCruz(Tablero* tableroEnemigo, int fila, int col){
    if(tableroEnemigo==nullptr || nivelActual !=3 || cargasEspeciales <=0){
        return -1;
    }

    if(!tableroEnemigo->coordenadaValida(fila,col)){
        return -1;
    }

    cargasEspeciales--;

    int df[5]={0,-1,1,0,0,};
    int dc[5]={0,0,0,-1,1};
    int impactos=0;

    for(int i=0; i<5; i++){
        int nf=fila+df[i];
        int nc=col+dc[i];

        if(tableroEnemigo->coordenadaValida(nf,nc)){
            int res=tableroEnemigo->disparar(nf,nc);
            if(res!=-1){
                disparosRealizados++;
                if(res==1 || res==2){
                    disparosHit++;
                    impactos++;
                    ptj+=100;
                    if(res==2){
                        barcosHundidos++;
                        ptj+=250;
                    }
                }
            }
        }
    }
    return impactos;
}

int Jugador::usarAtaqueSonar(Tablero* tableroEnemigo, int fila, int col){
    if (tableroEnemigo==nullptr || nivelActual!=3 || cargasEspeciales<=0){
        return -1;
    }
    if (!tableroEnemigo->coordenadaValida(fila, col)) {
        return -1;
    }

    cargasEspeciales--;
    tableroEnemigo->revelarZona3x3(fila, col);

    int detectados=0;
    for (int df=-1; df<=1; df++) {
        for (int dc=-1; dc<=1; dc++) {
            int nf=fila+df;
            int nc=col+dc;
            if (tableroEnemigo->coordenadaValida(nf, nc)) {
                if (tableroEnemigo->getEstado(nf, nc)=='B') {
                    detectados++;
                }
            }
        }
    }

    return detectados;
}

bool Jugador::perdio() const{
    if (tablero==nullptr){
        return false;
    }
    return tablero->flotaDestruida();
}

int Jugador::getNavesActivas() const{
    if (tablero==nullptr){
        return 0;
    }
    return tablero->navesActivas();
}

void Jugador::setNombre(const string &nombre){
    if (!nombre.empty()){
        this->nombre=nombre;
    }
}

string Jugador::getNombre() const{
    return nombre;
}

Tablero* Jugador::getTablero() const{
    return tablero;
}

int Jugador::getNivelActual() const{
    return nivelActual;
}

int Jugador::getMaxNaves() const{
    return maxNaves;
}

int Jugador::getNavesColocadas() const{
    return navesColocadas;
}

bool Jugador::siesHorizontal() const{
    return esHorizontal;
}

int Jugador::getDisparosRealizados() const{
    return disparosRealizados;
}

int Jugador::getDisparosAcertados() const{
    return disparosHit;
}

int Jugador::getBarcosHundidos() const{
    return barcosHundidos;
}

int Jugador::getPuntaje() const{
    return ptj;
}

double Jugador::getPorcentajePunteria() const{
    if (disparosRealizados==0){
        return 0.0;
    }
    return (disparosHit*100.0)/disparosRealizados;
}

int Jugador::getRachaHits() const{
    return rachaAciertos;
}

int Jugador::getCargasEspeciales() const{
    return cargasEspeciales;
}

bool Jugador::tieneAtaqueEspecial() const{
    return (nivelActual==3 && cargasEspeciales>0);
}

