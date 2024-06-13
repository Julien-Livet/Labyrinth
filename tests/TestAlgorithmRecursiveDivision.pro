QT += testlib

CONFIG += object_parallel_to_source

INCLUDEPATH += ../include

SOURCES = testalgorithmrecursivedivision.cpp \
          ../src/Labyrinth2d/Algorithm/TimeoutException.cpp \
          ../src/Labyrinth2d/Renderer/String.cpp \
          ../src/Labyrinth2d/Solver/FailureException.cpp \
          ../src/Labyrinth2d/Solver/TimeoutException.cpp \
          ../src/Labyrinth2d/Solver/WallHand.cpp \
          ../src/Labyrinth2d/Grid.cpp \
          ../src/Labyrinth2d/Labyrinth.cpp \
          ../src/Labyrinth2d/Player.cpp

HEADERS = ../include/Labyrinth2d/Algorithm/RecursiveDivision.h \
          ../include/Labyrinth2d/Algorithm/TimeoutException.h \
          ../include/Labyrinth2d/Renderer/String.h \
          ../include/Labyrinth2d/Solver/AStar.h \
          ../include/Labyrinth2d/Solver/Blind.h \
          ../include/Labyrinth2d/Solver/FailureException.h \
          ../include/Labyrinth2d/Solver/TimeoutException.h \
          ../include/Labyrinth2d/Solver/WallHand.h \
          ../include/Labyrinth2d/Grid.h \
          ../include/Labyrinth2d/Labyrinth.h \
          ../include/Labyrinth2d/Player.h \
          ../include/Labyrinth2d/utility.h

