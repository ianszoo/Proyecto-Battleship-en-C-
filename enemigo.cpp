#include "enemigo.h"
#include <QRandomGenerator>

PilaCoordenadas::PilaCoordenadas() : tope(nullptr){}

PilaCoordenadas::~PilaCoordenadas(){
    limpiar();
}

void PilaCoordenadas::push(int fila, int col){
    NodoCoord* nuevo=new NodoCoord(fila,col);
    nuevo->siguiente=tope;
    tope=nuevo;
}

bool PilaCoordenadas::pop(Coordenada& destino){
    if(estaVacia()){
        return false;
    }
    NodoCoord* temp=tope;
    destino=temp->dato;
    tope=tope->siguiente;
    delete temp;
    return true;
}

bool PilaCoordenadas::estaVacia() const{
    return tope==nullptr;
}

void PilaCoordenadas::limpiar(){
    Coordenada aux;
    while(pop(aux)){}
}

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
    cantidadBarcosNivel(0){
    configurarNivel(1,nullptr);
}

Enemigo::~Enemigo(){
    cancelarTurno();
    liberarMemoria();
    tableroObjetivo=nullptr;
}

void Enemigo::reservarMemoria(int tam, int cantBarcos){
    n=tam;
    cantidadBarcosNivel=cantBarcos;

    registroRadar=new int*[n];
    mapaProbabilidad=new int*[n];
    for(int i=0; i<n; i++){
        *(registroRadar+i)=new int[n];
        *(mapaProbabilidad+i)=new int[n];
        for(int j=0; j<n; j++){
            *(*(registroRadar+i)+j)=0;
            *(*(mapaProbabilidad+i)+j)=0;
        }
    }

    tamaniosBarcos=new int[cantidadBarcosNivel];
    barcosVivos=new bool[cantidadBarcosNivel];
    for(int i=0; i<cantidadBarcosNivel; i++){
        *(barcosVivos+i)=true;
    }
}

void Enemigo::liberarMemoria(){
    if(registroRadar!=nullptr){
        for(int i=0; i<n; i++){
            delete[] *(registroRadar+i);
            *(registroRadar+i)=nullptr;
        }
        delete[] registroRadar;
        registroRadar=nullptr;
    }

    if(mapaProbabilidad!=nullptr){
        for(int i=0; i<n; i++){
            delete[] *(mapaProbabilidad+i);
            *(mapaProbabilidad+i)=nullptr;
        }
        delete[] mapaProbabilidad;
        mapaProbabilidad=nullptr;
    }

    if(tamaniosBarcos!=nullptr){
        delete[] tamaniosBarcos;
        tamaniosBarcos=nullptr;
    }

    if(barcosVivos!=nullptr){
        delete[] barcosVivos;
        barcosVivos=nullptr;
    }

    n=0;
    cantidadBarcosNivel=0;
}

void Enemigo::cancelarTurno(){
    if(isRunning()){
        requestInterruption();
        wait();
    }
}

void Enemigo::configurarNivel(int nuevoNivel, Tablero* tableroJugador){
    cancelarTurno();
    liberarMemoria();

    nivel=nuevoNivel;
    tableroObjetivo=tableroJugador;
    estadoActual=EstadoIA::CAZA;
    pilaObjetivos.limpiar();

    if(tableroJugador!=nullptr && tableroJugador->getCantidadNaves()>0){
        int tam=tableroJugador->getN();
        int cant=tableroJugador->getCantidadNaves();
        reservarMemoria(tam,cant);
        for(int i=0; i<cant; i++){
            Naves* nave=tableroJugador->getNave(i);
            *(tamaniosBarcos+i)=(nave!=nullptr) ? nave->getTamano():2;
        }
    }else{
        int tam=Tablero::tamanoPorNivel(nivel);
        int cant=Tablero::maxNavesPorNivel(nivel);
        reservarMemoria(tam,cant);
        for(int i=0; i<cant; i++){
            *(tamaniosBarcos+i)=3;
        }
    }
}

void Enemigo::reiniciar(){
    cancelarTurno();
    estadoActual=EstadoIA::CAZA;
    pilaObjetivos.limpiar();

    if(registroRadar!=nullptr && mapaProbabilidad!=nullptr){
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                *(*(registroRadar+i)+j)=0;
                *(*(mapaProbabilidad+i)+j)=0;
            }
        }
    }

    if(barcosVivos!=nullptr){
        for(int i=0; i<cantidadBarcosNivel; i++){
            *(barcosVivos+i)=true;
        }
    }
}

bool Enemigo::coordenadaValida(int fila, int col) const{
    return registroRadar!=nullptr && fila>=0 && fila<n && col>=0 && col<n;
}

void Enemigo::apilarVecinosCardinales(int fila, int col){
    const int df[4]={-1,1,0,0};
    const int dc[4]={0,0,-1,1};

    for(int i=0; i<4; i++){
        int nf=fila+*(df+i);
        int nc=col+*(dc+i);

        if(coordenadaValida(nf,nc) && *(*(registroRadar+nf)+nc)==0){
            pilaObjetivos.push(nf,nc);
        }
    }
}

bool Enemigo::hayImpactosPendientes() const{
    if(registroRadar==nullptr){
        return false;
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(*(*(registroRadar+i)+j)==2){
                return true;
            }
        }
    }
    return false;
}

void Enemigo::reconstruirPilaDesdeImpactos(){
    pilaObjetivos.limpiar();
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(*(*(registroRadar+i)+j)==2){
                apilarVecinosCardinales(i,j);
            }
        }
    }
}

int Enemigo::contarConectadosRecursivo(int fila, int col, bool** visitados){
    if(!coordenadaValida(fila,col) || *(*(visitados+fila)+col)){
        return 0;
    }
    if(*(*(registroRadar+fila)+col)!=2){
        return 0;
    }

    *(*(visitados+fila)+col)=true;
    *(*(registroRadar+fila)+col)=3;

    return 1+contarConectadosRecursivo(fila-1,col,visitados)
           +contarConectadosRecursivo(fila+1,col,visitados)
           +contarConectadosRecursivo(fila,col-1,visitados)
           +contarConectadosRecursivo(fila,col+1,visitados);
}

void Enemigo::marcarBarcoHundidoEnRadar(int fila, int col){
    int tamHundido=0;

    if(tableroObjetivo!=nullptr){
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                if(*(*(registroRadar+i)+j)==2 && tableroObjetivo->casillaHundida(i,j)){
                    *(*(registroRadar+i)+j)=3;
                    tamHundido++;
                }
            }
        }
    }

    if(tamHundido==0){
        bool** visitados=new bool*[n];
        for(int i=0; i<n; i++){
            *(visitados+i)=new bool[n];
            for(int j=0; j<n; j++){
                *(*(visitados+i)+j)=false;
            }
        }

        tamHundido=contarConectadosRecursivo(fila,col,visitados);

        for(int i=0; i<n; i++){
            delete[] *(visitados+i);
            *(visitados+i)=nullptr;
        }
        delete[] visitados;
        visitados=nullptr;
    }

    for(int i=0; i<cantidadBarcosNivel; i++){
        if(*(barcosVivos+i) && *(tamaniosBarcos+i)==tamHundido){
            *(barcosVivos+i)=false;
            break;
        }
    }
}

int Enemigo::obtenerMinimoBarcoVivo() const{
    int minTam=99;
    for(int i=0; i<cantidadBarcosNivel; i++){
        if(*(barcosVivos+i) && *(tamaniosBarcos+i)<minTam){
            minTam=*(tamaniosBarcos+i);
        }
    }
    return (minTam==99) ? 2:minTam;
}

void Enemigo::registrarResultadoDisparo(int fila, int col, int resultado){
    if(!coordenadaValida(fila,col)){
        return;
    }

    if(resultado==0 || resultado==3){
        *(*(registroRadar+fila)+col)=1;
    }else if(resultado==1){
        *(*(registroRadar+fila)+col)=2;
        if(nivel>=2){
            estadoActual=EstadoIA::DESTRUCCION;
            apilarVecinosCardinales(fila,col);
        }
    }else if(resultado==2){
        *(*(registroRadar+fila)+col)=2;
        marcarBarcoHundidoEnRadar(fila,col);

        if(nivel>=2){
            if(hayImpactosPendientes()){
                estadoActual=EstadoIA::DESTRUCCION;
                reconstruirPilaDesdeImpactos();
            }else{
                pilaObjetivos.limpiar();
                estadoActual=EstadoIA::CAZA;
            }
        }
    }
}

Coordenada Enemigo::calcularDisparoNivel1(){
    Coordenada* disponibles=new Coordenada[n*n];
    int cantidad=0;

    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(*(*(registroRadar+i)+j)==0){
                *(disponibles+cantidad)={i,j};
                cantidad++;
            }
        }
    }

    Coordenada elegida={-1,-1};
    if(cantidad>0){
        int idx=QRandomGenerator::global()->bounded(cantidad);
        elegida=*(disponibles+idx);
    }

    delete[] disponibles;
    disponibles=nullptr;
    return elegida;
}

