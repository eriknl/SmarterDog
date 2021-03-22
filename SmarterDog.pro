#This file is part of SmarterDog (C) 2017 Erik de Jong

#SmarterDog is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#SmarterDog is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with SmarterDog. If not, see <https://www.gnu.org/licenses/>

QT += core network
QT -= gui

CONFIG += c++11

TARGET = SmarterDog
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    cleverdogudp.cpp \
    mediarecaster.cpp \
    rtsphandler.cpp \
    smarterdog.cpp \
    cleverdogcamera.cpp \
    rtspconnection.cpp \
    cleverdogbackend.cpp \
    cleverdogbackendconnection.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    cleverdogudp.h \
    mediarecaster.h \
    rtsphandler.h \
    smarterdog.h \
    cleverdogcamera.h \
    rtspconnection.h \
    cleverdogbackend.h \
    cleverdogbackendconnection.h

DISTFILES += \
    LICENSE \
    README.md
