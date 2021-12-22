/**
  \file
  \brief    Définition de la classe RandomMoveTickHandler.
  \author   JCO
  \date     août 2020
*/
#include "objettickhandler.h"

#include <cstdlib>
#include <ctime>
#include <QtMath>
#include <QPointF>
#include <QRectF>
#include <QTime>
#include <QTimer>
#include <time.h>

#include <chrono>
#include <thread>

#include "sprite.h"
#include "gamescene.h"
#include "player.h"
#include "gamecore.h"
#include "resources.h"

qreal ObjetTickHandler::s_vitesseObtsacle = 7;

//! Constructeur.
//! \param pParentSprite Sprite dont le déplacement doit être géré.
ObjetTickHandler::ObjetTickHandler(Sprite* pParentSprite) : SpriteTickHandler (pParentSprite)
{

    //Mise en place et démarrage du timer pour l'augmentation de la vitesse des sprites
    m_tickTimerAugmentationVitesseObstacle.setSingleShot(false);
    m_tickTimerAugmentationVitesseObstacle.setInterval(AUGMENTATION_VITESSE_INTERVAL);
    m_tickTimerAugmentationVitesseObstacle.setTimerType(Qt::PreciseTimer); // Important pour avoir un précision suffisante sous Windows
    connect(&m_tickTimerAugmentationVitesseObstacle, SIGNAL(timeout()), this, SLOT(augmentationVitesseObstacle()));

    m_tickTimerAugmentationVitesseObstacle.start();

}

//! Cadence : détermine le mouvement que fait le sprite durant le temps écoulé.
void ObjetTickHandler::tick(long long elapsedTimeInMilliseconds) {
    // Création d'un vecteur de déplacement du sprite.
    QPointF spriteMovement(0, s_vitesseObtsacle);
    // Détermine la prochaine position du sprite
    QRectF nextSpriteRect = m_pParentSprite->globalBoundingBox().translated(spriteMovement);
    // Récupère tous les sprites de la scène que toucherait ce sprite à sa prochaine position
    auto collidingSprites = m_pParentSprite->parentScene()->collidingSprites(nextSpriteRect);
    // Supprimer le sprite lui-même, qui collisionne toujours avec sa boundingbox
    collidingSprites.removeAll(m_pParentSprite);

    // Si la prochaine position du sprite n'est pas comprise au sein de la scène,
    // le sprite n’est pas déplacé et est détruit
    if (!m_pParentSprite->parentScene()->isInsideScene(nextSpriteRect))
        m_pParentSprite->deleteLater();
    else
        // Si le sprite ne sort pas de la scène, on le déplace
        // (en lui appliquant le vecteur de déplacement)
        m_pParentSprite->setPos(m_pParentSprite->pos() + spriteMovement);

}

/**
 * Configuration timer pour gérer la vitesse de déplacement des sprite
 * @brief GameCore::startSpawnObstacleTimer
 * @param tickInterval
 */
void ObjetTickHandler::startTimerVitesseObstacle(int tickInterval)  {

    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerAugmentationVitesseObstacle.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerAugmentationVitesseObstacle.start();
}

/**
 * Augmente la vitesse de l'obstacle
 * @brief ObjetTickHandler::augmentationVitesseObstacle
 */
void ObjetTickHandler::augmentationVitesseObstacle(){
    s_vitesseObtsacle+=0.1;
}

//! Indique si le sprite piloté par ce gestionnaire doit être détruit en cas de collision.
//! \param destroyOnCollision  Indique si une collision provoque la destruction du sprite.
void ObjetTickHandler::setDestroyOnCollisionEnabled(bool destroyOnCollision) {
    m_destroyOnCollision = destroyOnCollision;
}


