#include "naves.h"
#include <cstring>

Naves::Naves() {
    std::strcpy(nombre, "Sin nombre");
    tamano = 0;
    impactos = 0;
    horizontal = true;
    fila = -1;
    col = -1;
}

Naves::Naves(const char* nombre, int tamano) {
    if (nombre != nullptr){
        std::strncpy(this->nombre, nombre, 19);
        this->nombre[19]='\0';
    } else {
        std::strcpy(this->nombre, "Sin nombre");
    }
    this->tamano = (tamano > 0) ? tamano : 1;
    this->impactos = 0;
    this->horizontal = true;
    this->fila = -1;
    this->col = -1;
}

Naves::~Naves() {

}

void Naves::colocar(int fila, int col, bool horizontal) {
    this->fila = fila;
    this->col = col;
    this->horizontal = horizontal;
}

void Naves::rotar() {
    horizontal = !horizontal;
}

void Naves::recibirImpacto() {
    if (impactos < tamano) {
        impactos++;
    }
}

bool Naves::estaHundida() const {
    return (tamano > 0 && impactos >= tamano);
}

bool Naves::estaColocada() const {
    return (fila != -1 && col != -1);
}

void Naves::reiniciar() {
    impactos = 0;
    fila = -1;
    col = -1;
    horizontal = true;
}

int Naves::getTamano() const{
    return tamano;
}

int Naves::getImpactos() const{
    return impactos;
}

int Naves::getFila() const{
    return fila;
}

int Naves::getCol() const{
    return col;
}

bool Naves::esHorizontal() const{
    return horizontal;
}

const char* Naves::getNombre() const{
    return nombre;
}