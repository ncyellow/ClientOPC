#-------------------------------------------------
#
# Project created by QtCreator 2015-05-18T09:40:27
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ClientOPC
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += "c:/usr/app/WinDDK/7600.16385.1/inc/atl71"
INCLUDEPATH += "C:/usr/lib/boost_1_57_0"

SOURCES += main.cpp \
    opc/opccomn_i.c \
    opc/opcda_i.c \
    opc/OpcEnum_i.c \
    clientopc.cpp \
    server.cpp \
    configreader.cpp




HEADERS += \
    opc/opccomn.h \
    opc/opcda.h \
    opc/OpcEnum.h \
    clientopc.h \
    server.h \
    configreader.h

LIBS += -L"c:/usr/app/WinDDK/7600.16385.1/lib/ATL/i386" -lAdvapi32 -Oleaut32
