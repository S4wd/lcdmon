#-------------------------------------------------
#
# Project created by QtCreator 2015-02-05T12:22:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = s4wd
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
        busagent.cpp \
        graphicsitems.cpp \
	logger.cpp \
    ioagent.cpp \
    poweragent.cpp

HEADERS  += widget.h \
        busagent.h \
        datastructs.h \
        graphicsitems.h \
        logger.h \ 
	s4wd.h \
    ioagent.h \
    poweragent.h

FORMS    += widget.ui

RESOURCES += \
    icons.qrc



#LIBS += -L/usr/src/1w/builds/uLinuxGNU -ls4wd
