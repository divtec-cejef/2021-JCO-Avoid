#include "obstacle.h"
#include "gamescene.h"
#include <QDebug>
#include "resources.h"

obstacle::obstacle(QGraphicsItem* pParent) : Sprite(GameFramework::imagesPath() + "personnage.png", pParent)
{

}

void obstacle::tick(long long elapsedTimeInMilliseconds) {

}

void obstacle::lengthUpdate(){

}

void obstacle::obstacleSpawn(){

}

void obstacle::obstacleDestroy(){

}
