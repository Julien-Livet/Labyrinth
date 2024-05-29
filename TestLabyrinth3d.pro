#-------------------------------------------------
#
# Project created by QtCreator 2015-12-20T13:43:00
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -std=c++17 -pipe

QT += core gui

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets opengl openglwidgets

CONFIG(debug, debug|release) {
    TARGET = TestLabyrinth3dd
} else {
    TARGET = TestLabyrinth3d
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
    testLabyrinth3dMain.cpp \
    src/Labyrinth3d/Algorithm/CellFusion.cpp \
    src/Labyrinth3d/Algorithm/Kruskal.cpp \
    src/Labyrinth3d/Algorithm/RandomDegenerative.cpp \
    src/Labyrinth3d/Algorithm/TimeoutException.cpp \
    src/Labyrinth3d/Algorithm/WaySearch.cpp \
    src/Labyrinth3d/Qt/GlWidget.cpp \
    src/Labyrinth3d/Solver/FailureException.cpp \
    src/Labyrinth3d/Solver/TimeoutException.cpp \
    src/Labyrinth3d/Labyrinth.cpp \
    src/Labyrinth3d/Player.cpp

HEADERS += \
    include/Labyrinth3d/Algorithm/Algorithm.h \
    include/Labyrinth3d/Algorithm/CellFusion.h \
    include/Labyrinth3d/Algorithm/Fractal.h \
    include/Labyrinth3d/Algorithm/Kruskal.h \
    include/Labyrinth3d/Algorithm/Random.h \
    include/Labyrinth3d/Algorithm/RandomDegenerative.h \
    include/Labyrinth3d/Algorithm/Recursive.h \
    include/Labyrinth3d/Algorithm/RecursiveDivision.h \
    include/Labyrinth3d/Algorithm/TimeoutException.h \
    include/Labyrinth3d/Algorithm/WaySearch.h \
    include/Labyrinth3d/Qt/GlWidget.h \
    include/Labyrinth3d/Renderer/Renderer.h \
    include/Labyrinth3d/Solver/AStar.h \
    include/Labyrinth3d/Solver/Blind.h \
    include/Labyrinth3d/Solver/FailureException.h \
    include/Labyrinth3d/Solver/Solver.h \
    include/Labyrinth3d/Solver/TimeoutException.h \
    include/Labyrinth3d/Grid.h \
    include/Labyrinth3d/Grid.tcc \
    include/Labyrinth3d/Labyrinth.h \
    include/Labyrinth3d/Player.h \
    include/Labyrinth3d/utility.h
