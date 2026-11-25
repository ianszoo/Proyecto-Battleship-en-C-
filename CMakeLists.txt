cmake_minimum_required(VERSION 3.16)

project(Battleship LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Busca Qt6 o Qt5 automáticamente (Core para QThread/QTimer, Widgets para QGraphicsView)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

add_executable(Battleship
    main.cpp
    recursos.qrc
    enemigo.h enemigo.cpp
    historial.h historial.cpp
    jugador.h jugador.cpp
    naves.h naves.cpp
    nivel.h nivel.cpp
    tablero.h tablero.cpp
    escenatablero.h escenatablero.cpp
    ventanajuego.h ventanajuego.cpp
)

target_link_libraries(Battleship PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)