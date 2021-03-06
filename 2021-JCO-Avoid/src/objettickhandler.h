/**
  \file
  \brief    Déclaration de la classe RandomMoveTickHandler.
  \author   PAPEFAB
  \date     décembre
*/

#ifndef OBJETTICKHANDLER_H
#define OBJETTICKHANDLER_H
#include <QElapsedTimer>
#include <QObject>
#include "spritetickhandler.h"
#include "player.h"


//! \brief Classe qui gère le déplacement vers le bas des sprites
//!
//! Le sprite s'occupe de se déplacer vers le bas et de détecter les collisions
//!
//! en cas de collision avec le bord bas de la scène le sprite est détruit
//!
//! La vitesse des sprite s'accélèrent au fur et à mesure que le temps passe
class ObjetTickHandler : public QObject, public SpriteTickHandler
{

    Q_OBJECT

public:
    enum { KEEP_PREVIOUS_TICK_INTERVAL = -1  };

    ObjetTickHandler(Sprite* pParentSprite = nullptr);

    virtual void tick(long long elapsedTimeInMilliseconds);
    void startTimerVitesseObstacle(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);
    void setDestroyOnCollisionEnabled(bool destroyOnCollision);

    static qreal s_vitesseObtsacle;

private:

    bool m_destroyOnCollision = false;

    bool m_keepTicking;
    QElapsedTimer m_lastUpdateTime;
    QTimer m_tickTimerAugmentationVitesseObstacle;

    const int AUGMENTATION_VITESSE_INTERVAL = 1000;

private slots:
    void augmentationVitesseObstacle();

};

#endif // OBJETTICKHANDLER_H
