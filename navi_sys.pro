
QT       += core gui
QT       += serialport
QT       += network
QT       += webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = navi_sys
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

CONFIG   += C++11

DISTFILES +=

RESOURCES += \
    map.qrc



