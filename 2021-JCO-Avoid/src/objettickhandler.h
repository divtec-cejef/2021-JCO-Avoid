/**
  \file
  \brief    Déclaration de la classe RandomMoveTickHandler.
  \author   JCO
  \date     août 2020
*/

#ifndef OBJETTICKHANDLER_H
#define OBJETTICKHANDLER_H
#include <QElapsedTimer>
#include <QObject>
#include "spritetickhandler.h"
#include "player.h"


//! \brief Classe qui gère le déplacement vers le bas des sprites
//!
//! Le sprite s'occupe seulement de se déplacer vers le bas
//!
//! en cas de collision avec le bord bas de la scène le sprite est détruit
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
    QTimer m_tickTimerAugmentationObstacle;

private slots:
    void augmentationVitesseObstacle();

};

#endif // OBJETTICKHANDLER_H
