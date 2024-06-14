QT += testlib

CONFIG += object_parallel_to_source

INCLUDEPATH += ../include \
               D:\Programmes\boost_1_85_0

SOURCES = testalgorithmrandomslicer.cpp \
          ../src/Labyrinth2d/Algorithm/CellFusion.cpp \
          ../src/Labyrinth2d/Algorithm/Kruskal.cpp \
          ../src/Labyrinth2d/Algorithm/TimeoutException.cpp \
          ../src/Labyrinth2d/Algorithm/WaySearch.cpp \
          ../src/Labyrinth2d/Solver/FailureException.cpp \
          ../src/Labyrinth2d/Solver/TimeoutException.cpp \
          ../src/Labyrinth2d/Solver/WallHand.cpp \
          ../src/Labyrinth2d/Renderer/String.cpp \
          ../src/Labyrinth2d/Grid.cpp \
          ../src/Labyrinth2d/Labyrinth.cpp \
          ../src/Labyrinth2d/Player.cpp \
          ../src/Labyrinth3d/Algorithm/TimeoutException.cpp \
          ../src/Labyrinth3d/Solver/FailureException.cpp \
          ../src/Labyrinth3d/Solver/TimeoutException.cpp \
          ../src/Labyrinth3d/Grid.cpp \
          ../src/Labyrinth3d/Labyrinth.cpp \
          ../src/Labyrinth3d/Player.cpp

HEADERS = ../include/Labyrinth2d/Algorithm/Algorithm.h \
          ../include/Labyrinth2d/Algorithm/CellFusion.h \
          ../include/Labyrinth2d/Algorithm/Fractal.h \
          ../include/Labyrinth2d/Algorithm/Kruskal.h \
          ../include/Labyrinth2d/Algorithm/RecursiveDivision.h \
          ../include/Labyrinth2d/Algorithm/WaySearch.h \
          ../include/Labyrinth2d/Algorithm/TimeoutException.h \
          ../include/Labyrinth2d/Renderer/String.h \
          ../include/Labyrinth2d/Solver/AStar.h \
          ../include/Labyrinth2d/Solver/Blind.h \
          ../include/Labyrinth2d/Solver/FailureException.h \
          ../include/Labyrinth2d/Solver/Solver.h \
          ../include/Labyrinth2d/Solver/TimeoutException.h \
          ../include/Labyrinth2d/Solver/WallHand.h \
          ../include/Labyrinth2d/Grid.h \
          ../include/Labyrinth2d/Labyrinth.h \
          ../include/Labyrinth2d/Player.h \
          ../include/Labyrinth2d/utility.h \
          ../include/Labyrinth3d/Algorithm/RandomSlicer.h \
          ../include/Labyrinth3d/Algorithm/TimeoutException.h \
          ../include/Labyrinth3d/Solver/AStar.h \
          ../include/Labyrinth3d/Solver/Blind.h \
          ../include/Labyrinth3d/Solver/FailureException.h \
          ../include/Labyrinth3d/Solver/Solver.h \
          ../include/Labyrinth3d/Solver/TimeoutException.h \
          ../include/Labyrinth3d/Grid.h \
          ../include/Labyrinth3d/Labyrinth.h \
          ../include/Labyrinth3d/Player.h \
          ../include/Labyrinth3d/utility.h

