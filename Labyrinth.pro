#-------------------------------------------------
#
# Project created by QtCreator 2015-12-20T13:43:00
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -std=c++17 -pipe

QT += core gui

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets printsupport multimedia opengl openglwidgets testlib

CONFIG(debug, debug|release) {
    TARGET = Labyrinthd
} else {
    TARGET = Labyrinth
}

CONFIG += c++17
CONFIG += object_parallel_to_source
CONFIG += object_with_source

RCC_DIR = qrc
RC_FILE += resources.rc

RESOURCES += resources.qrc

TEMPLATE = app
MOC_DIR = moc
VERSION = 1.0

TR_EXCLUDE += D:\Programmes\boost_1_85_0\boost\*

INCLUDEPATH += include \
               D:\Programmes\boost_1_85_0

SOURCES += \
    main.cpp \
    src/Labyrinth2d/Algorithm/CellFusion.cpp \
    src/Labyrinth2d/Algorithm/Kruskal.cpp \
    src/Labyrinth2d/Algorithm/RandomDegenerative.cpp \
    src/Labyrinth2d/Algorithm/TimeoutException.cpp \
    src/Labyrinth2d/Algorithm/WaySearch.cpp \
    src/Labyrinth2d/Mover/QKeyPress.cpp \
    src/Labyrinth2d/Qt/GLLabyrinth.cpp \
    src/Labyrinth2d/Qt/MainWindow.cpp \
    src/Labyrinth2d/Qt/QLabyrinth.cpp \
    src/Labyrinth2d/Renderer/QPainter.cpp \
    src/Labyrinth2d/Renderer/String.cpp \
    src/Labyrinth2d/Solver/FailureException.cpp \
    src/Labyrinth2d/Solver/TimeoutException.cpp \
    src/Labyrinth2d/Solver/WallHand.cpp \
    src/Labyrinth2d/Grid.cpp \
    src/Labyrinth2d/Labyrinth.cpp \
    src/Labyrinth2d/Player.cpp \
    src/Labyrinth3d/Algorithm/CellFusion.cpp \
    src/Labyrinth3d/Algorithm/Kruskal.cpp \
    src/Labyrinth3d/Algorithm/RandomDegenerative.cpp \
    src/Labyrinth3d/Algorithm/TimeoutException.cpp \
    src/Labyrinth3d/Algorithm/WaySearch.cpp \
    src/Labyrinth3d/Mover/QKeyPress.cpp \
    src/Labyrinth3d/Solver/FailureException.cpp \
    src/Labyrinth3d/Solver/TimeoutException.cpp \
    src/Labyrinth3d/Grid.cpp \
    src/Labyrinth3d/Labyrinth.cpp \
    src/Labyrinth3d/Player.cpp

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
    include/Labyrinth2d/Qt/constants.h \
    include/Labyrinth2d/Qt/GLLabyrinth.h \
    include/Labyrinth2d/Qt/MainWindow.h \
    include/Labyrinth2d/Qt/QLabyrinth.h \
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
    include/Labyrinth2d/utility.h \
    include/Labyrinth3d/Algorithm/Algorithm.h \
    include/Labyrinth3d/Algorithm/CellFusion.h \
    include/Labyrinth3d/Algorithm/Fractal.h \
    include/Labyrinth3d/Algorithm/Kruskal.h \
    include/Labyrinth3d/Algorithm/Random.h \
    include/Labyrinth3d/Algorithm/RandomDegenerative.h \
    include/Labyrinth3d/Algorithm/RandomSlicer.h \
    include/Labyrinth3d/Algorithm/Recursive.h \
    include/Labyrinth3d/Algorithm/RecursiveDivision.h \
    include/Labyrinth3d/Algorithm/TimeoutException.h \
    include/Labyrinth3d/Algorithm/WaySearch.h \
    include/Labyrinth3d/Mover/QKeyPress.h \
    include/Labyrinth3d/Renderer/Renderer.h \
    include/Labyrinth3d/Solver/AStar.h \
    include/Labyrinth3d/Solver/Blind.h \
    include/Labyrinth3d/Solver/FailureException.h \
    include/Labyrinth3d/Solver/Solver.h \
    include/Labyrinth3d/Solver/TimeoutException.h \
    include/Labyrinth3d/Grid.h \
    include/Labyrinth3d/Labyrinth.h \
    include/Labyrinth3d/Player.h \
    include/Labyrinth3d/utility.h
