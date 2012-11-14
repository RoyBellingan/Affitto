#-------------------------------------------------
#
# Project created by QtCreator 2012-10-25T23:49:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mamma
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cclassseriallib_modem.cpp \
    cclassseriallib.cpp

HEADERS  += mainwindow.h \
    cclassseriallib_modem.h \
    cclassseriallib.h

FORMS    += mainwindow.ui

linux-* {
    target.path = /opt/Mamma/bin
    INSTALLS += target
}
