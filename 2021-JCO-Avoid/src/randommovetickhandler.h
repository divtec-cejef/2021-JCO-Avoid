/**
  \file
  \brief    Déclaration de la classe RandomMoveTickHandler.
  \author   JCO
  \date     août 2020
*/

#ifndef RANDOMMOVETICKHANDLER_H
#define RANDOMMOVETICKHANDLER_H

#include "spritetickhandler.h"
#include "player.h"

//! \brief Classe qui gère un déplacement aléatoire d'un sprite
//!
//! Le sprite est déplacer tout en cherchant à éviter les collisions.
//!
//! Si la destruction sur collision est activée (setDestroyOnCollisionEnabled),
//! en cas de collision le sprite est détruit.
class RandomMoveTickHandler : public SpriteTickHandler
{
public:

    RandomMoveTickHandler(Sprite* pParentSprite = nullptr);
    RandomMoveTickHandler(bool ignoreCollision, bool destroyOnCollision, Sprite* pParentSprite = nullptr);

    virtual void init();
    virtual void tick(long long elapsedTimeInMilliseconds);

    void setSpriteVelocity(double velocityInPixelPerSecond);
    void setIgnoreCollisionEnabled(bool ignoreCollision);
    bool isIgnoreCollisionEnable() const;
    void setDestroyOnCollisionEnabled(bool destroyOnCollision);
    bool isDestroyedOnCollision() const;

private:
    void initNextMove();

    static bool s_seeded;

    void configureAnimation();

    bool m_ignoreCollision = false;
    bool m_destroyOnCollision = false;
    Player* m_pPlayer;

    long m_moveDuration = 0; // en millisecondes
    double m_moveAngle = 0;  // en radians
    double m_spriteVelocity; // en pixels/seconde
    int m_playerDirection = 1;

};

#endif // RANDOMMOVETICKHANDLER_H
