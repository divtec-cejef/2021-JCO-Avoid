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
    obstacle.cpp \
    player.cpp \
    playertickhandler.cpp \
    randommovetickhandler.cpp \
    sprite.cpp \
    gamecore.cpp \
    resources.cpp \
    gameview.cpp \
    utilities.cpp \
    gamecanvas.cpp \
    walkingman.cpp \
    spinningpinwheel.cpp \
    spritetickhandler.cpp \
    bouncingspritehandler.cpp \
    manualwalkinghandler.cpp \
    automaticwalkinghandler.cpp

HEADERS  += mainfrm.h \
    bouton.h \
    gamescene.h \
    keytickhandler.h \
    obstacle.h \
    player.h \
    playertickhandler.h \
    randommovetickhandler.h \
    sprite.h \
    gamecore.h \
    resources.h \
    gameview.h \
    utilities.h \
    gamecanvas.h \
    walkingman.h \
    spinningpinwheel.h \
    spritetickhandler.h \
    bouncingspritehandler.h \
    manualwalkinghandler.h \
    automaticwalkinghandler.h

FORMS    += mainfrm.ui
