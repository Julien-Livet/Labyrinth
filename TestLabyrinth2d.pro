#-------------------------------------------------
#
# Project created by QtCreator 2015-12-20T13:43:00
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -std=c++17 -pipe

QT += core gui

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG(debug, debug|release) {
    TARGET = TestLabyrinth2dd
} else {
    TARGET = TestLabyrinth2d
}

CONFIG += c++17
CONFIG += object_parallel_to_source
CONFIG += object_with_source

TEMPLATE = app
MOC_DIR = moc
VERSION = 1.0

INCLUDEPATH += include \
               D:\Programmes\boost_1_85_0

SOURCES += \
    testLabyrinth2dMain.cpp \
    src/Labyrinth2d/Algorithm/CellFusion.cpp \
    src/Labyrinth2d/Algorithm/Kruskal.cpp \
    src/Labyrinth2d/Algorithm/RandomDegenerative.cpp \
    src/Labyrinth2d/Algorithm/TimeoutException.cpp \
    src/Labyrinth2d/Algorithm/WaySearch.cpp \
    src/Labyrinth2d/Mover/QKeyPress.cpp \
    src/Labyrinth2d/Renderer/QPainter.cpp \
    src/Labyrinth2d/Renderer/String.cpp \
    src/Labyrinth2d/Solver/FailureException.cpp \
    src/Labyrinth2d/Solver/TimeoutException.cpp \
    src/Labyrinth2d/Solver/WallHand.cpp \
    src/Labyrinth2d/Grid.cpp \
    src/Labyrinth2d/Labyrinth.cpp \
    src/Labyrinth2d/Player.cpp

HEADERS += \
    include/Labyrinth2d/Algorithm/Algorithm.h \
    include/Labyrinth2d/Algorithm/CellFusion.h \
    include/Labyrinth2d/Algorithm/Fractal.h \
    include/Labyrinth2d/Algorithm/Kruskal.h \
    include/Labyrinth2d/Algorithm/Random.h \
    include/Labyrinth2d/Algorithm/RandomDegenerative.h \
    include/Labyrinth2d/Algorithm/Recursive.h \
    include/Labyrinth2d/Algorithm/RecursiveDivision.h \
    include/Labyrinth2d/Algorithm/TimeoutException.h \
    include/Labyrinth2d/Algorithm/WaySearch.h \
    include/Labyrinth2d/Mover/QKeyPress.h \
    include/Labyrinth2d/Renderer/QPainter.h \
    include/Labyrinth2d/Renderer/Renderer.h \
    include/Labyrinth2d/Renderer/String.h \
    include/Labyrinth2d/Solver/AStar.h \
    include/Labyrinth2d/Solver/Blind.h \
    include/Labyrinth2d/Solver/FailureException.h \
    include/Labyrinth2d/Solver/Solver.h \
    include/Labyrinth2d/Solver/TimeoutException.h \
    include/Labyrinth2d/Solver/WallHand.h \
    include/Labyrinth2d/Grid.h \
    include/Labyrinth2d/Labyrinth.h \
    include/Labyrinth2d/Player.h \
    include/Labyrinth2d/utility.h
