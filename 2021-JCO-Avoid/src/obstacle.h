#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "sprite.h"


class obstacle : public Sprite
{
public:
    obstacle(QGraphicsItem* pParent = nullptr);
    virtual void tick(long long elapsedTimeInMilliseconds);
    void lengthUpdate();
    void obstacleSpawn();
    void obstacleDestroy();





private :
    int speed;
    int length;

};

#endif // OBSTACLE_H
