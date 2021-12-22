/**
  \file
  \brief    Définition de la classe KeyTickHandler.
  \author   JCO
  \date     août 2020
*/
#include "keytickhandler.h"

#include "gamescene.h"
#include "gamecore.h"

//! Constructeur
//! \param pGameCore        Pointeur sur le GameCore qui signal l'appui sur les touches du clavier.
//! \param pParentSprite    Sprite dont le déplacement doit être géré.
KeyTickHandler::KeyTickHandler(GameCore* pGameCore, Sprite* pParentSprite) : SpriteTickHandler(pParentSprite)
{

}





