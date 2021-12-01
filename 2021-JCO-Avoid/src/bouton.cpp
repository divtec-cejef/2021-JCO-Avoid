#include "bouton.h"

#include "resources.h"
#include "gamescene.h"
#include <QDebug>



Bouton::Bouton(QGraphicsItem* pParent) : Sprite(GameFramework::imagesPath() + "replay.png", pParent)
{

}


//! Traite le clique de la souris.



