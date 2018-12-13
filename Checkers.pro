TEMPLATE = app
CONFIG += c++1z
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lglut -lGLU -lGL -lGLEW
SOURCES += \
        main.cpp \
    game.cpp \
    alphabeta.cpp \
    board.cpp

HEADERS += \
    game.hpp \
    alphabeta.hpp \
    board.hpp
