QT += testlib

CONFIG += object_parallel_to_source

INCLUDEPATH += ../include

SOURCES = testrendererqpainter.cpp \
          ../src/Labyrinth2d/Algorithm/CellFusion.cpp \
          ../src/Labyrinth2d/Algorithm/TimeoutException.cpp \
          ../src/Labyrinth2d/Renderer/QPainter.cpp \
          ../src/Labyrinth2d/Grid.cpp \
          ../src/Labyrinth2d/Labyrinth.cpp \
          ../src/Labyrinth2d/Player.cpp

HEADERS = ../include/Labyrinth2d/Algorithm/CellFusion.h \
          ../include/Labyrinth2d/Algorithm/TimeoutException.h \
          ../include/Labyrinth2d/Renderer/QPainter.h \
          ../include/Labyrinth2d/Grid.h \
          ../include/Labyrinth2d/Labyrinth.h \
          ../include/Labyrinth2d/Player.h \
          ../include/Labyrinth2d/utility.h

