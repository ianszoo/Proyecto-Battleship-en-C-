cmake_minimum_required(VERSION 3.16)

project(Battleship LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Battleship main.cpp
    enemigo.h enemigo.cpp
    tablero.h tablero.cpp
    naves.h naves.cpp
    nivel.h nivel.cpp
    jugador.h jugador.cpp
    historial.h historial.cpp)

include(GNUInstallDirs)
install(TARGETS Battleship
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