Coordenada Enemigo::calcularDisparoNivel2(){
    if(estadoActual==EstadoIA::DESTRUCCION){
        Coordenada objetivo;
        while(pilaObjetivos.pop(objetivo)){
            if(coordenadaValida(objetivo.fila,objetivo.col) && *(*(registroRadar+objetivo.fila)+objetivo.col)==0){
                return objetivo;
            }
        }
        estadoActual=EstadoIA::CAZA;
    }

    return calcularDisparoNivel1();
}

void Enemigo::evaluarColocaciones(int tamBarco, bool horizontal, bool modoImpactoActivo){
    int maxF=horizontal ? n:(n-tamBarco+1);
    int maxC=horizontal ? (n-tamBarco+1):n;
    int df=horizontal ? 0:1;
    int dc=horizontal ? 1:0;

    for(int f=0; f<maxF; f++){
        for(int c=0; c<maxC; c++){
            bool valido=true;
            int impactosCubiertos=0;

            for(int k=0; k<tamBarco; k++){
                int val=*(*(registroRadar+(f+k*df))+(c+k*dc));
                if(val==1 || val==3){
                    valido=false;
                    break;
                }
                if(val==2){
                    impactosCubiertos++;
                }
            }

            if(valido && (!modoImpactoActivo || impactosCubiertos>0)){
                int peso=1+(impactosCubiertos*50);
                for(int k=0; k<tamBarco; k++){
                    int nf=f+k*df;
                    int nc=c+k*dc;
                    if(*(*(registroRadar+nf)+nc)==0){
                        *(*(mapaProbabilidad+nf)+nc)+=peso;
                    }
                }
            }
        }
    }
}

Coordenada Enemigo::calcularDisparoNivel3(){
    if(mapaProbabilidad==nullptr || registroRadar==nullptr){
        return {-1,-1};
    }

    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            *(*(mapaProbabilidad+i)+j)=0;
        }
    }

    bool modoImpactoActivo=hayImpactosPendientes();

    for(int b=0; b<cantidadBarcosNivel; b++){
        if(!*(barcosVivos+b)){
            continue;
        }
        int tamBarco=*(tamaniosBarcos+b);
        evaluarColocaciones(tamBarco,true,modoImpactoActivo);
        evaluarColocaciones(tamBarco,false,modoImpactoActivo);
    }

    int minBarco=obtenerMinimoBarcoVivo();
    int maxProb=-1;

    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(*(*(registroRadar+i)+j)!=0){
                continue;
            }
            if(!modoImpactoActivo && ((i+j)%minBarco!=0)){
                continue;
            }
            if(*(*(mapaProbabilidad+i)+j)>maxProb){
                maxProb=*(*(mapaProbabilidad+i)+j);
            }
        }
    }

    bool usarParidad=(!modoImpactoActivo && maxProb>0);
    if(!usarParidad){
        maxProb=-1;
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                if(*(*(registroRadar+i)+j)==0 && *(*(mapaProbabilidad+i)+j)>maxProb){
                    maxProb=*(*(mapaProbabilidad+i)+j);
                }
            }
        }
    }

    if(maxProb>0){
        Coordenada* mejores=new Coordenada[n*n];
        int cantMejores=0;

        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                if(*(*(registroRadar+i)+j)!=0){
                    continue;
                }
                if(usarParidad && ((i+j)%minBarco!=0)){
                    continue;
                }
                if(*(*(mapaProbabilidad+i)+j)==maxProb){
                    *(mejores+cantMejores)={i,j};
                    cantMejores++;
                }
            }
        }

        Coordenada elegida={-1,-1};
        if(cantMejores>0){
            int idx=QRandomGenerator::global()->bounded(cantMejores);
            elegida=*(mejores+idx);
        }

        delete[] mejores;
        mejores=nullptr;

        if(elegida.fila!=-1){
            return elegida;
        }
    }

    return calcularDisparoNivel2();
}

void Enemigo::run(){
    for(int i=0; i<6; i++){
        if(isInterruptionRequested()){
            return;
        }
        msleep(100);
    }

    if(isInterruptionRequested()){
        return;
    }

    Coordenada tiro={-1,-1};
    if(nivel==1){
        tiro=calcularDisparoNivel1();
    }else if(nivel==2){
        tiro=calcularDisparoNivel2();
    }else{
        tiro=calcularDisparoNivel3();
    }

    if(!isInterruptionRequested() && tiro.fila!=-1 && tiro.col!=-1){
        emit disparoCalculado(tiro.fila,tiro.col);
    }
}

int Enemigo::getNivel() const{
    return nivel;
}

EstadoIA Enemigo::getEstadoActual() const{
    return estadoActual;
}