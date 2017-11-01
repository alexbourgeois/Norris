#-------------------------------------------------
#
# Project created by QtCreator 2017-05-24T09:53:35
#
#-------------------------------------------------
include(Plugins\qxtglobalshortcut5\qxt.pri)

QT       += core gui network
QT += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ClipboardAssistant-cpp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    bucket.cpp \
    node.cpp \
    nodewidget.cpp \
    downloadmanager.cpp \
    settings.cpp \
    mimedata.cpp

HEADERS  += mainwindow.h \
    bucket.hpp \
    node.h \
    nodewidget.h \
    downloadmanager.h \
    settings.h \
    mimedata.h

FORMS    += mainwindow.ui \
    node.ui \
    trash.ui \
    bucket.ui

RESOURCES += \
    rsc.qrc
