#-------------------------------------------------
#
# Project created by QtCreator 2015-02-13T22:37:29
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = garbcol
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++11

TEMPLATE = app


SOURCES += main.cpp \
    gc/gccollector.cpp \
    gc/gccontainer.cpp \
    gc/gcobject.cpp \
    gc/gcpointer.cpp

HEADERS += \
    gc/gc.h \
    gc/gcarray.h \
    gc/gccontainer.h \
    gc/gcdeque.h \
    gc/gclist.h \
    gc/gcmap.h \
    gc/gcobject.h \
    gc/gcpointer.h \
    gc/gcsequence.h \
    gc/gcset.h \
    gc/gcvector.h