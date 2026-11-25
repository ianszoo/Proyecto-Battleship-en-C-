#include "tablero.h"
#include <cstdlib>
using namespace std;

Tablero::Tablero(int n, int maxNaves){
    this->n=n;
    this->maxNaves=maxNaves;
    cantidadNaves=0;

    celdas=new Casilla*[n];
    for(int i=0; i<n; i++){
        celdas[i]=new Casilla[n];
    }

    flota=new Naves*[maxNaves];
    for(int i=0; i<maxNaves; i++){
        flota[i]=nullptr;
    }

    inicializar();
}

Tablero::~Tablero(){
    for(int i=0; i<n; i++){
        delete[] celdas[i];
    }
    delete[] celdas;
    for(int i=0; i<cantidadNaves; i++){
        delete flota[i];
    }
    delete[] flota;
    celdas=nullptr;
    flota=nullptr;
}

int Tablero::tamanoPorNivel(int nivel){
    if(nivel==1){
        return 8;
    }
    if(nivel==2){
        return 10;
    }
    return 14;
}

int Tablero::maxNavesPorNivel(int nivel){
    if(nivel==1){
        return 3;
    }
    if(nivel==2){
        return 5;
    }
    return 7;
}

void Tablero::inicializar(){
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            celdas[i][j].estado='~';
            celdas[i][j].idBarco=-1;
            celdas[i][j].hayMina=false;
            celdas[i][j].revelada=false;
        }
    }
    for(int i=0; i<cantidadNaves; i++){
        if(flota[i]!=nullptr){
            flota[i]->reiniciar();
        }
    }
}

void Tablero::cargarFlotaNivel(int nivel){
    if(cantidadNaves>0){
        return;
    }

    if(nivel==1){
        agregarNave(new Naves("Cruiser",3));
        agregarNave(new Naves("Submarino",3));
        agregarNave(new Naves("Destroyer",2));
    }else if(nivel==2){
        agregarNave(new Naves("Carrier",5));
        agregarNave(new Naves("Battleship",4));
        agregarNave(new Naves("Cruiser",3));
        agregarNave(new Naves("Submarino",3));
        agregarNave(new Naves("Destroyer",2));
    }else{
        agregarNave(new Naves("Carrier",5));
        agregarNave(new Naves("Battleship",4));
        agregarNave(new Naves("Cruiser 1",3));
        agregarNave(new Naves("Cruiser 2",3));
        agregarNave(new Naves("Submarino",3));
        agregarNave(new Naves("Destroyer 1",2));
        agregarNave(new Naves("Destroyer 2",2));
    }
}

bool Tablero::coordenadaValida(int fila, int col) const{
    return(fila>=0 && fila<n && col>=0 && col<n);
}

int Tablero::getN() const{
    return n;
}

int Tablero::getCantidadNaves() const{
    return cantidadNaves;
}

Naves* Tablero::getNave(int indice) const{
    if(indice<0 || indice>=cantidadNaves){
        return nullptr;
    }
    return *(flota+indice);
}

char Tablero::getEstado(int fila, int col) const{
    if(!coordenadaValida(fila,col)){
        return '?';
    }
    return celdas[fila][col].estado;
}

bool Tablero::casillaHundida(int fila, int col) const{
    if(!coordenadaValida(fila,col)){
        return false;
    }
    int id=celdas[fila][col].idBarco;
    if(id<0 || id>=cantidadNaves){
        return false;
    }
    return(*(flota+id))->estaHundida();
}

bool Tablero::estaRevelada(int fila, int col) const{
    if(!coordenadaValida(fila,col)){
        return false;
    }
    return celdas[fila][col].revelada;
}

bool Tablero::agregarNave(Naves* nave){
    if(nave==nullptr || cantidadNaves>=maxNaves){
        return false;
    }
    *(flota+cantidadNaves)=nave;
    cantidadNaves++;
    return true;
}

