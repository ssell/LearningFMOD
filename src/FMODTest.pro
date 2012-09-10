#-------------------------------------------------
#
# Project created by QtCreator 2012-09-05T15:19:38
#
#-------------------------------------------------

QT       += core gui

TARGET = FMODTest
TEMPLATE = app

#-------------------------------------------------
#-------------------------------------------------

LIBS += -L/share/users/ssell/Desktop/fmodapi44203linux64/api/lib \
        -lfmodex64

INCLUDEPATH += /share/users/ssell/Desktop/fmodapi44203linux64/api/inc

#-------------------------------------------------
#-------------------------------------------------

SOURCES += main.cpp\
        mainwindow.cpp \
    fmod_resources.cpp

HEADERS  += mainwindow.h \
    fmod_resources.h

FORMS    += mainwindow.ui
