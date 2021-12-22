/**
  \file
  \brief    Déclaration de la classe KeyTickHandler.
  \author   JCO
  \date     août 2020
*/

#ifndef KEYTICKHANDLER_H
#define KEYTICKHANDLER_H

#include <QObject>
#include <QPointF>

#include "spritetickhandler.h"

//! \brief Classe qui gère le déplacement par d'un sprite au moyen des flèches
//! du clavier.
//!

class GameCore;

class KeyTickHandler : public QObject, public SpriteTickHandler
{
    Q_OBJECT

public:
    KeyTickHandler(GameCore* pGameCore, Sprite* pParentSprite = nullptr);





};

#endif // KEYTICKHANDLER_H