bool Tablero::validarNave(int fila, int col, int tam, bool horizontal) const{
    if(!coordenadaValida(fila,col)){
        return false;
    }

    int filaFinal=horizontal ? fila:fila+tam-1;
    int colFinal=horizontal ? col+tam-1:col;

    if(!coordenadaValida(filaFinal,colFinal)){
        return false;
    }

    for(int i=0; i<tam; i++){
        int f=horizontal ? fila:fila+i;
        int c=horizontal ? col+i:col;
        Casilla* casilla=*(celdas+f)+c;

        if(casilla->estado=='B'){
            return false;
        }
    }
    return true;
}

bool Tablero::colocarNave(int idNave, int fila, int col, bool horizontal){
    if(idNave<0 || idNave>=cantidadNaves){
        return false;
    }
    Naves* nave=*(flota+idNave);

    int tam=nave->getTamano();
    if(!validarNave(fila,col,tam,horizontal)){
        return false;
    }
    for(int i=0; i<tam; i++){
        int f=horizontal ? fila:fila+i;
        int c=horizontal ? col+i:col;
        Casilla* casilla=*(celdas+f)+c;
        casilla->estado='B';
        casilla->idBarco=idNave;
    }

    nave->colocar(fila,col,horizontal);
    return true;
}

int Tablero::disparar(int fila, int col){
    if(!coordenadaValida(fila,col)){
        return -1;
    }

    Casilla* casilla=*(celdas+fila)+col;

    if(casilla->estado=='X' || casilla->estado=='O'){
        return -1;
    }

    if(casilla->estado=='~'){
        casilla->estado='O';
        if(casilla->hayMina){
            casilla->hayMina=false;
            revelarZona3x3(fila,col);
            return 3;
        }
        return 0;
    }

    if(casilla->estado=='B'){
        casilla->estado='X';
        int id=casilla->idBarco;
        if(id>=0 && id<cantidadNaves){
            Naves* nave=*(flota+id);
            if(nave!=nullptr){
                nave->recibirImpacto();
                if(nave->estaHundida()){
                    return 2;
                }
            }
        }
        return 1;
    }

    return -1;
}

bool Tablero::flotaDestruida() const{
    if(cantidadNaves==0){
        return false;
    }

    for(int i=0; i<cantidadNaves; i++){
        Naves* nave=*(flota+i);
        if(nave!=nullptr && !nave->estaHundida()){
            return false;
        }
    }
    return true;
}

int Tablero::navesActivas() const{
    int activas=0;
    for(int i=0; i<cantidadNaves; i++){
        Naves* nave=*(flota+i);
        if(nave!=nullptr && !nave->estaHundida()){
            activas++;
        }
    }
    return activas;
}

void Tablero::revelarZona3x3(int fila, int col){
    for(int i=-1; i<=1; i++){
        for(int j=-1; j<=1; j++){
            int f=fila+i;
            int c=col+j;
            if(coordenadaValida(f,c)){
                Casilla* casilla=*(celdas+f)+c;
                casilla->revelada=true;
            }
        }
    }
}

void Tablero::colocarFlotaAleatoria(){
    bool completado=false;
    int reintentos=0;

    while(!completado && reintentos<20){
        inicializar();
        completado=true;

        for(int i=0; i<cantidadNaves; i++){
            Naves* nave=*(flota+i);
            if(nave==nullptr){
                continue;
            }

            bool colocada=false;
            int intentos=0;

            while(!colocada && intentos<250){
                int fila=rand()%n;
                int col=rand()%n;
                bool horizontal=(rand()%2==0);

                if(colocarNave(i,fila,col,horizontal)){
                    colocada=true;
                }
                intentos++;
            }

            if(!colocada){
                completado=false;
                break;
            }
        }
        reintentos++;
    }
}

void Tablero::colocarMinas(int cantidad){
    if(cantidad<=0){
        return;
    }

    int disponibles=0;
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            Casilla* casilla=*(celdas+i)+j;
            if(casilla->estado=='~' && !casilla->hayMina){
                disponibles++;
            }
        }
    }

    int porColocar=(cantidad<disponibles) ? cantidad:disponibles;
    int colocadas=0;

    while(colocadas<porColocar){
        int fila=rand()%n;
        int col=rand()%n;
        Casilla* casilla=*(celdas+fila)+col;

        if(casilla->estado=='~' && !casilla->hayMina){
            casilla->hayMina=true;
            colocadas++;
        }
    }
}