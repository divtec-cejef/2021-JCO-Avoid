#-------------------------------------------------
#
# Project created by QtCreator 2014-01-31T13:38:06
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Avoid
TEMPLATE = app

#DEFINES += DEBUG_SPRITE_COUNT
#DEFINES += DEBUG_BBOX
#DEFINES += DEBUG_SHAPE
#DEFINES += DEPLOY # Pour une compilation dans un but de déploiement

SOURCES += main.cpp\
    bouton.cpp \
    keytickhandler.cpp \
        mainfrm.cpp \
    gamescene.cpp \
    player.cpp \
    randommovetickhandler.cpp \
    sprite.cpp \
    gamecore.cpp \
    resources.cpp \
    gameview.cpp \
    utilities.cpp \
    gamecanvas.cpp \
    walkingman.cpp \
    spritetickhandler.cpp \
    manualwalkinghandler.cpp

HEADERS  += mainfrm.h \
    bouton.h \
    gamescene.h \
    keytickhandler.h \
    player.h \
    randommovetickhandler.h \
    sprite.h \
    gamecore.h \
    resources.h \
    gameview.h \
    utilities.h \
    gamecanvas.h \
    walkingman.h \
    spritetickhandler.h \
    manualwalkinghandler.h

FORMS    += mainfrm.ui
