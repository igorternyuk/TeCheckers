TEMPLATE = app
DEFINES += DEBUG
CONFIG += c++23
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lglut -lGLU -lGL -lGLEW -lpthread
SOURCES += \
        main.cpp \
    game.cpp \
    alphabeta.cpp \
    board.cpp \
    utils.cpp

HEADERS += \
    game.hpp \
    alphabeta.hpp \
    board.hpp \
    gamestatus.hpp \
    utils.hpp
