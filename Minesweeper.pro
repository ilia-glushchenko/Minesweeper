QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Minesweeper
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    cell.cpp \
    minesweeper.cpp

HEADERS += \
    cell.h \
    minesweeper.h
