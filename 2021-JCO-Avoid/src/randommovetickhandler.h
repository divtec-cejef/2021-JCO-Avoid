/**
  \file
  \brief    Déclaration de la classe RandomMoveTickHandler.
  \author   JCO
  \date     août 2020
*/

#ifndef RANDOMMOVETICKHANDLER_H
#define RANDOMMOVETICKHANDLER_H
#include <QElapsedTimer>
#include <QObject>
#include "spritetickhandler.h"
#include "player.h"


//! \brief Classe qui gère un déplacement aléatoire d'un sprite
//!
//! Le sprite est déplacer tout en cherchant à éviter les collisions.
//!
//! Si la destruction sur collision est activée (setDestroyOnCollisionEnabled),
//! en cas de collision le sprite est détruit.
class RandomMoveTickHandler : public QObject, public SpriteTickHandler
{

    Q_OBJECT
//signals:
    //void onplayerDestroyed();

public:
    enum { KEEP_PREVIOUS_TICK_INTERVAL = -1  };

    RandomMoveTickHandler(Sprite* pParentSprite = nullptr);
    RandomMoveTickHandler(bool ignoreCollision, bool destroyOnCollision, Sprite* pParentSprite = nullptr);

    virtual void init();
    virtual void tick(long long elapsedTimeInMilliseconds);

    void setSpriteVelocity(double velocityInPixelPerSecond);
    void setIgnoreCollisionEnabled(bool ignoreCollision);
    bool isIgnoreCollisionEnable() const;
    void setDestroyOnCollisionEnabled(bool destroyOnCollision);
    bool isDestroyedOnCollision() const;
    void startTimerVitesseObstacle(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);


    static qreal s_vitesseObtsacle;
private:
    void initNextMove();

    static bool s_seeded;


    void configureAnimation();

    bool m_keepTicking;
    QElapsedTimer m_lastUpdateTime;


    bool m_ignoreCollision = false;
    bool m_destroyOnCollision = false;
    Player* m_pPlayer;
    Sprite* m_pMort;


    QTimer m_tickTimerAugmentationObstacle;

    long m_moveDuration = 0; // en millisecondes
    double m_moveAngle = 0;  // en radians
    double m_spriteVelocity; // en pixels/seconde
    int m_playerDirection = 1;

private slots:
    void augmentationVitesseObstacle();

};

#endif // RANDOMMOVETICKHANDLER_H
