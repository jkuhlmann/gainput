QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-example
TEMPLATE = app

LIBS += -L$${OUT_PWD}/../../lib -lgainput
win32:LIBS += -lws2_32 -luser32 -lxinput

INCLUDEPATH += $${PWD}/../../lib/include

SOURCES += main.cpp widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui
